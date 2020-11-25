#pragma once

#include <TIME.H>

#include <Dev.H>
#include <Mes.H>
#include <SostCLB.h>
#include <Pack.h>
#include <NvRam.H>
#include <Param.h>

/////////////////////////////////////////////////////////////////////////////
// класс логической исходящей связи КСЛ
//

class CSlot;
class CLinkNetTab;

class CLinkNet
{
public:
	int				m_NumUMi;		// номер связанного УМ
	int				m_NumSLOTi;		// номер связанного слота в УМ
	CSlot*			m_pLink;		// указатель на связанный КРК
	CLinkNetTab*	m_pTabLinkNet;	// таблица логических связей, к которой относится данная
	CParam*			m_pParam;		// дополнительные параметры

	CComPtr<IXMLDOMNode>	m_XMLNode;
	bool					m_FlagModify;

	CLinkNet(CLinkNetTab* aTabLinkNet, CParam* aParam);
	CLinkNet( CLinkNet &aLink);
	~CLinkNet();

	void	Clear();

	void	XMLRemove();
	void	XMLUpdate();
	void	XMLAdd();
	void	XMLLoad(CComPtr<IXMLDOMNode> spNode);
	void	XMLPostLoad();
	void	XMLRelease();

	static	const	TCHAR*	s_LinkNet;
	static	const	TCHAR*	s_UM;
	static	const	TCHAR*	s_Slot;
};

/////////////////////////////////////////////////////////////////////////////
// класс таблицы логических исходящих связей КСЛ

class CLinkNetTab : public list<CLinkNet*>
{
public:
	CLinkNetTab(CSlot* aSlot) 
		: m_pSlot(aSlot)
		, m_FlagModify(false)	{};
	~CLinkNetTab()	{ClearAll();};

	void ClearAll()
	{
		for(list<CLinkNet*>::iterator i = begin(); i != end(); ++i)
		{
			delete *i;
		}
		clear();
	}
	void		Clear() { ClearAll(); };
	bool		Insert(int aNumUMi, int aNumSLOTi, CSlot*	aLink, CParam* aParam);
	CLinkNet*	FindLink(int aNumUMi, int aNumSLOTi);
	CLinkNet*	FindLink(CSlot*	aLink);
	void		ReOrder();					// сортировка по номеру слота на ММИ

	CSlot*					m_pSlot;
	CComPtr<IXMLDOMNode>	m_XMLNode;
	bool					m_FlagModify;

	void	XMLRemove();
	void	XMLUpdate();
	void	XMLAdd();
	void	XMLLoad(CComPtr<IXMLDOMNode> spNode);
	void	XMLRelease();

	static	const	TCHAR*	s_TabLinkNet;
};

/////////////////////////////////////////////////////////////////////////////
// класс IP адреса

class CIpAddr
{
public:
	BYTE	f0;	// поле 0
	BYTE	f1;	// поле 1
	BYTE	f2;	// поле 2
	BYTE	f3;	// поле 3

	CIpAddr() {Clear();};

	void Clear()	{f0 = 0; f1 = 0; f2 = 0; f3 = 0;};

	CAtlString	GetString();
	bool		SetString(const TCHAR* aStr);
	DWORD		GetValue();
	bool		SetValue(DWORD aValue);
	DWORD		GetValueW();
	bool		SetValueW(DWORD aValue);

	bool		IsValid();
	bool		IsNul();
	static bool	TestModifyIP(DWORD IP, int iField, int iValue);

	void	operator  = (const CIpAddr& a) {f0 = a.f0; f1 = a.f1; f2 = a.f2; f3 = a.f3;};
	bool	operator == (CIpAddr& i) { return(f0==i.f0&&f1==i.f1&&f2==i.f2&&f3==i.f3);};
};

/////////////////////////////////////////////////////////////////////////////
// класс MAC адреса

class CMacAddr
{
public:
	BYTE	f0;	// поле 0
	BYTE	f1;	// поле 1
	BYTE	f2;	// поле 2
	BYTE	f3;	// поле 3
	BYTE	f4;	// поле 4
	BYTE	f5;	// поле 5

