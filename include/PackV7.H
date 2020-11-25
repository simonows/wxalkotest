#pragma once

#include <Mes.h>
#include <IdentV7.h>

/////////////////////////////////////////////////////////////////////////////
// пакет устройств v7

class CPackV7
{
public:
	enum 
	{ 
		mxLen		= 1024, 
		minLen		= 15, 
		lenKeySys	= 16,
		lenVoice	= 96,
		lBlocks		= 8,				// длина блока шифрования
		lMaxBlocks	= mxLen/lBlocks,	// максимальное количество блоков шифрования
	};
	enum soundDirect
	{
		sdToLift	= 0x5F,	// направление ДИСПЕТЧЕР->ЛИФТ (кнопка нажата)
		sdToDisp	= 0x50,	// направление ЛИФТ->ДИФПЕТЧЕР (кнопка отпущена)
	};
private:

	BYTE	m_Buf[mxLen];	// тело пакета
	int		m_Len;			// длина пакета
public:
	enum classPack			// классы пакетов
	{
		cpNone		= 0,	// неопределен
		cpLink		= 3,	// служебные пакеты поддержки связи, анонсов маршрутов
		cpData		= 4,	// числовые данные (пакеты)
		cpVoice		= 7,	// голосовые данные (пакеты)
	};
	enum typPack			
	{
						// типы служебных пакетов поддержки связи, анонсов маршрутов (cpLink)
		tpConfLink	= 0,	// пакет подтверждения LINK-a без данных;
		tpAnnLink	= 1,	// пакет анонсов аннулирования маршрутов;
		tpAskLink	= 2,	// пакет запроса проверки Link-a;
		tpAnswLink	= 3,	// пакет ответа на запрос проверки Link-a;
		tpClosLink	= 4,	// пакет отписки от заданного Link-a;
		tpICMP		= 5,	// пакет с информацией об ошибках обработки пакета

						// типы цифровых пакетов (cpData)
		tpNone		= 0,	// пакет без данных
		tpMsgCLB	= 1,	// пакет с инкапсулированных сообщением КЛШ v5
		tpMsgLB		= 2,	// пакет с инкапсулированных сообщением ЛБv5,v6
		tpDirect	= 3,	// прямой запрос к устройству
		tpVideo		= 4,	// видео данные
        tpItv7P     = 5, // пакет с инкапсулированным сообщением ИТv7П
        tpItv7C     = 6, // пакет с инкапсулированным сообщением ИТv7С

		tpSDRIN		= 8,	// сервис SD RIN
		tpAuthen	= 9,	// аутенфикация

						// типы голосовых пакетов (cpVoice)
		tpG723		= 1,	// пакет формата G723.1 (CT8022)
		tpRALCWI	= 2,	// пакет формата RALCWI (CMX638)
		tpG723Slave	= 5,	// пакет формата G723.1 (CT8022) режим SLAVE
	};
	enum typDirAsk
	{
		tdAnswer	= 0,	// ответ
		tdQuery		= 1,	// запрос
	};
	enum typAsk
	{
						// общие запросы
		taNone			= -1,	// неопределен
		taWho			= 0,	// Кто ты ?
		taNVRAM			= 1,	// чтение/запись NVRAM
		taRestart		= 2,	// выполнить рестарт устройства
		taIAP			= 3,	// IAP сервис (загрузка микропрограмм)
		taStatusEth		= 4,	// состояние Ethernet интерфейса
		taStatusDirect	= 5,	// состояние направления
		taStatusRoute	= 6,	// состояние таблицы маршрутизации
		taStatusTxRx	= 7,	// Чтение/сброс статистики Tx/Rx Data
		taStatusNTP		= 8,	// дай состояние NTP сервиса
		taStatusWIFI	= 9,	// дай состояние WIFI интерфейса
		taServiceOTK	= 10,	// сервис ОТК

						// запросы сервиса SD RIN

		taDescrRIN		= 0,	// описатель
		taStatusRIN		= 1,	// состояние

						// запросы USB конвертера
		taStatusVoice	= 64,	// состотяние USB конвертера
		taOnVoice		= 65,	// включи ГГС
		taOffVoice		= 66,	// выключи ГГС

