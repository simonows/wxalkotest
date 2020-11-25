#pragma once

#include <Sost.h>

/////////////////////////////////////////////////////////////////////////////
// приоритеты

enum enPrior
{
	prDial		= 8,	// команды оператора
	prAskDriv	= 16,	// опрос ЛБ при открытом диалоговом окне
	prGetSost	= 60,	// приоритет опроса КЛШ
};

/////////////////////////////////////////////////////////////////////////////
// класс сообщения

class CMes
{
public:
	enum maxLen
	{
		mxLen	= 63,
	};
protected:
	int		m_iLenMes;			// длина сообщения в байтах
	BYTE	m_bMes[mxLen+1];	// сообщение
public:
	enum typPrefix	// код подгруппы устройств
	{
		tpxNone		= 0,	// не определен
		tpxHomeFon	= 1,	// домофон
	};
	enum typMes
	{
		tmNULL		= -1,		// несуществующая команда
		tmNone		= 0,		// кто ты ?
		tmRetMes4	= 4,		// возврат сообщения 4
		tmConfBlok	= 8,		// конфигурация КЛШ
		tmConfNVR	= 10,		// содержимое NVRAM
		tmConfNVREx	= 13,		// содержимое расширенного NVRAM
		tmAnswServ	= 14,		// ответ доронительных сервисов узла
		tmSostSlot	= 16,		// уточнённое состояния узлового устройтсва
		tmParDev	= 17,		// параметры оконечного устройства
		tmSostBlok	= 24,		// состояние КЛШ
		tmSostUnit	= 25,		// состояние устройства
		tmSostUnitD	= 26,		// параметры устройства после прямого запроса
		tmSostUnitDn= 28,		// параметры лифта после прямого запроса без перекодировки
		tmSostDevV7	= 29,		// состояние устройства от КЛШ V7
		tmSostDevDV7= 30,		// параметры устройства после прямого запроса от КЛШ V7
		tmSostDisp	= 31,		// параметры диспетчерской
		tmRestart	= 39,		// запрос рестарта
		tmGetSetDev	= 40,		// считывание/установка конфигурации КЛШ
		tmGetNVR	= 42,		// чтение содержимого NVRAM
		tmGetNVREx	= 45,		// чтение расширенного содержимого NVRAM
		tmAskServ	= 46,		// запрос к доронительным сервисам узла
		tmSetSost	= 48,		// изменение уточнённого состояния
		tmSetParDev	= 49,		// считывание/установка параметров оконечного устройства
		tmGetBlock	= 56,		// запрос состояния КЛШ
		tmGetSost	= 57,		// запрос уточнённого состояния устройства
		tmGetSostD	= 58,		// запрос уточнённого состояния прямым запросом устройства
		tmGetSostDn	= 60,		// запрос уточнённого состояния прямым запросом ЛБ без перекодировки
		tmGetSostV7	= 61,		// запрос уточнённого состояния устройства КЛШ V7
		tmGetSostDV7= 62,		// запрос уточнённого состояния прямым запросом устройства КЛШ V7
		tmGetDisp	= 63,		// запрос/установка параметров диспетчерской
	};
	enum askFromMain
	{
		amNULL		= -1,		// невозможный запрос
		amNone		= 0,		// кто ты ?
		amClrTrig	= 1,		// дай состояние, сброс тирггерных состояний
		amOffGGS	= 2,		// дай состояние, выключение разговорного тракта
		amOnGGS		= 3,		// дай состояние, включение разговорного тракта
		amOnGGSD	= 4,		// дай состояние, включение разговорного тракта
		amModeDZ	= 5,		// дай настройки DZ цепи
		amNumEtaz	= 5,		// дай номера этажей
		amNumKey	= 6,		// дай номера ключа
		amSetOut	= 7,		// дай состояние выходов, включить нагрузку
		amOnLift	= 7,		// дай состояние, включение лифта, сброс тиггерных состояний
		amClrOut	= 8,		// дай состояние выходов, выключить нагрузку
		amOffLift	= 8,		// дай состояние, выключение лифта
		amCodRest	= 9,		// дай код рестарта, сброс бита рестарта
		amValLift	= 10,		// дай значение датчиков лифта (ИТ)
		amValUB		= 11,		// дай значение датчиков УБ (DZ)
		amResCmd	= 12,		// ответ/результат исполнения команды
		amCommand	= 13,		// специфичные команды управления
		amRemote	= 14,		// удаленный терминал
		amGetNVRAM	= 15,		// чтение NVRAM
		amMSP		= 16,		// MSP запрос
		amIAP		= 17,		// IAP сервис
		amServ18	= 18,		// сервисы оповещения/управления
	};
	enum subAskFromMain			// подзапросы
	{
		saParamLU	= 0,		// дополнительные параметры лифтового блока
		saStatist	= 2,		// дай/установи статистику работы лифта
		saGetCache	= 3,		// дай образ контрольных точек на момент аварии лифта
	};
	enum subServ18				// номера сервисов оповещения/управления (amServ18)
	{
		saInOutMem	= 0,		// Сервис #0 - чтение / запись памяти (not used)	UT only
		saInBlkMem	= 1,		// Сервис #1 - чтение блока параметров (not used)	UT only
		saRestart	= 2,		// Сервис #2 - запрос на рестарт устройства	
		saTypLB		= 3,		// Сервис #3 - служба типов протокола ЛШ	ЛБv6
		saRealTime	= 4,		// Сервис #4 - служба реального времени	ЛБv6
		saGradOff	= 5,		// Сервис #5 - служба <плавного отключения> лифта	ЛБv6
		saTestStat	= 6,		// Сервис #6 - тест статистических данных движения	ЛБv6
		saTestMCS	= 7,		// Сервис #7 - служба самопроверки канала MCS	ЛБv6
		saTestGGS	= 8,		// Сервис #8 - служба самопроверки тракта  ГГС	ЛБv6
		saCntFlоors	= 10,		// Сервис #10 - служба счета этажей
		saTestBatt	= 11,		// Сервис #11 - служба проверки аккумуляторной батареи ЛБv6
		saAvailSrv	= 63,		// перечень доступных сервисов
	};
	enum subAskIAP				// подзапросы IAP сервиса
	{
		saIAPNone	= 0,		// 
		saIAPTotal	= 1,		// дай обощенные параметры модуля IAP
		saIAPPage	= 2,		// дай описатель N-ой страницы памяти
		saIAPClear	= 3,		// очисти N-ую страницу памяти
		saIAPPut	= 4,		// запиши X-й фрагмент в N-ую страницу памяти
		saIAPActiv	= 5,		// установи активной N-ую страницу памяти
		saIAPRefr	= 6,		// обнови состояние N-ой страницы памяти
	};
	enum typMsgDisp				// тип состояние диспетчеркой
	{
		tdNone		= 0,		// не определено
		tdAlert		= 1,		// оповещение
	};
	enum typAlert				// тип оповещения
	{
		taNone		= 0,		// не определено
		taOnGGS		= 1,		// включена ГГС
		taOffGGS	= 2,		// выключена ГГС
		taOffBlk	= 3,		// блок перешел в неработоспособное состояние
		taOnBlk		= 4,		// блок перешел в работоспособное состояние
		taNoLink	= 5,		// до блока нет пути
		taLink		= 6,		// появился путь до блока
		taOnKey		= 7,		// вставлен СК
		taOffKey	= 8,		// извлечен СК
		taRestart	= 9,		// рестарт
		taBusyCOM	= 10,		// занят порт
		taNoBUS		= 11,		// нет обмена по шине
		taLowVers	= 12,		// устаревшая версия ПО устройства
		taBadLine	= 13,		// нарушение линий связи (неисправность локальной шине КЛШ)
		taFailPwr	= 14,		// неисправен источник 60В (для КЛШ)
		taBadLine_	= 15,		// нарушение линий связи (неисправность локальной шине КЛШ)
		taFailPwr_	= 16,		// неисправен источник 60В (для КЛШ)
		taRezPwr	= 17,		// режим аварийного питания - возникновение
		taRezPwr_	= 18,		// режим аварийного питания - пропадание
		taNoLinkLLC	= 19,		// нет обмена в КСЛе по указанной связи + УМ
		taBadBatt	= 20,		// неисправна батарея питания КЛШ(КСЛ)
		taBadBatt_	= 21,		// исправна батарея питания КЛШ(КСЛ)
		taRezLink	= 22,		// работа на резерном канале
		taRezLink_	= 23,		// работа на основном канале
		taLinkLLC	= 24,		// появился обмен в КСЛе по указанной связи + УМ
	};
	enum reasonRez				// причина смены канала
	{
		rrLostLink		= 1,	// потеря связи
		rrReturnMain	= 2,	// возврат на основной канал
		rrInputData		= 3,	// пришли данные
	};
	enum rezNVRAM				// результат для msg13
	{
		rnGood		= 0,		// выполнено
		rnErrAdr	= 1,		// ошибка адреса
		rnErrLen	= 2,		// ошибка длины массива
		rnErrDan	= 3,		// ошибка данных запроса
		rnErrMes	= 128,		// сообщение не msg13
	};
	enum jobNVRAM				// действие для msg45
	{
		jnGet		= 0,		// прочитать
		jnSet		= 1,		// записать
	};
	enum typService				// тип сервиса для msg46
	{
		tsIAP		= 1,
		tsREQ		= 2,
		trsLLCON	= 3,		// статистика работы КСЛ
		tsLLBAT		= 4,		// самопроверка батареи
		tsETH		= 5,		// состояние Ethernet интерфейса КСЛа
	};
	enum bitExtended			// биты Extended запроса 10
	{
		beOpen		= 0,		// двереи открыты
		beUp		= 1,		// признак движения вверх
		beDown		= 2,		// признак движения вниз
		be15kg		= 3,		// есть 15 кг в кабине
		be90		= 4,		// есть 90% загрузки
		be110		= 5,		// есть 110% загрузки
		beDamag		= 6,		// ниличие аварии
		beSharp		= 7,		// датчик точной остановки
		beEnd
	};
	enum modeLift				// режим работы лифта
	{
		mlNorma		= 0,		// нормальный режим
		mlLoading	= 1,		// погрузка
		mlFire		= 2,		// пожарная опасность, перевозка пожарных подразделений
		mlRevision	= 3,		// ревизия                                                     
		mlDrivMP	= 4,		// управление из МП
		mlMP1		= 5,		// МП1                                                 
		mlParam		= 6,		// ввод параметров лифта                                       
		mlMP2		= 7,		// МП2                                                         
		mlCorrRun	= 8,		// корректировочный рейс                                       
		mlMornMode	= 9,		// утренний режим                                              
		mlEvenMode	= 10,		// вечерний режим                                             
		mlGuide		= 11,		// с проводником                                              
		mlRemotOff	= 12,		// дистанционное выключение                                   
		mlDamage	= 13,		// авария                                                     
		mlSeismic	= 14,		// сейсмическая опасность                                     
		mlHospital	= 15,		// больничный режим  
		mlEnd
	};
	enum flMes
	{
		flSetGate	= 0x01,		// признак использования шлюза локальной маршрутизации ( 1 - использовать m_SlGate)
		flNoIdent	= 0x02,		// признак отправки без Ident
	};
	CMes();
	CMes(const CMes& aMes);
	CMes(int aLen, const BYTE* aBody);
	CMes(int aUmDest, int aSlDest, int aUmOrig, int aSlOrig, typMes aTypMes, BYTE aMask);
	CMes(int aUmDest, int aSlDest, int aUmOrig, int aSlOrig, typAlert aTypAlert, const BYTE* aMask = NULL, int aLen = 0);
	CMes(int aUmDest, int aSlDest, int aUmOrig, int aSlOrig, typMes aTypMes, const BYTE* aMask, int aLen);
	CMes(int aUmDest, int aSlDest, int aUmOrig, int aSlOrig, typService aTypSrv, const BYTE* aBody, int aLen);
	CMes(int aUmDest, int aSlDest, int aUmOrig, int aSlOrig, int aDriv, typMes aTypMes, askFromMain aAsk, const BYTE* aMask = NULL, int aLen = 0, int aDuration = -1);
	CMes(int aUmDest, int aSlDest, int aUmOrig, int aSlOrig, int aDriv, subServ18 aSubServ18, const BYTE* aBody = 0, int aLen = 0);
	CMes(int aUmDest, int aSlDest, int aUmOrig, int aSlOrig, jobNVRAM aJobNVRAM, long aAddr, int aLen, const BYTE* aBody = NULL);
	CMes(int aUmDest, int aSlDest, int aUmOrig, int aSlOrig, rezNVRAM aRezNVRAM, long aAddr, int aLen, const BYTE* aBody = NULL);
	CMes(int aUmDest, int aSlDest, int aUmOrig, int aSlOrig, typPrefix aTypPrefix, BYTE aAddr);
	CMes(int aUmDest, int aSlDest, int aUmOrig, int aSlOrig, typPrefix aTypPrefix, BYTE aAddr, askFromMain aAsk, int aLen, const BYTE* aBody = NULL);