	CMacAddr() {Clear();};

	void Clear()	{f0 = 0; f1 = 0; f2 = 0; f3 = 0; f4 = 0; f5 = 0;};

	CAtlString	GetString();
	bool		SetString(const TCHAR* aStr);

	void		operator  = (const CMacAddr& a) {f0 = a.f0; f1 = a.f1; f2 = a.f2; f3 = a.f3; f4 = a.f4; f5 = a.f5;};
};

/////////////////////////////////////////////////////////////////////////////
// класс IP порта

class CIpPort
{
public:
	BYTE	l;	// младший байт
	BYTE	h;	// старший байт

	CIpPort() {Clear();};
	CIpPort(int aValue) {SetValue(aValue);};

	void	Clear()	{l = 0; h = 0;};

	void	operator  = (const CIpPort& a) {l = a.l; h = a.h;};

	unsigned int GetValue();
	bool	SetValue(unsigned int aValue);

	bool	operator == (CIpPort& p);
};

class CLinkCLLTab;

/////////////////////////////////////////////////////////////////////////////
// класс исходящей связи КСЛ

class CLinkCLL
{
public:
	enum
	{
		lenFone	= 21,
	};
	int				m_Number;			// номер
	int				m_TimeLife;			// время жизни маршрута
	int				m_NumUM;			// номер связанного УМ
	int				m_NumSLOT;			// номер связанного слота в УМ
	int				m_TypModem;			// тип модема
	int				m_Rez1;				// резерв 1
	int				m_TimeLink;			// время удержания соединения
	int				m_Rez2;				// резерв 2
	CString			m_Fone;				// строка набора номера
	CIpAddr			m_IP;				// IP адрес
	CIpAddr			m_IPDop[2];			// дополнительные IP адреса
	CIpAddr			m_Mask;				// маска подсети
	CIpAddr			m_GTW;				// IP адрес шлюза
	CMacAddr		m_MAC;				// MAC адрес
	CIpPort			m_DataPort;			// порт для данных
	CIpPort			m_VoicePort;		// порт для звука
	CString			m_Note;			// примечание
	bool			m_IsKnov;			// признак известности связи
	bool			m_IsModify;			// признак модификации связи
	bool			m_IsLink;			// признак наличия связи
	CLinkCLLTab*	m_pTabLinkCLL;		// ссылка на талицу, которой принадлежит связь	
	CParam*			m_pParam;			// дополнитльеные параметры


	CComPtr<IXMLDOMNode>	m_XMLNode;
	bool					m_FlagModify;

	CLinkCLL(int aNumber, CLinkCLLTab* aTabLinkCLL);
	~CLinkCLL();

	void	XMLRemove();
	void	XMLUpdate();
	void	XMLAdd();
	void	XMLLoad(CComPtr<IXMLDOMNode> spNode);
	void	XMLPostLoad();
	bool	operator  < (const CLinkCLL* aLink) { return m_Number < aLink->m_Number; };

	static	const	TCHAR*	s_LinkCLL;
	static	const	TCHAR*	s_Number;
	static	const	TCHAR*	s_UM;
	static	const	TCHAR*	s_Slot;
	static	const	TCHAR*	s_TimeLife;
	static	const	TCHAR*	s_TypModem;
	static	const	TCHAR*	s_TimeLink;
	static	const	TCHAR*	s_Fone;
	static	const	TCHAR*	s_IP;
	static	const	TCHAR*	s_IPDop1;
	static	const	TCHAR*	s_IPDop2;
	static	const	TCHAR*	s_Mask;
	static	const	TCHAR*	s_GTW;
	static	const	TCHAR*	s_MAC;
	static	const	TCHAR*	s_DataPort;
	static	const	TCHAR*	s_VoicePort;
	static	const	TCHAR*  s_Note;
};

/////////////////////////////////////////////////////////////////////////////
// сравниватель <

struct LTpLinkCLL : public binary_function<CLinkCLL*, CLinkCLL*, bool>
{ 
	bool operator()(const CLinkCLL* _Left, const CLinkCLL* _Right) const { return (_Left->m_Number < _Right->m_Number); }
};

