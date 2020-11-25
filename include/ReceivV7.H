#pragma once

#include "Slot.H"
#include "PackV7.H"
#include "KeySys.H"

/////////////////////////////////////////////////////////////////////////////
// класс приема данный пакетов V7

class CSendV7;

class CReceivV7
{
	CRITICAL_SECTION							m_Lock;						// блокировка доступа
	bool										m_bOpen;					// признак запуска потока

	int		Bind(CString& aMesErr);			// уcтановка прослушивающего порта
public:
	CReceivV7();
	~CReceivV7();

	CIpAddr					m_AddrOwn;		// собственный (прослушиваемый) IP адрес
	CIpPort					m_PortOwn;		// собственный (прослушиваемый) IP порт
	CIdentV7				m_Ident;		// собственный идентификатор
	CKeySys					m_Key;			// ключ системы
	CIpAddr					m_GTW;			// шлюз при работе с протоколом прямого доступа
	time_t					m_LastLinkFromCloud;

	void		Start();				// инициация потоков
	void		Stop();					// останов потоков
	bool		IsStart()	{ return m_bOpen; };
	int			Send(const BYTE* aBody, int aLen, DWORD aIPAddr, int aIPPort);

	HANDLE		volatile m_hWinSockThread;
	DWORD		m_dwThreadWinSockID;
    SOCKET		m_Socket;					// прослушиваемый порт

	virtual	void		WaitEndThread(volatile HANDLE& hThread, const TCHAR* aMesErr);
	virtual	CSendV7*	NewSender(const CIdentV7& aOrig, DWORD aAddr, int aPort, const CKeySys& aKey, const CReceivV7* pReceiv)		{ return 0;};
	virtual void		LogPut(const CSendV7* pSender, DWORD aAddr, int aPort, const BYTE* aMes, int aLen)	{};
	virtual	void		BadPut(const TCHAR* aMes, bool isNeedInform = false)		= 0;
	virtual	void		BadPutErr(const TCHAR* aMes = NULL, int aErr = -1)			= 0;
	virtual	void		BadPutMes(const TCHAR* aCnt, const BYTE* aPack, int aLen, bool isNeedInform = false)	= 0;
	virtual	void		HandlingData(const CPackV7& aPack, const CSendV7* pSender)	= 0;
	virtual	CSendV7*	GetSend(const CIdentV7& aID)								= 0;
	virtual void		IsChangIP(const CSendV7* pSender)							{};
	virtual	bool		ConnReset(const TCHAR* aIpPort, int aErr);
	virtual	bool		ChanDecod(const BYTE* aBuf, int aLen, CPackV7& aPack, CKeySys& aKey, bool& isFromCloud)		{ return false;};
	virtual CAtlString	NameApp()													{ return _T("CReceivV7");};

	static		DWORD	WINAPI	WinSockProc( LPVOID lpData );
	static const int s_IdentCloud;	// идентификатор LKDSCloud
};