	int			m_SubNet;			// номер подсети
	int			m_SlGate;			// слот локальной маршрутизации
	int			m_Flags;			// признаки 
	int			m_Prior;			// приоритет
	int			m_iUmOrig;			// УМ отправителя
	int			m_iSlOrig;			// слот отправителя
	int			m_iUmDest;			// УМ получателя
	int			m_iSlDest;			// слот получателя
	int			m_iHopCount;		// счетчик прохождение через маршрутизаторы
	int			m_iDev;				// номер устройства на шине
	typMes		m_iTypMes;			// тип сообщение
	typPrefix	m_TypPrefix;		// код подгруппы устройств
	askFromMain	m_iAsk;				// код запроса
	typMsgDisp	m_TypMsgDisp;		// тип состояние диспетчеркой
	typAlert	m_TypAlert;			// тип оповещения
	CSostDev	m_Sost;				// доп.параметры

	void		Clear();
	void		UnPack();				// разобрать сообщение
	int			IncHopCount();			// увеличить счетчик проходов через маршрутизаторы (возврат уже увеличенного счетчика)
	bool		IsGood();				// достоверное сообщение
	bool		IsNoBody()			{ return m_iLenMes <= 6;};
	rezNVRAM	GetNVRAM(long& aAdrNVRAM, int& aLen, BYTE* aBuf, int aMaxLen);
	bool		GetNVRAM(int& aAdr, unsigned short& aValue);
	bool		GetLbNVRAM(int& aAddr, unsigned short& aVal, bool& aIsWord);
	int			GetIAP(BYTE* aBuf, int aMaxLen);
	int			GetAlert(void* aBuf, int aMaxLen);
	void		SetOrig(int aUm, int aSlot);
	void		SetDest(int aUm, int aSlot);
	void		SetDev(int aDev);
	bool		Conv6To8(int aLen6, int aPosBeg, int aLen8, BYTE* aDann8, int aMaxLen8);
	bool		SetDann(int aLen, const BYTE* aBody);
	void		SetTypMes(typMes aTypMes);
	CAtlString	Str(const TCHAR* aTitle = 0);
	bool		SetByte(int aIndexData, BYTE aByte);
	bool		GetSTAT(long &TotOn, long &TimOn, long &TotDor, long &TimDor);

