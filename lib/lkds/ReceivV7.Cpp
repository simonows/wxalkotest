#include "stdafx.h"
#include "ReceivV7.h"
#include "SendV7.H"
#include "Xml.h"
#include "LoadRes.h"
#include "resource.h"

const int CReceivV7::s_IdentCloud	= 9999997;	// идентификатор LKDSCloud

CReceivV7::CReceivV7() 
	: m_hWinSockThread(0)
	, m_dwThreadWinSockID(0)
	, m_bOpen(false)
	, m_Socket(0)
	, m_LastLinkFromCloud(0)
{
	::InitializeCriticalSection(&m_Lock);
}

CReceivV7::~CReceivV7() 
{
	::DeleteCriticalSection(&m_Lock);
}

void	CReceivV7::WaitEndThread(volatile HANDLE& hThread, const TCHAR* aMesErr)
{
	while(hThread)	Sleep(1);
}

int	CReceivV7::Send(const BYTE* aBody, int aLen, DWORD aIPAddr, int aIPPort)
{
	::EnterCriticalSection(&m_Lock);

	int rc = aLen;

	if(aIPAddr != 0)
	{
		sockaddr_in	Server;

		memset(&Server,0,sizeof(Server));

		Server.sin_family			= AF_INET;
		Server.sin_addr.S_un.S_addr	= aIPAddr;
		Server.sin_port				= htons(aIPPort);

		rc = sendto(m_Socket, reinterpret_cast<const char*>(aBody), aLen, 0, (SOCKADDR*)&Server, sizeof(Server));

		if( rc == SOCKET_ERROR)
		{
			BadPutErr(_T("CReceivV7::Send sendto()"), WSAGetLastError());
		}
	}
	::LeaveCriticalSection(&m_Lock);

	return rc;
}

int	CReceivV7::Bind(CString& aMesErr)
{
	m_Socket = socket(AF_INET, SOCK_DGRAM, 0 );

	if (m_Socket == INVALID_SOCKET)
	{
		int	err	= WSAGetLastError();

		BadPutErr(_T("CReceivV7::Bind(): socket()"), err);
		return err;
	}
	sockaddr_in local;

	local.sin_family			= AF_INET;
	local.sin_port				= htons(m_PortOwn.GetValue());
	local.sin_addr.S_un.S_addr	= m_AddrOwn.GetValue(); 

	if(bind(m_Socket, (struct sockaddr*)&local, sizeof(local) ) == SOCKET_ERROR) 
	{
		int	err	= WSAGetLastError();

		closesocket(m_Socket);

		BadPutErr(_T("CReceivV7::Bind(): bind()"), err);
		return err;
	}

	return 0;
}

void	CReceivV7::Start()
{
	try
	{
		int		rc = 0;
		CString	MesErr;

		if((rc = Bind(MesErr)) != 0)	throw ErrorXML(MesErr, rc);

		m_bOpen	= true;

		m_hWinSockThread = ::CreateThread(	NULL, 0, 
											(LPTHREAD_START_ROUTINE) WinSockProc,
											this, 0, &m_dwThreadWinSockID);
		if(!m_hWinSockThread)	throw ErrorXML(_T("Error start CReceivV7::WinSockProc"), GetLastError());
	}
	catch(ErrorXML aErr)
	{
		m_bOpen	= false;

		throw	aErr;
	}
}

void	CReceivV7::Stop()
{
	m_bOpen	= false;

	closesocket(m_Socket);

	WaitEndThread(m_hWinSockThread,_S_(IDS_V7_NOEND)+_T("CReceivV7::WinSockProc"));
}

bool	CReceivV7::ConnReset(const TCHAR* aIpPort, int aErr)
{
	BadPutErr(aIpPort+_S_(IDS_V7_BREAK_CONN), aErr);

	return true;
}

