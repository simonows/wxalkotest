#include "stdafx.h"
#include "SendV7.h"
#include "ReceivV7.h"
#include "Xml.h"
#include "LoadRes.h"
#include "resource.h"

CSendV7::CSendV7() 
	: m_Wind(this)
	, m_SuppDNS(false)
	, m_SuppNTP(false)
	, m_SuppSubscript(false)
	, m_SuppVoiceByData(false)
	, m_SuppSDRIN(false)
{
	::InitializeCriticalSection(&m_Lock);
	m_hExist			= ::CreateEvent(NULL, TRUE, FALSE, NULL);

	m_LastChangedIPport	= 0;
	m_AddrRemote		= 0;
	m_PortRemote		= 0;
	m_hPutThread		= NULL;
	m_dwThreadPutID		= 0;
	
	m_Init				= false;
	m_CountIdent		= 0;
	
	m_MaxRetry			= 5;
	m_IntervalNext		= 2000;
	
	m_StaticOnly		= false;
	m_Disable			= false;
}

CSendV7::~CSendV7() 
{
	::EnterCriticalSection(&m_Lock);

	clear();

	::LeaveCriticalSection(&m_Lock);

	::DeleteCriticalSection(&m_Lock);
	::CloseHandle(m_hExist);
}

void	CSendV7::WaitEndThread(volatile HANDLE& hThread, const TCHAR* aMesErr)
{
	while(hThread)	Sleep(1);
}

bool CSendV7::SendMes(const CMes& aMes)
{
	return Put(CPackV7(aMes));
}

bool CSendV7::Put(CPackV7& aPack)
{
	bool rc = false;

	if(!m_Disable && m_hPutThread)
	{
		::EnterCriticalSection(&m_Lock);

		while(size() > 1000 && !front().IsOnOffGGS()) pop_front();

		if(aPack.IsOnOffGGS())	push_front(aPack);
		else					push_back(aPack);

		rc = true;

		SetEvent(m_hExist);								// установка признака непустоты очереди
		::LeaveCriticalSection(&m_Lock);
	}
	return(rc);
}

bool CSendV7::SendSocket(CPackV7& aPack)
{
	bool	rc = true;

	::EnterCriticalSection(&m_Lock);

	if(aPack.m_Orig == GetReceiv()->m_Ident)
	{		// отправляется пакет он нашего имени и в него добавляются подтверждения на присланные пакеты (в транзитные не добавляются)
		for(list<InpID>::iterator i = m_ReplyForSend.begin(); i != m_ReplyForSend.end(); ++i)
		{
			if(!(i->m_SendReply))
			{
				if(aPack.AddRepl(i->m_IdentInp))
				{
					i->m_SendReply	= true;
				}
				else
				{
					break;
				}
			}
		}
	}
	if(aPack.m_ClassPack == CPackV7::cpLink && aPack.m_Dest.GetDev() != 0)
	{	// в служебных пакетах в ID получателя всегда ставить тип устройства 0
		CIdentV7	Orig	= aPack.m_Orig;
		CIdentV7	Dest(aPack.m_Dest.GetNode(), 0);

		aPack.SetID(Orig, Dest);
	}

	if(	aPack.m_ClassPack	!= CPackV7::cpData	||	// игнорирование пакетов данных, 
		aPack.m_Ident		!= 0				||	// не имеющих идентификатора
		aPack.m_Reply		!= 0				||	// не содержащих подтверждение
	   !aPack.IsNoMes() )							// не содержащих данных
	{
		if(m_Init)
		{
			BYTE	Coding[CPackV7::mxLen];
			int lenOut = SOCKET_ERROR;

			int	Len = 	aPack.ChanCod(Coding, sizeof(Coding), m_Key.IsEmpty()? GetReceiv()->m_Key: m_Key);

//LogPut(m_Ident, m_AddrRemote, m_PortRemote, Coding, Len);	// !!! для отладки

			lenOut = GetReceiv()->Send(Coding, Len, m_AddrRemote, m_PortRemote);

			LogPut(m_Ident, m_AddrRemote, m_PortRemote, aPack, aPack.LenCRC());

			if(lenOut == SOCKET_ERROR) 
			{
				rc = false;
				//BadPutErr(_T("CSendV7::SendSocket: Send()"),WSAGetLastError());
			}

			if(m_Wind.IsNoLink())	IsNoLink();
		}
	}
	::LeaveCriticalSection(&m_Lock);

	return rc;
}