/////////////////////////////////////////////////////////////////////////////
// класс таблицы исходящих связей КСЛ

class CLinkCLLTab : public list<CLinkCLL*>
{
public:
	CLinkCLLTab(CSlot*	aSlot);
	~CLinkCLLTab()
	{
		Clear();
	}

	void	Clear() 
	{
		for(list<CLinkCLL*>::iterator i = begin(); i != end(); ++i)
		{
			delete *i;
		}
		clear();
		m_VolRec	= 0;
		m_VolPlay	= 0;
	};
	CLinkCLL*	Find(int aNumber);
	CLinkCLL*	Find(int aUM, int aSlot);
	CLinkCLL*	FindUM(int aUM);
	CLinkCLL*	FindIP(DWORD aIP);
	CLinkCLL*	GetItem(int aNumber);
	bool		Delete(int aNumber);
	void		ReOrder()		{ sort(LTpLinkCLL()); };
	void		ReNumber();

	int			m_VolRec;		// уровень чуствительности входа 
	int			m_VolPlay;		// уровень громкости выхода

	CSlot*					m_pSlot;
	CComPtr<IXMLDOMNode>	m_XMLNode;
	bool					m_FlagModify;

	void	XMLRemove();
	void	XMLUpdate();
	void	XMLAdd();
	void	XMLLoad(CComPtr<IXMLDOMNode> spNode);
	void	XMLPostLoad();
	void	XMLRelease();

	static	const	TCHAR*	s_TabLinkCLL;
	static	const	TCHAR*	s_VolRec;
	static	const	TCHAR*	s_VolPlay;
};

/////////////////////////////////////////////////////////////////////////////
// класс КЛШ
//

class CUM;