DWORD WINAPI CReceivV7::WinSockProc( LPVOID lpData )
{
	CReceivV7*		pSock	= (CReceivV7*)lpData;
	CString			buf;
	sockaddr_in		from;
	int				fromlen = sizeof(from);
	BYTE			Buffer[CPackV7::mxLen];

	while(pSock->m_bOpen)
	{
		int	rc = recvfrom(pSock->m_Socket, reinterpret_cast<char*>(Buffer), sizeof(Buffer), 0, (sockaddr *)&from, &fromlen);

		if(!pSock->m_bOpen)	break;

		if(rc == SOCKET_ERROR) 
		{
			int	err	= WSAGetLastError();

			if(err == WSAECONNRESET)
			{				// клиент разорвал соединение
				buf.Format(_T("%d.%d.%d.%d:%u - ")
					, from.sin_addr.S_un.S_un_b.s_b1
					, from.sin_addr.S_un.S_un_b.s_b2
					, from.sin_addr.S_un.S_un_b.s_b3
					, from.sin_addr.S_un.S_un_b.s_b4
					, ntohs(from.sin_port));

				if(pSock->ConnReset(buf, err))	continue;
				else							break;
			}
			else if(err == WSAEINTR)
			{				// прерывание ожидания
				closesocket(pSock->m_Socket);
				break;
			}
			else
			{
				CString	MesErr;

				pSock->BadPutErr(_T("CReceivV7::WinSockProc recvfrom()"), err);

				::EnterCriticalSection(&(pSock->m_Lock));
				closesocket(pSock->m_Socket);
				Sleep(1*1000);
				int rcBind = pSock->Bind(MesErr);
				::LeaveCriticalSection(&(pSock->m_Lock));

				if(rcBind == 0)	continue;
				break;
			}
		}
		else if(rc == 0) 
		{
			buf.Format(_T("%d.%d.%d.%d:%u - ")
				, from.sin_addr.S_un.S_un_b.s_b1
				, from.sin_addr.S_un.S_un_b.s_b2
				, from.sin_addr.S_un.S_un_b.s_b3
				, from.sin_addr.S_un.S_un_b.s_b4
				, ntohs(from.sin_port));

			if(pSock->ConnReset(buf, 0))	continue;
			else							break;
		}
		
//pSock->LogPut(0, from.sin_addr.S_un.S_addr, ntohs(from.sin_port), Buffer, rc);	// !!! для отладки

		if(rc >= CPackV7::minLen)
		{
			bool	isChanDecod	= false;
			bool	isFromCloud	= false;
			CPackV7	PackInp;
			CKeySys	Key	= pSock->m_Key;

			isChanDecod	= PackInp.ChanDecod(Buffer, rc, Key);

			if(!isChanDecod)	isChanDecod	= pSock->ChanDecod(Buffer, rc, PackInp, Key, isFromCloud);

			if(isChanDecod)
			{
				CSendV7* pSendV7 = pSock->GetSend(PackInp.m_Orig);

				//if(!pSendV7) 12.07.2016
				if(!pSendV7 && !PackInp.IsCloseLink())
				{
					pSendV7 = pSock->NewSender(PackInp.m_Orig, from.sin_addr.S_un.S_addr, htons(from.sin_port), Key, pSock); // 13.08.2015
				}
				if(pSendV7)
				{		// найден отправитель
					if(!pSendV7->m_Disable)
					{
						pSock->LogPut(pSendV7, from.sin_addr.S_un.S_addr, ntohs(from.sin_port), PackInp, PackInp.LenCRC());

						if(pSendV7->m_Ident != pSock->m_Ident)	// 30.01.2014
						{
							if(pSendV7->m_StaticOnly)
							{
								if(	pSendV7->m_AddrRemote	!= from.sin_addr.S_un.S_addr)
								{
									CIpAddr	IP;
									IP.SetValue(pSendV7->m_AddrRemote);

									//buf.Format(_T("Receive data from device %s from IP %d.%d.%d.%d, device have IP ")
									buf.Format(_S_(IDS_IGNORE_FROM_NO_STATIC_IP)
										, static_cast<const TCHAR*>(PackInp.m_Orig.GetString())
										, from.sin_addr.S_un.S_un_b.s_b1
										, from.sin_addr.S_un.S_un_b.s_b2
										, from.sin_addr.S_un.S_un_b.s_b3
										, from.sin_addr.S_un.S_un_b.s_b4);

									buf	+= IP.GetString();
									pSock->BadPut(buf, true);

									continue;	// 28.04.2011 - пакет, пришедший с другого IP, игнорируется
								}
							}
							else
							{
								if(	pSendV7->m_AddrRemote	!= from.sin_addr.S_un.S_addr || 
									pSendV7->m_PortRemote	!= ntohs(from.sin_port))
								{
									CIpAddr	IP;
									IP.SetValue(pSendV7->m_AddrRemote);

									//if(	pSock->m_Ident == PackInp.m_Dest || pSock->GetSend(PackInp.m_Dest) )	// пакет этому ПКЛШ или ретранслируется через этот ПКЛШ 09.02.2018
									if(	pSock->m_Ident == PackInp.m_Dest || pSock->GetSend(PackInp.m_Dest) || (pSendV7->m_LastChangedIPport == 0) )	// пакет этому ПКЛШ или ретранслируется через этот ПКЛШ или адрес еще не был назначен 21.03.2018
									{
										if( (time(0) - pSendV7->m_LastChangedIPport) <= 2)	//+ 08.02.2018
										{	// с предыдущей смены IP:port прошло меньше 3 сек
											//buf.Format(_T("New IP:port from device %s %d.%d.%d.%d:%u exist %s:%u - ignored, because time from previous < 3 sec")
											buf.Format(_S_(IDS_IGNORE_CHANGE_IP_PORT)
												, static_cast<const TCHAR*>(PackInp.m_Orig.GetString())
												, from.sin_addr.S_un.S_un_b.s_b1
												, from.sin_addr.S_un.S_un_b.s_b2
												, from.sin_addr.S_un.S_un_b.s_b3
												, from.sin_addr.S_un.S_un_b.s_b4
												, ntohs(from.sin_port)
												, static_cast<const TCHAR*>(IP.GetString())
												, pSendV7->m_PortRemote);

											pSock->BadPut(buf, true);
										}
										else
										{
											pSendV7->m_LastChangedIPport = time(0);			//- 08.02.2018

											//buf.Format(_T("Changed IP:port for device %s - new %d.%d.%d.%d:%u old %s:%u")
											buf.Format(_S_(IDS_CHANGE_IP_PORT)
												, static_cast<const TCHAR*>(PackInp.m_Orig.GetString())
												, from.sin_addr.S_un.S_un_b.s_b1
												, from.sin_addr.S_un.S_un_b.s_b2
												, from.sin_addr.S_un.S_un_b.s_b3
												, from.sin_addr.S_un.S_un_b.s_b4
												, ntohs(from.sin_port)
												, static_cast<const TCHAR*>(IP.GetString())
												, pSendV7->m_PortRemote);

											pSock->BadPut(buf, true);

											pSendV7->m_AddrRemote	= from.sin_addr.S_un.S_addr;
											pSendV7->m_PortRemote	= ntohs(from.sin_port);

											pSock->IsChangIP(pSendV7);
										}
									}
									else
									{
										//buf.Format(_T("New IP:port from device %s %d.%d.%d.%d:%u exist %s:%u - ignored, because not known recipient: %s")
										buf.Format(_S_(IDS_IGNORE_IP_PORT_TO_IDENT)
											, static_cast<const TCHAR*>(PackInp.m_Orig.GetString())
											, from.sin_addr.S_un.S_un_b.s_b1
											, from.sin_addr.S_un.S_un_b.s_b2
											, from.sin_addr.S_un.S_un_b.s_b3
											, from.sin_addr.S_un.S_un_b.s_b4
											, ntohs(from.sin_port)
											, static_cast<const TCHAR*>(IP.GetString())
											, pSendV7->m_PortRemote
											, static_cast<const TCHAR*>(PackInp.m_Dest.GetString()));

										pSock->BadPut(buf, true);
									}
								}
							}

							if(pSock->m_Ident == PackInp.m_Dest)
							{		// получатель мы
								//pSendV7->m_Wind.MngReply(PackInp.m_Reply); 12.07.2017
								pSendV7->MngReply(PackInp.m_Reply);

								if(PackInp.m_Ident != 0)
								{
									if(!(pSendV7->MngIdentInp(PackInp.m_Ident))) 
									{					// повторно присланный пакет, который не нужно обрабатывать, но на него нужно послать Reply
										pSendV7->SendReply();	// 14.03.2016
										continue;
									}
								}
								pSock->HandlingData(PackInp, pSendV7);

								pSendV7->SendReply();
							}
							else
							{		// ретрансляция другим получателям
								if(!pSendV7->Retransmission(pSock, PackInp, from.sin_addr.S_un.S_addr, htons(from.sin_port)))
								{
									//buf.Format(L"Not known recipient: %s -> %s from IP %d.%d.%d.%d"
									buf.Format(_S_(IDS_NO_KNOWN_RECIPIENT)
										, static_cast<const TCHAR*>(PackInp.m_Orig.GetString())
										, static_cast<const TCHAR*>(PackInp.m_Dest.GetString())
										, from.sin_addr.S_un.S_un_b.s_b1
										, from.sin_addr.S_un.S_un_b.s_b2
										, from.sin_addr.S_un.S_un_b.s_b3
										, from.sin_addr.S_un.S_un_b.s_b4);
									pSock->BadPutMes(buf, PackInp, PackInp.Len(), true);
								}
							}
						}
						else
						{
							//buf.Format(_T("Pack from own Ident %s - %d.%d.%d.%d:%u")
							buf.Format(_S_(IDS_DATA_FROM_OWN_IDENT)
								, static_cast<const TCHAR*>(PackInp.m_Orig.GetString())
								, from.sin_addr.S_un.S_un_b.s_b1
								, from.sin_addr.S_un.S_un_b.s_b2
								, from.sin_addr.S_un.S_un_b.s_b3
								, from.sin_addr.S_un.S_un_b.s_b4
								, ntohs(from.sin_port));

							pSock->BadPutMes(buf, PackInp, PackInp.Len(), true);
						}
					}
					else
					{
						buf.Format(_T("Pack from disable link Ident %s - %d.%d.%d.%d:%u")
							, static_cast<const TCHAR*>(PackInp.m_Orig.GetString())
							, from.sin_addr.S_un.S_un_b.s_b1
							, from.sin_addr.S_un.S_un_b.s_b2
							, from.sin_addr.S_un.S_un_b.s_b3
							, from.sin_addr.S_un.S_un_b.s_b4
							, ntohs(from.sin_port));

						pSock->BadPutMes(buf, PackInp, PackInp.Len());
					}
				}
				else
				{
					CSendV7*	pSelf = pSock->GetSend(pSock->m_Ident);
					if(pSelf) pSock->LogPut(pSelf, from.sin_addr.S_un.S_addr, ntohs(from.sin_port), PackInp, PackInp.LenCRC());

					if(PackInp.m_Orig.GetNode() == s_IdentCloud)
					{
						pSock->m_LastLinkFromCloud = time(0);
					}
					else
					{
						buf.Format(_S_(IDS_V7_UNKN_IDENT)
							, static_cast<const TCHAR*>(PackInp.m_Orig.GetString())
							, from.sin_addr.S_un.S_un_b.s_b1
							, from.sin_addr.S_un.S_un_b.s_b2
							, from.sin_addr.S_un.S_un_b.s_b3
							, from.sin_addr.S_un.S_un_b.s_b4);
						pSock->BadPutMes(buf, PackInp, PackInp.Len(), true);
					}
				}
			}
			else
			{
				//buf.Format(_T("Bad Pro pack %u.%u.%u.%u:%u")
				buf.Format(_S_(IDS_BAD_INPUT_DATA)
					, from.sin_addr.S_un.S_un_b.s_b1
					, from.sin_addr.S_un.S_un_b.s_b2
					, from.sin_addr.S_un.S_un_b.s_b3
					, from.sin_addr.S_un.S_un_b.s_b4
					, ntohs(from.sin_port));

				pSock->BadPutMes(buf, Buffer, rc, true);
			}
		}
	}

	CloseHandle(pSock->m_hWinSockThread);
	pSock->m_hWinSockThread	= NULL;
	pSock->m_dwThreadWinSockID	= 0;

	pSock->m_bOpen	= false;

	return( TRUE ) ;
}