bool CSendV7::Send()
{
	bool		rc = false;
	CPackV7		PackSend;

	WaitForSingleObject(m_hExist, INFINITE);		// ожидание непустоты очереди
	::EnterCriticalSection(&m_Lock);

	if(!empty())
	{
		PackSend	= front();
		
		PackSend.SetID(GetReceiv()->m_Ident, m_Ident);

		pop_front();

		rc = true;
	}
	if(empty()) ::ResetEvent(m_hExist);				// установка признака пустоты очереди
	::LeaveCriticalSection(&m_Lock);

	if(rc)	m_Wind.Put(PackSend);

	return(rc);
}

void CSendV7::Start()
{
	if(!m_Disable)
	{
		m_Init				= true;
		m_SuppDNS			= false;
		m_SuppNTP			= false;
		m_SuppSubscript		= false;
		m_SuppSDRIN			= false;
		m_LastChangedIPport	= 0;

		try
		{
			clear();
			m_ReplyForSend.clear();

			m_hPutThread = CreateThread(NULL, 0, 
										(LPTHREAD_START_ROUTINE) PutProc,
										this, 0, &m_dwThreadPutID);
			if(!m_hPutThread)																	throw ErrorXML(_T("Error start PutProc"), GetLastError());

			m_Wind.Start();
		}
		catch(ErrorXML aErr)
		{
			if(aErr.m_Hr == 0 || aErr.m_Hr == 1)	BadPut(aErr.m_Message);
			else									BadPutErr(aErr.m_Message, aErr.m_Hr);

			m_Init	= false;
		}
	}
}

void CSendV7::Stop()
{
	BYTE	buf[4];
	CPackV7	PackCls(GetReceiv()->m_Ident, m_Ident);

	GetReceiv()->m_Ident.GetValue(buf);
	//PackCls.SetLink(CPackV7::tpClosLink, &(buf[1]), 3);
	PackCls.SetLink(CPackV7::tpClosLink, buf, 4);

	SendSocket(PackCls);

	m_Init	= false;

	m_Wind.Clear();

	Put(CPackV7());

	WaitEndThread(m_hPutThread,	_S_(IDS_V7_NOEND)+_T("CSendV7::PutProc"));

	m_Wind.Stop();

	clear();

	m_ReplyForSend.clear();
}

BYTE CSendV7::NextIdent()
{
	m_CountIdent++;
	m_CountIdent &= 0x7F;

	if(m_CountIdent == 0) m_CountIdent = 1;

	return(m_CountIdent);
}

void	CSendV7::ClearReplyForSend()
{
	::EnterCriticalSection(&m_Lock);

	m_ReplyForSend.clear();

	::LeaveCriticalSection(&m_Lock);
}

void	CSendV7::MngReply(int aReply)
{
	m_Wind.MngReply(aReply);
}

bool	CSendV7::MngIdentInp(BYTE aIdent)
{
	bool	rc = true;

	::EnterCriticalSection(&m_Lock);

	for(list<InpID>::iterator i = m_ReplyForSend.begin(); i != m_ReplyForSend.end();)
	{
		if( (clock() - i->m_TimeInp) > 60*1000 )
		{
			i = m_ReplyForSend.erase(i);
		}
		else
		{
			if(i->m_IdentInp == aIdent)
			{
				i->m_SendReply	= false;
				i->m_TimeInp	= clock();
				rc				= false;
			}
			++i;
		}
	}
	if(rc)
	{
		if(m_ReplyForSend.size() >= szSaveIdentInp)	m_ReplyForSend.pop_front();
		m_ReplyForSend.push_back(InpID(aIdent));
	}
	::LeaveCriticalSection(&m_Lock);
	return rc;
}

void	CSendV7::SendReply()
{
	::EnterCriticalSection(&m_Lock);

	for(list<InpID>::iterator i = m_ReplyForSend.begin(); i != m_ReplyForSend.end(); ++i)
	{
		if(!(i->m_SendReply))
		{
			SendSocket(CPackV7(GetReceiv()->m_Ident, m_Ident));
			break;
		}
	}
	::LeaveCriticalSection(&m_Lock);
}

DWORD WINAPI CSendV7::PutProc( LPVOID lpData )
{
	CSendV7*	pQueue = (CSendV7*)lpData;

	do
	{
		pQueue->Send();
	}
	while(pQueue->m_Init);

	CloseHandle(pQueue->m_hPutThread);
	pQueue->m_hPutThread	= NULL;
	pQueue->m_dwThreadPutID	= 0;

	return( TRUE ) ;
}