class CSlot
{
public:
	enum typSlot				// тип устройства в слоте
	{
		tsNone	= 0,			// не указан
		tsCLB	= 1,			// контроллер локальной шины
		tsCLLo	= 2,			// контроллер соединительной линии исходящий
		tsCLLi	= 3,			// контроллер соединительной линии входящий
		tsMon	= 4,			// диспетчеркий пульт
		tsCRCo	= 5,			// контроллер радиоканала исходящий
		tsCRCi	= 6,			// контроллер радиоканала входящий
		tsNETo	= 7,			// связь по сети исходящая
		tsNETi	= 8,			// связь по сети входяя
		tsGSMo	= 9,			// связь по GSM исходящая
		tsGSMi	= 10,			// связь по GSM входщая
		tsGGS	= 11,			// ГГС ретраслятор для сетевого подключения
		tsCLLEo	= 12,			// связь по КСЛ+ (Ethernet) исходящая
		tsCLLEi	= 13,			// связь по КСЛ+ (Ethernet) входящая
		tsCLLMo	= 14,			// связь по КСЛ+ (PSTN, GSM) исходящая
		tsCLLMi	= 15,			// связь по КСЛ+ (PSTN, GSM) входящая
		tsCLLRo	= 16,			// связь по КСЛ+ (433MHz/2,4ГГц) исходящая
		tsCLLRi	= 17,			// связь по КСЛ+ (433MHz/2,4ГГц) входящая
		tsBRDGo	= 18,			// связь по концентратору ММИ исходящая
		tsBRDGi	= 19,			// связь по концентратору ММИ входящая
		tsCLLGo	= 20,			// связь по КСЛ+ (2,4ГГц) исходящая (только для MScan)
		tsCLLGi	= 21,			// связь по КСЛ+ (2,4ГГц) входящая (только для MScan)
		tsDPL	= 1001,			// ЛДП
		tsDPG	= 1002,			// ЦДП
	};
	enum typDev					// код типа устройства
	{
		tdNONE		= -1,		// не определено
		tdCLB		= 0,		// КЛШ
		tdCDC		= 1,		// КЛШ-КДК
		tdCUS		= 2,		// КУС
		tdCLB_		= 3,		// КЛШ+
		tdCLBP		= 4,		// КЛШ-Pro
		tdCLB5m		= 5,		// КЛШ-Lite v5,7 моноблок
		tdCLB7m		= 6,		// КЛШ-Lite v7 моноблок
		tdCLBCM3	= 7,		// КЛШ-Pro CM3
		tdCLBCM3m	= 8,		// КЛШ-Lite CM3 моноблок
		tdCLL		= 32,		// КСЛ
		tdCRC		= 33,		// КРК
		tdGSM		= 34,		// GSM
		tdNET		= 35,		// сетевой соединитель
		tdGGS		= 36,		// КРЕС
		tdCLLM		= 37,		// КСЛ+ (PSTN, GSM) соединитель по модему (обычному, выделенная линия и GSM)
		tdCLLR		= 38,		// КСЛ+ (433MHz) соединитель по радиомодему
		tdCLLE		= 39,		// КСЛ+ (Ethernet) соединитель по сети
		tdBRDG		= 40,		// концентратор ММИ
		tdCLLG		= 41,		// КСЛ+ (2,4ГГц) соединитель по радиомодему
		td723		= 43,		// кодек 723.1
		tdCLLEm		= 50,		// КСЛ-Ethernet-Lite моноблок
		tdCLLRm		= 51,		// КСЛ-433MHz-Lite моноблок
		tdCLLGm		= 52,		// КСЛ-2,4ГГz-Lite моноблок
		tdCLLMm		= 53,		// КСЛ-PSTN,GSM-Lite моноблок
		tdCLLMCM3	= 54,		// КСЛ CM3(PSTN,GSM)
		tdCLLRCM3	= 55,		// КСЛ CM3(433MHz)
		tdCLLECM3	= 56,		// КСЛ CM3(Ethernet)
		tdCLLMCM3m	= 57,		// КСЛ CM3(PSTN,GSM)-Lite моноблок
		tdCLLRCM3m	= 58,		// КСЛ CM3(433MHz)-Lite моноблок
		tdCLLECM3m	= 59,		// КСЛ CM3(Ethernet)-Lite моноблок
		tdDPL		= 1001,		// ЛДП
		tdDPG		= 1002,		// ЦДП
	};
	enum typBus
	{
		tbNONE	= 0,			// не определен
		tbARM	= 10,			// АРМ
		tbOB4x	= 11,			// ОБЬ-4.Х
		tbOB31	= 12,			// ОБЬ-3.1
		tbOB30	= 13,			// ОБЬ-3.0
		tbCDC	= 14,			// КДК-32
		tbOB5X	= 15,			// ОБЬ-5.Х
	};
	enum flagConf
	{
		flLB	= 0x1,			// лифтовые блоки
		flTerm	= 0x2,			// инженерные терминалы
	};
	enum typDP
	{
		tdLDP	= 8,			// номер слота локального диспечерского пункта
		tdGDP	= 9,			// номер слота центрального диспетчерского пункта
	};
	enum addr
	{
		addrKeyNet	= 684,		// адрес ключа сети
		lenKeyNet	= 4,		// длина ключа сети в байтах
	};
	int				m_NumSLOT;		// номер слота в УМ
	COleDateTime	m_LastMes;		// дата прихода последнего состояния
	COleDateTime	m_TimSost;		// дата установки состояния
	CSostCLB		m_SostCLB;		// текущее состояние 
	COleDateTime	m_TimRestart;	// дата последнего рестарта
	int				m_CodRestart;	// код последнего рестарта
	typSlot			m_TypSlot;		// тип слота
	typDev			m_TypDev;		// код типа устройства
	bool			m_IsProgram;	// устройство - программа
	int				m_VersProg[2];	// версия прикладной микропрограммы
	int				m_MsgApi[2];	// версия базовой микропрограммы
	typBus			m_TypBus;		// тип локальной шины
	int				m_MaxAdr;		// максимальное значение адреса
	CSlot*			m_pLink;		// указатель на связанный КСЛ (для других NULL)
	CLinkNetTab		m_TabLinkNet;	// таблица логических исходящих связей КСЛ
	CLinkCLLTab		m_TabLinkCLL;	// таблица исходящих связей КСЛ
	CUM*			m_pUM;			// указатель на узловой модуль, к которому подключен блок
	CPackMes		m_Pack;			// пакет сообщений к данному блоку
	CNvRam			m_NvRam;		// содержимое NVRAM
	COleDateTime	m_TimKey;		// время вставка нового ключа
	BYTE			m_LvlKey;		// уровень ключа
	BYTE			m_PrefKey;		// префикс ключа
	WORD			m_NumKey;		// номер ключа
	int				m_Ident;		// текущий идентификатор
	CDevTab			m_TabDev;		// описание подключенных устройств
	CParam*			m_pParam;		// дополнитльеные параметры
	int				m_TimeAns;		// среднее время отвеиа
	bool			m_SuppIAP;		// Поддержка сервиса IAP
	bool			m_SuppREQ;		// Поддержка сервиса REQ
	bool			m_SuppLLBAT;	// Поддержка сервиса LLBAT
	bool			m_KnownSost;	// известно текущее состояние 
	long			m_LastGetConf;	// время последней посылки запроса на чтение состояния Clock()
	COleDateTime	m_TimGetConf;	// время последней посылки запроса на чтение состояния сек
	long			m_CountConf;	// количесво прихода сообщений состояния
	long			m_IntervConf;	// общее время в милисекундах от посылм запроса на состояние до ответа
									//  т.е. среднее время m_IntervSost/m_CountParam
	COleDateTime	m_TimLast;		// дата прихода последнего сообщения
	bool			m_ExistInp;		// наличие сообщений слота

