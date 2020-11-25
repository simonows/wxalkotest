#include "stdafx.h"
#include <GGSV7.h>
#include <XML.h>

/////////////////////////////////////////////////////////////////////////////
// передача ГГС к ЛБ7

bool CGGSV7::PutSock(CPackV7& aPack)
{
	BYTE	Coding[CPackV7::mxLen];

	aPack.AddRepl(0);
	aPack.SetIdent(0);
	aPack.SetID(m_IdentOWN, m_IdentLB);
	int	Len = aPack.ChanCod(Coding, sizeof(Coding), m_Key);

	//int	Len = aPack.LenFul();
	//memcpy(Coding, static_cast<const BYTE*>(aPack), Len);

	sockaddr_in	Server;
	memset(&Server,0,sizeof(Server));
	Server.sin_family			= AF_INET;
	Server.sin_addr.S_un.S_addr	= m_AddrIPOut;
	Server.sin_port				= htons(m_PortOut);

	int	rc = sendto(m_Socket, reinterpret_cast<const char*>(Coding), Len, 0, (SOCKADDR*)&Server, sizeof(Server));

	return(rc != SOCKET_ERROR);
}

void CGGSV7::GetVolume(BYTE& aVolumIn, BYTE& aVolumOut)
{
	aVolumIn	= 7; 
	aVolumOut	= 7;
}

void CGGSV7::Start( const TCHAR* aPort, const CIdentV7& aIdentOWN, const CIdentV7& aIdentLB, DWORD aAddrIPInp, DWORD aAddrIPOut, unsigned short aPortInp, unsigned short aPortOut, const CKeySys& aKey)
{
	try
	{
		CString	buf;

		m_IdentOWN	= aIdentOWN;
		m_IdentLB	= aIdentLB;
		m_AddrIPInp	= aAddrIPInp;
		m_AddrIPOut	= aAddrIPOut;
		m_PortInp	= aPortInp;
		m_PortOut	= aPortOut;
		m_Key		= aKey;

		Open(aPort);

		m_Socket = socket(AF_INET, SOCK_DGRAM, 0 );

		if (m_Socket == INVALID_SOCKET)														throw ErrorXML(_T("CGGSV7::Start - socket()"), WSAGetLastError());
	    
		if(m_PortInp != 0)
		{
			sockaddr_in local;
			memset(&local,0,sizeof(local));

			local.sin_family			= AF_INET;
			local.sin_addr.S_un.S_addr	= m_AddrIPInp; 
			local.sin_port				= htons(m_PortInp);

			if(bind(m_Socket, (struct sockaddr*)&local, sizeof(local) ) == SOCKET_ERROR)		throw ErrorXML(_T("CGGSV7::Start - bind()"), WSAGetLastError());
		}
		else
		{
			CPackV7	Pack;
			
			Pack.SetLink(CPackV7::tpConfLink, 0, 0);
			
			PutSock(Pack);
		}
		m_hWinSockThread = ::CreateThread(	NULL, 0, 
											(LPTHREAD_START_ROUTINE) WinSockProc,
											this, 0, &m_dwThreadWinSockID);
		if(!m_hWinSockThread)																throw ErrorXML(_T("CGGSV7::Start - CreateThread"), GetLastError());

		{
			CPackV7	Pack;

			BYTE	Body[3];
			
			Body[0]	= CPackV7::tpG723;
			GetVolume(Body[1], Body[2]);

			Pack.SetAsk(CPackV7::tpDirect, CPackV7::taOnVoice, CPackV7::tdQuery, Body, sizeof(Body));

			PutCnv(Pack);	// запрос Включи ГГС ? голосовому конвертору
		}
	}
	catch(ErrorXML aErr)
	{
		Stop();
		throw aErr;
	}
}

void CGGSV7::Stop()
{
	CPackV7	Pack;

	Pack.SetAsk(CPackV7::tpDirect, CPackV7::taOffVoice, CPackV7::tdQuery);

	PutCnv(Pack);

	Close();

	closesocket(m_Socket);

	m_Socket	= 0;

	while(m_hWinSockThread != NULL) Sleep(1);
}

DWORD WINAPI CGGSV7::WinSockProc( CGGSV7* lpSrv )
{
	int			rc;
	CString		buf;
	sockaddr_in	from;
	int			fromlen = sizeof(from);
	BYTE		Buffer[1024];

	while(1)
	{
		rc = recvfrom(lpSrv->m_Socket, reinterpret_cast<char*>(Buffer), sizeof(Buffer), 0, (sockaddr *)&from, &fromlen);

		if(rc == SOCKET_ERROR) 
		{
			int	err	= WSAGetLastError();

			if(err == WSAECONNRESET)
			{				// клиент разорвал соединение
				continue;
			}
			else if(err == WSAEINTR)
			{				// прерывание ожидания
				closesocket(lpSrv->m_Socket);
				break;
			}
			else
			{
				break;
			}
		}
		if(rc > 0) 
		{
			CPackV7	Pack;

			if(Pack.ChanDecod(Buffer, rc, lpSrv->m_Key))
			{
				if(Pack.m_ClassPack == CPackV7::cpVoice)
				{
					if(!lpSrv->PutCnv(Pack))
					{
					}
				}
			}
		}
	}

	CloseHandle(lpSrv->m_hWinSockThread);
	lpSrv->m_hWinSockThread		= NULL;
	lpSrv->m_dwThreadWinSockID	= 0;

	return( TRUE ) ;
}

void	CGGSV7::MngPack(CPackV7& aPack)
{
	switch(aPack.m_ClassPack)
	{
	case CPackV7::cpVoice:
		{
			PutSock(aPack);
			break;
		}
	case CPackV7::cpData:
		{
			if(	aPack.m_TypPack == CPackV7::tpDirect &&
				aPack.m_Direct	== CPackV7::tdAnswer )
			{
				switch(aPack.m_TypAsk)
				{
				case CPackV7::taWho:
					{
						break;
					}
				}
			}
			break;
		}
	}
}

bool	CGGSV7::PutCnv(CPackV7& aPack)
{
	aPack.SetID(CIdentV7(0,0), CIdentV7(0,2));

	return PutPort(aPack);
}