						// запросы видео (класс 4, тип 4)
		taTestVideo		= 0,	// тест состояния видео устройства
		taSubVideo		= 1,	// подписаться на получение видео
		taUnSubVideo	= 2,	// отписаться от получения  видео
		taSetVideo		= 3,	// установить параметры видео
		taParFrame		= 126,	// параметры очередного видеокадра
		taFrameVideo	= 127,	// видеокадр
	};
	classPack			m_ClassPack;	// класс пакета
	typPack				m_TypPack;		// тип пакета
	typAsk				m_TypAsk;		// тип запроса
	typDirAsk			m_Direct;		// направление
	int					m_Ident;		// идентификатор пакета
	int					m_Reply;		// подтверждение на предыдущий пакет
	CIdentV7			m_Orig;			// отправитель
	CIdentV7			m_Dest;			// получатель
	int					m_Retrans;		// счетчик ретрансляций

	CMes::typMes		m_TypMes;		// тип инкапсулируемого сообщения
	CMes::askFromMain	m_Ask;			// код запроса к устройству
	int					m_NumDev;		// номер устройства

	bool				m_bIsValid;		// признак верности пакета
	bool				m_bIsSubscrib;	// пакет содержит сообщение о подписке на изменение состояния ЛБ6Pro
	COleDateTime		m_InsTime;		// время вставки пакета в очередь

	static	const BYTE	DefaultKey[16];	// ключ шифрования по умолчанию
	static	const BYTE	ForKeySYS[16];	// ключ для получения ключа системы

	CPackV7();
	CPackV7(const CMes& aMes);
	CPackV7(const CPackV7& pk) {*this = pk;};
	CPackV7(const CIdentV7& aOrig, const CIdentV7& aDest);
	CPackV7(int len, const BYTE* buf);


	void	UnPack();				// разобрать пакет
	void	Pack();					// формировать пакет
	void	Clear();				// очистка пакета
	void	SetID(const CIdentV7& aOrig, const CIdentV7& aDest);
	void	SetIdent(int aIdent);	// установить идентификатор пакета		
	void	DelIdent();				// удалить идентификатор пакета 
	bool	SetMes(const CMes& mes);// инкапсулировать сообщение
	bool	GetMes(CMes& mes) const;// извлечь сообщение
	bool	SetVoice(typPack aTypPack, const BYTE* aBuf, int aLen);
	bool	SetVoice(const BYTE* aBuf, int aLen) { return SetVoice(tpG723, aBuf, aLen); };
	void	SetVoiceToLU(const BYTE* aBuf);
	bool	SetLink(typPack aTypPack, const BYTE* aBuf, int aLen);
	bool	SetAsk(typPack aTypPack, typAsk aTypAsk, typDirAsk aDirect, const BYTE* aBuf=0, int aLen=0);
	bool	SetAskData(const BYTE* aBuf=0, int aLen=0);
	void	CalcCrc();				// расчитать CRC1
	int		ChanCod(BYTE* aBuf, int aMaxLen, const BYTE* KeySys);		// канальное кодирование
	bool	ChanDecod(const BYTE* aBuf, int aLen, const BYTE* KeySys);	// канальное декодирование
	bool	AddRepl(BYTE aRepl);	// добавить в пакет подтверждение
	static BYTE	CalcCrc(const BYTE* aBody, int aLen);
	static unsigned short CalcCrc16(const BYTE* aBody, int aLen);
	bool	IsInIdent() const;
	bool	IsCloseLink() const;
	bool	TestCRC() const;
	bool	IsOnOffGGS() const;
	bool	IsRestart() const;
	int		Len() const			{ return m_Len; };
	int		LenCRC() const		{ return m_Len+2; };
	int		LenFul() const		{ return LenCRC()+1; };
	bool	IsValid() const		{ return m_bIsValid; };
	bool	IsNoMes() const		{ return LenCRC() <= minLen; };
	const	BYTE *Data() const	{ return m_Buf+13; };
	int		LenData() const		{ return m_Len-13; };

	operator const	BYTE *() const	{ return(m_Buf);};
	operator const	char *() const	{ return(reinterpret_cast<const char *>(m_Buf));};
	void operator  = (const CPackV7& pk);
};

/////////////////////////////////////////////////////////////////////////////
// класс входной очереди пакетов устройств v7

class InpQueuePackV7 : public queue<CPackV7>
{
	HANDLE	m_Lock;				// блокировочное события
public:
	InpQueuePackV7() {m_Lock = CreateEvent(NULL, FALSE, TRUE, NULL);};
	~InpQueuePackV7();

	bool	Put(const CPackV7& aPack);	// записать в очередь
	bool	Read(CPackV7& aPack);	// прочитать из очереди
};