	int				m_NumUMi;		// номер связанного УМ
	int				m_NumSLOTi;		// номер связанного слота в УМ
	bool			m_NoJob;		// признак неработоспособности
	bool			m_OnGGS;		// признак включения ГГС
	bool			m_NoLink;		// признак отсутсвия пути
	bool			m_OnKey;		// свтавлен СК
	bool			m_LowVers;		// устаревшая версия ПО
	bool			m_WaitKey;		// ожидание вычитывания номера ключа

	CComPtr<IXMLDOMNode>	m_XMLNode;
	bool					m_FlagModify;
	
	CSlot(int aNumSLOT) 
		: m_pParam(NULL)
		, m_TabLinkNet(this)
		, m_TabLinkCLL(this)
		, m_TabDev(this) 
	{Clear(); m_NumSLOT = aNumSLOT;};
	CSlot(int aNumSLOT, CUM* apUM, typSlot aTypSlot, CParam* aParam);
	CSlot()	
		: m_pParam(NULL)
		, m_TabLinkNet(this)
		, m_TabLinkCLL(this)
		, m_TabDev(this) 
	{ Clear();};
	CSlot( CSlot &aSlot);
	~CSlot();

	void	Clear();
	int		Vers()								{return(m_VersProg[0]*100+m_VersProg[1]);};

