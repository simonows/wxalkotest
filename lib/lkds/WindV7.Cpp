#include "stdafx.h"
#include "WindV7.H"
#include "SendV7.H"
#include "ReceivV7.h"
#include "Xml.h"
#include "LoadRes.h"
#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// класс пакетов в окне на отсылку

CPackWindV7::CPackWindV7(const CPackV7& aPack, CWindV7*	pWind)
	: m_Pack(aPack)
	, m_CountRetry(0)
	, m_pWind(pWind)
	, m_TimerNextSend(0)
{
}

/////////////////////////////////////////////////////////////////////////////
// класс окна на отсылку

CWindV7::CWindV7(CSendV7*	pSend)
	: m_pSend(pSend)
	, m_IsStart(false)
	, m_hTimerThread(0)
	, m_dwThreadTimerID(0)
	, m_CountNoReply(0)
	, m_CountNoReply_(0)
	, m_ExistInput(false)
{
	//m_hLock		= ::CreateEvent(NULL, FALSE, TRUE, NULL);	// синхронизационное событие доступа
	::InitializeCriticalSection(&m_Lock);
	m_hFree		= ::CreateEvent(NULL, TRUE,  TRUE, NULL);	// наличе свободных ячеек в окне
	m_hTimer	= ::CreateEvent(NULL, TRUE,  FALSE, NULL);	// организация таймера
}

CWindV7::~CWindV7()
{
	Clear();

	CloseHandle(m_hTimer);
	CloseHandle(m_hFree);
	//CloseHandle(m_hLock);
	::DeleteCriticalSection(&m_Lock);
}

void	CWindV7::Clear()
{
	//WaitForSingleObject(m_hLock, INFINITE);
	::EnterCriticalSection(&m_Lock);

	for(iterator i = begin(); i != end(); ++i)
	{
		delete *i;
	}
	clear();

	SetEvent(m_hFree);								// установка признака присутсвия свободных мест в окне
	//SetEvent(m_hLock);								// освобождение окна для доступа
	::LeaveCriticalSection(&m_Lock);
}

bool	CWindV7::Put(CPackV7& aPack)
{
	bool	rc = true; 

	WaitForSingleObject(m_hFree, INFINITE);					// ожидание наличия в окне свободного места
	//m_pSend->WaitWithTimeOut(m_hLock, _T("CWindV7::Put"));	// ожидание освобождения окна для доступа
	::EnterCriticalSection(&m_Lock);

	if(aPack.IsInIdent())
	{	// пакет должен отправляться с Ident
		if(aPack.m_Orig == m_pSend->GetReceiv()->m_Ident)
		{		// отправляется пакет он нашего имени
			aPack.SetIdent(m_pSend->NextIdent());

			rc = m_pSend->SendSocket(aPack);					// посылка пакета по IP сети

			if(rc)
			{
				CPackWindV7*	pPack = new CPackWindV7(aPack, this);

				pPack->m_CountRetry		= 1;						// надо отправлять с Ident
				pPack->m_TimerNextSend	= m_pSend->m_IntervalNext;	// интервал следующей посылки в (мс)

				push_back(pPack);
			}
			else
			{
				if(aPack.m_bIsSubscrib)	m_PackSubscribe = aPack;

				m_CountNoReply++;
			}
		}
		else
		{		// отправляется транзитный пакет
			rc = m_pSend->SendSocket(aPack);				// посылка пакета по IP сети

			if(!rc)	m_CountNoReply++;
		}
	}
	else
	{			// отправляется пакет без Ident
		aPack.DelIdent();

		rc = m_pSend->SendSocket(aPack);				// посылка пакета по IP сети

		if(!rc)	m_CountNoReply++;
	}
	if(size() == lenWind)	ResetEvent(m_hFree);		// установка признака отсутсвия свободных мест в окне
	//SetEvent(m_hLock);									// освобождение окна для доступа
	::LeaveCriticalSection(&m_Lock);

	return rc;
}