	const BYTE*	Body() const		{ return m_bMes;};
	const BYTE*	Data() const		{ return m_bMes+6;};

	operator const	BYTE *() const	{ return &m_bMes[6];};
	operator const	void *() const	{ return m_bMes;};
	BYTE		operator[](int nIndex) const;
	int			Len() const			{ return m_iLenMes;};
	int			LenData() const		{ return m_iLenMes-6;};

	int			LvlAsk10() const;			// уровень данных в msg26 ask10

// методы Extended запроса 10

	bool		TestBit(bitExtended aBit);		// состояние дополнитльеных битов
	int			CurLvl();				// текщий этаж
	modeLift	Mode();					// режим работы лифта
	//int			TypDev();				// тип лифтового блока
	//int			TypExt();				// модуля расширения
	int			CodDmamge();			// код аварии

// методы Full запроса 10

	bool		TestAsk(int aNumLvl);	// вызовы с этажей
	bool		TestCmd(int aNumLvl);	// панель приказов

	static	CAtlString   nameMode(modeLift aMode);
	static		bool Conv6To8(int aLen6, const BYTE* aDann6, int aLen8, BYTE* aDann8, int aMaxLen8);
	static		bool Conv8To6(int aLen8, const BYTE* aDann8, BYTE* aDann6, int aMaxLen6);

	static	void SetAdr(BYTE* aAdr, int aUM, int aSlot);
	static	void GetAdr(const BYTE *aAdr, int& aUM, int& aSlot);
};