	void	Put();
	void	PutNoIdent();
	void	AddIdent(int aIdent)				{m_Pack.AddIdent(aIdent);};
	void	SetIdent(int aIdent);				// установить идентификатор пакета							
	void	SetPrior(int aPrior);				// установить приоритет пакета
	void	AddMes(int aPrior, CMes::typMes aTypMes, const BYTE aMask);
	void	AddMes(int aPrior, CMes::typMes aTypMes, CMes::askFromMain aAsk, int dev, const BYTE* aMask = NULL, int aLen = 0, int aDuration = -1);
	void	AddMes(int aPrior, CMes::typService aService, int aDuration, int dev, CMes::askFromMain aAsk, const BYTE* aData = NULL, int aLen = 0);
	void	AddMes(int aPrior, CMes::typMes aTypMes, const BYTE* aMask, int aLen);
	void	AddMes(int aPrior, CMes::typService aTypSrv, const BYTE* aBody, int aLen);
	void	AddMes(int aPrior, const CMes& aMes);
	void	AddMesNoIdent(int aPrior, const CMes& aMes);
	void	AddMes(int aPrior, CMes::typPrefix aTypPrefix, BYTE aAddr);
	void	AddMes(int aPrior, CMes::typPrefix aTypPrefix, BYTE aAddr, CMes::askFromMain aAsk, int aLen = 0, const BYTE* aBody = NULL);
	void	AddMesSetNVRAM(int aPrior, int aAddr, int aValue);
	void	AddMes(int aPrior, int aDriv, CMes::subServ18 aSubServ18, const BYTE* aBody = 0, int aLen = 0);
	bool	Ggs(CMes::askFromMain aAsk, int dev, DWORD aIP, DWORD aPort, int aNumWavIn = -1,  int aNumWavOut = -1);
	void	SetAdmPult();					// назначить адрес диспетчерского пульта
	void	SetAddr();						// запись адреса диспетчерской и собственного
	void	GetNvRamAll();					// прочитать содержимое NVRAM
	bool	IsLocal();						// слот локального УМ
	bool	IsEther();						// исходящий эфирный слот
	bool	IsLine();						// исходящий линейный слот
	bool	IsCLB();						// устройство КЛШ
	bool	IsInpLink();					// входящее узловое устройтво
	bool	IsDevice();						// признак физического устройства
	bool	IsNewGGS();						// признак включение ГГС для новых устройств
	void	ReLinkEth(typSlot aTypInp, typSlot aTypOut);	// изменение направления связей
	void	ReLinkOne(typSlot aTypInp, typSlot aTypOut);	// изменение направления связей
	void	GetSostL();						// читать состояние локального КЛШ
	void	GetSostR();						// читать состояние удаленного КЛШ
	void	SetNoInp();						// установить признак "не было данных от слота"
	void	AskConf();						// запросить конфигурацию слота
	void	TestOffL();						// проверка потери связи с локальным КЛШ
	void	TestOffR();						// проверка потери связи с удаленным КЛШ
	void	SetTimeConf();					// установка времени запроса конфигурации
	void	ClrStat();						// обнуление статистики
	bool	IsLowVers();					// устаревшая версия ПО в слоте
	bool	MinVers(int& aV);				// минимально необходимая версия ПО в слоте
	bool	CanProgramm()	{ return CanProgramm(m_TypSlot); };
	bool	IsProgram();

	void	GetSost();						// читать состояние КЛШ
	BYTE	NextIdent();
	void	LoadKeyNet();
	void	SaveKeyNet(const TCHAR* aKeyNet);
	void	GenKeyNet(const TCHAR* aKeyNet, BYTE* aKeyNetBin);
	void	ReOrder()	{m_TabDev.ReOrder(); m_TabLinkCLL.ReOrder(); m_TabLinkNet.ReOrder();};	// сортировка по номеру слота на ММИ

	void	XMLRemove();
	void	XMLUpdate();
	void	XMLAdd();
	void	XMLLoad(CComPtr<IXMLDOMNode> spNode);
	void	XMLPostLoad();

	static	CAtlString	nameTypDev(typDev aTypDev);
	static	CAtlString	nameTypBus(typBus aTypBus);
	static	bool		CanProgramm(typSlot aTypSlot);

	static  BYTE	s_KeyNet[16];				// ключ IDEA для формирования ключа доступа к сети КСЛов
	static	const	TCHAR*	s_Slot;
	static	const	TCHAR*	s_Number;
	static	const	TCHAR*	s_TypSlot;
	static	const	TCHAR*	s_Program;
	static	const	TCHAR*	s_NumUMi;
	static	const	TCHAR*	s_NumSLOTi;
};

/////////////////////////////////////////////////////////////////////////////
// КЛШ, подключенные к УМ

class CSlotTab : public list<CSlot*>
{
public:
	~CSlotTab()
	{
		for(list<CSlot*>::iterator i = begin(); i != end(); ++i)
		{
			delete *i;
		}
		clear();
	}
	void	GetSost();					// читать состояние КЛШ
	CSlot*	FindBlok(int aNumSLOT);		// поиск КЛШ
	bool	Insert(CSlot* aSlot);
	void	ReOrder();					// сортировка по номеру слота на ММИ
	void	GetSostL();					// читать состояние локального КЛШ
	void	GetSostR();					// читать состояние удаленного КЛШ
	void	SetNoInp();					// установить признак "не было данных от слота"
	void	TestOffL();					// проверка потери связи с локальным КЛШ
	void	TestOffR();					// проверка потери связи с удаленным КЛШ
};
