#pragma once

#include "Slot.H"
#include "PackV7.H"
#include "WindV7.H"
#include "KeySys.H"

/////////////////////////////////////////////////////////////////////////////
// класс посылки данных пакетов V7

class CReceivV7;

class CSendV7 : public list<CPackV7>
{
	struct InpID
	{
		BYTE	m_IdentInp;				// Ident принятого пакета
		bool	m_SendReply;			// подтверждение на m_IdentInp послано
		clock_t	m_TimeInp;				// время получения пакета	

		InpID(BYTE aIdentInp)	
			: m_IdentInp(aIdentInp)
			, m_SendReply(false)
		{
			m_TimeInp = clock();
		};
	};

	HANDLE		m_hExist;					// ручное, несигнал - непустота очереди ожидающих пакетов
	int			m_CountIdent;				// счетчик для идентификатора посланного пакета
	HANDLE	volatile m_hPutThread;
	DWORD	m_dwThreadPutID;

	static	DWORD WINAPI PutProc( LPVOID lpData );
protected:
	CRITICAL_SECTION	m_Lock;				// блокировка доступа
	bool				m_Init;				// флаг инициации
	list<InpID>			m_ReplyForSend;		// идентификаторы принятых пакетов
public:
	enum sizeWind
	{
		szSaveIdentInp	= CWindV7::lenWind*2,
	};
	CSendV7();
	~CSendV7();

	virtual	void		WaitEndThread(volatile HANDLE& hThread, const TCHAR* aMesErr);
	virtual	bool		Retransmission(CReceivV7* pReceiv, CPackV7&	aPack, DWORD aAddr, int aPort)	{return false;};
	virtual void		LogPut(const CIdentV7& aID, DWORD aAddr, int aPort, const BYTE* aMes, int aLen)	{};
	virtual	void		IsNoLink()													= 0;
	virtual	void		BadPut(const TCHAR* aMes)									= 0;
	virtual	void		BadPutErr(const TCHAR* aMes = NULL, int aErr = -1)			= 0;
	virtual	void		BadPutMes(const TCHAR* aCnt, const BYTE* aPack, int aLen)	= 0;
	virtual CReceivV7*	GetReceiv()													= 0;
	virtual	void		MngReply(int aReply);
	virtual	bool		Put(CPackV7& aPack);	// записать пакет в очередь
	virtual bool		Send();					// инициация процесса вывода пакета по сетевому направлению

	void		Start();					// инициация потоков
	void		Stop();						// останов потоков
	bool		SendMes(const CMes& aMes);	// записать инкапсулированное сообщение в очередь
	bool		SendSocket(CPackV7& aPack);	// посылка пакет в сокет
	void		Break();					// посылка прерывающего пакета для процедуры PutProc
	BYTE		NextIdent();				// дать следующий Ident для отправляемого пакета
	bool		MngIdentInp(BYTE aIdent);	// обработка идентификатора принятого пакета
	void		SendReply();				// формирование пустого пакета если есть необходимость послать Reply, а исходящая очередь пуста
	void		ClearReplyForSend();

	time_t		m_LastChangedIPport;		// врмя последнего изменением IP:Port ЛБ
	DWORD		m_AddrRemote;				// IP адрес клиента
	int			m_PortRemote;				// UDP порт клиента
	CIdentV7	m_Ident;					// идентификатор клиента
	CKeySys		m_Key;						// ключ системы конкретного ЛБ
	CWindV7		m_Wind;						// окно отсылки
	int			m_MaxRetry;					// максимальное число повторов
	int			m_IntervalNext;				// интервал следующей посылки (мс)
	bool		m_StaticOnly;				// только статический IP адрес
	bool		m_Disable;					// связь запрещена
	bool		m_SuppDNS;					// поддержка DNS
	bool		m_SuppNTP;					// поддержка NTS (службы времени)
	bool		m_SuppSubscript;			// поддержка подписки
	bool		m_SuppVoiceByData;			// поддержка передачи голоса по порту данных
	bool		m_SuppSDRIN;				// поддержка сервиса SD РИН
};