void	CWindV7::MngReply(BYTE aReply)
{
	m_CountNoReply	= 0;		// 22.03.2010 как только что-то приходит от ЛБ6Pro
	m_ExistInput	= true;


	if(m_PackSubscribe.m_bIsSubscrib)
	{
		m_pSend->Put(m_PackSubscribe);	// посылка не переданной подписки
		m_PackSubscribe.Clear();
	}

	if(aReply > 0)
	{
		//m_pSend->WaitWithTimeOut(m_hLock, _T("CWindV7::MngReply"));	// ожидание освобождения окна для доступа
		::EnterCriticalSection(&m_Lock);

		for(iterator i = begin(); i != end();)
		{
			CPackWindV7* pPack = *i;

			if(pPack->m_Pack.m_Ident == aReply)
			{
				for(iterator k = begin(); k != i; ++k)
				{
					(*k)->m_TimerNextSend = 0;			// все пакеты, расположенные в окне до подтвержденного, отправить как можно быстрее
				}

				i = erase(i);
				delete pPack;
				continue;
			}
			 ++i;
		}

		if(size() < lenWind)	SetEvent(m_hFree);			// установка признака присутсвия свободных мест в окне

		//SetEvent(m_hLock);									// освобождение окна для доступа
		::LeaveCriticalSection(&m_Lock);
	}
}


void	CWindV7::Start()
{
	if(!m_IsStart)
	{
		clear();

		m_IsStart		= true;
		m_CountNoReply	= 0;
		m_CountNoReply_	= 0;
		m_ExistInput	= false;

		SetEvent(m_hTimer);
		ResetEvent(m_hTimer);

		try
		{
			m_hTimerThread = CreateThread(NULL, 0, 
										(LPTHREAD_START_ROUTINE) TimerProc,
										this, 0, &m_dwThreadTimerID);
			if(!m_hTimerThread)																	throw ErrorXML(_T("Error start CWindV7::TimerProc"), GetLastError());
		}
		catch(ErrorXML aErr)
		{
			if(aErr.m_Hr == 0 || aErr.m_Hr == 1)	m_pSend->BadPut(aErr.m_Message);
			else									m_pSend->BadPutErr(aErr.m_Message, aErr.m_Hr);

			m_IsStart	= false;
		}
	}
}

void	CWindV7::Stop()
{
	if(m_IsStart)
	{
		m_IsStart	= false;

		m_pSend->WaitEndThread(m_hTimerThread,_S_(IDS_V7_NOEND)+_T("CWindV7::TimerProc"));
	}
}

DWORD	WINAPI		CWindV7::TimerProc(LPVOID lpData)
{
	CWindV7*	pWind	= static_cast<CWindV7*>(lpData);

	while(pWind->m_IsStart)
	{
		DWORD rc = WaitForSingleObject(pWind->m_hTimer, interval);
		
		if(!pWind->m_IsStart) break;

		//pWind->m_pSend->WaitWithTimeOut(pWind->m_hLock, _T("CWindV7::TimerProc"));	// ожидание освобождения окна для доступа
		::EnterCriticalSection(&(pWind->m_Lock));

		for(CWindV7::iterator i = pWind->begin(); i != pWind->end();)
		{
			CPackWindV7* pPack = *i;

			if(pPack->m_CountRetry < pWind->m_pSend->m_MaxRetry)
			{
				pPack->m_TimerNextSend	-= interval;

				if(pPack->m_TimerNextSend <= 0)
				{
					bool rc = pWind->m_pSend->SendSocket(pPack->m_Pack);

					pPack->m_CountRetry++;
					pPack->m_TimerNextSend	= pWind->m_pSend->m_IntervalNext;

					if(!rc) pWind->m_CountNoReply++;
				}
			}
			else
			{
				pWind->m_CountNoReply++;
				pWind->m_CountNoReply_++;

				if(pWind->m_CountNoReply_ > 10)
				{
					if(pWind->m_ExistInput)
					{
						pWind->m_pSend->BadPut("Remote unit "+pWind->m_pSend->m_Ident.GetString()+" not receiving data from it unit");
					}

					pWind->m_CountNoReply_	= 0;
					pWind->m_ExistInput		= false;
				}

				if(pPack->m_Pack.m_bIsSubscrib)	pWind->m_PackSubscribe	= pPack->m_Pack;
				i = pWind->erase(i);
				delete pPack;
				continue;
			}
			 ++i;
		}

		if(pWind->size() < lenWind)	SetEvent(pWind->m_hFree);	// установка признака присутсвия свободных мест в окне

		//SetEvent(pWind->m_hLock);								// освобождение окна для доступа
		::LeaveCriticalSection(&(pWind->m_Lock));
	}
	CloseHandle(pWind->m_hTimerThread);
	pWind->m_hTimerThread		= 0;
	pWind->m_dwThreadTimerID	= 0;

	return( TRUE ) ;
}

bool	CWindV7::IsNoLink()		
{ 
	if(m_CountNoReply > 1)
	{
		m_CountNoReply = 0;
		return true;
	}
	else
	{
		return false;
	}
};
