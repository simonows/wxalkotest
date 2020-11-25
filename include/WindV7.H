#pragma once

#include "PackV7.H"

/////////////////////////////////////////////////////////////////////////////
// класс пакетов в окне на отсылку

class CWindV7;

class CPackWindV7
{
public:
	CPackV7		m_Pack;				// пакет на отсылку
	bool		m_IsSendet;			// на пакет пришло подтверждение
	int			m_CountRetry;		// количестово повторных посылок
	int			m_TimerNextSend;	// интервал до следующей посылки пакета
	CWindV7*	m_pWind;			// указатель на окно

	CPackWindV7(const CPackV7& aPack, CWindV7* pWind);
};

/////////////////////////////////////////////////////////////////////////////
// класс окна на отсылку

class CSendV7;

class CWindV7 : public list<CPackWindV7*>
{
	//HANDLE	m_hLock;			// синхронизационное событие доступа
	CRITICAL_SECTION	m_Lock;		// блокировка доступа
	HANDLE	m_hFree;			// наличе свободных ячеек в окне
	HANDLE	m_hTimer;			// организация таймера
	bool	m_IsStart;			// запущен поток таймера
	HANDLE	volatile m_hTimerThread;
	DWORD	m_dwThreadTimerID;
	int		m_CountNoReply;		// число пакетов, подряд не получивших подтверждение
	int		m_CountNoReply_;	// число пакетов, подряд не получивших подтверждение
	bool	m_ExistInput;		// есть ли принятые пакеты
	CPackV7	m_PackSubscribe;	// сохраненный пакет подписки
public:
	enum
	{
		lenWind		= 5,	// размер окна
		//maxRetry	= 5,	// максимальное число повторов
		interval	= 100,	// интервал таймера (мс)
		//nextSend	= 2000,	// интервал следующей посылки (мс)
	};
	CWindV7(CSendV7*	pSend);
	~CWindV7();

	CSendV7*	m_pSend;

	void	Clear();
	void	Start();
	void	Stop();
	void	MngReply(BYTE aReply);	// обработка подтверждения
	bool	Put(CPackV7& aPack);
	bool	IsNoLink();

	static	DWORD	WINAPI	TimerProc(LPVOID lpData);	// процедура повторной посылки пакетов
};
