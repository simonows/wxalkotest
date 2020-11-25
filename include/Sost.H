#pragma once

#include <InfoCLB.h>

/////////////////////////////////////////////////////////////////////////////
// класс ответа оконечного устройства
//

class CSostDev
{
	enum	maxLen { mxLen	= 48 };
	int		m_Len;
	BYTE	m_bSost[mxLen];
public:
	enum bitHF			// сосотояний утсройства управления домофоном
	{
		bHfBegin	= 0,	// начало
		bHfRestart	= 0,	// был рестарт устройтва
		bHfOpenDoor	= 1,	// дыери открыты более заданоого интервала (1.25 час)
		bHfLowCurr	= 2,	// ток через электромагнит меньше нижнего порога
		bHfHighCurr	= 3,	// ток через электромагнит больше верхнего порога
		bHfPressKey	= 4,	// кнопка "Двери" нажата более заданного времени - 18с
		bHfNoLinkRec= 5,	// нет связи со всети трубками
		bHfJobErr	= 13,	// при обработке возникли ошибки
		bHfJobEnd	= 14,	// процесс обработки завершен
		bHfJobBeg	= 15,	// обработка команд начата
	};
	enum bitIT
	{
		bItBegin	= 0,	// начало
		bItServKey	= 0,	// вставлен сервисный ключ
		bItRestart	= 1,	// рестарт программы
		bItRez1		= 2,	// резерв 1
		bItMP		= 3,	// Проникновение в МП	Rev 2.0
		bItPower	= 4,	// Резервное питание	Rev 2.0
		bItCngDZ	= 5,	// изменилось состояние DZ цепи
		bItCngDev	= 6,	// Изменилось состояние внешних устройств	
		bItCngOut	= 7,	// Изменилось состояние выходов	Rev 2.1
		bItUSER1	= 8,	// USER 1
		bItUSER2	= 9,	// USER 2
		bItUSER3	= 10,	// USER 3
		bItUSER4	= 11,	// USER 4
		bItUSER5	= 12,	// USER 5
		bItUSER6	= 13,	// USER 6
		bItUSER7	= 14,	// USER 7
		bItUSER8	= 15,	// USER 8
		bItAsc		= 16,	// вызов диспетчера
		bItWrnDZ	= 17,	// внимание по DZ цепи
		bItStPrc	= 18,	// ИТ  занят/свободен
		bItDen		= 19,	// есть данные для передачи
		bItLF		= 20,	// был передан последний фрагмент
		bItErPrc	= 21,	// зафиксированы ошибки, процесс остановлен
		bItNDB		= 22,	// в буфере ИТ есть новые данные от оконечных устройств
		bItMF		= 23,	// промежуточный фрагмент
	};
	enum bitLB
	{
		sbBegin		=  0,		// начало
		sbKeyServ	=  0,		// вставлен сервисный ключ
		sbRestart	=  1,		// был рестарт блока
		sbRevers	=  2,		// многократный реверс дверей
		sbInpMP		=  3,		// проникновение в МП
		sbNonU		=  4,		// отсутсвует напряжение а цепи управления
		sbKeyStop	=  5,		// зажата кпопка стоп в кабине лифта
		sbOpen		=  6,		// не закрыта дверь кабины
		sbDestrSec	=  7,		// разрв цепи безопасности
		sbBadSenrUB	=  8,		// не сработал датчик УБ
		sbKZSecur	=  9,		// КЭ цени безопасности
		sbAsk		=  10,		// вызов диспетчера
		sbBadMove	=  11,		// Несанкционированое движение кабины
		sbEmergDrv	=  12,		// авария главного привода по УКСЛ
		sbEmergOpn	=  13,		// Авария привода дверей
		sbInpInnr	=  14,		// Проникновение в шахту
		sbBadUB		=  15,		// неиспровность УБ
		sbEmrgUPL	=  16,		// Аварийная блокировка УПЛ/УЖЛ
		sbOpenMP	=  17,		// Открыто МП
		sbLiftStop	=  18,		// Не открыты двери после движения
		sbNoJobDK	=  19,		// Не сработал датчик ДК
		sbOpenInnr	=  20,		// Не закрыта дверь шахты по УБ
		sbOnDriver	=  21,		// Главный привод включен
		sbRez1		=  22,		// РЕЗЕРВ (нефатальный бит пользователя)
		sbBadSemi	=  23,		// Перемычка пускателя,неиспр.оптосимистора
		sbLockRD	=  24,		// Блокировка РД без РКД
		sbAskMP		=  25,		// Уточнение вызова диспетчера: 0-кабина / 1 - МП
		sbBadGGS	=  26,		// Неисправность тракта ГГС кабины лифта
		sbStatL		=  27,		// Младший бит статистики
		sbStatH		=  28,		// Старший бит статистики
		sbRez29		=  29,		// РЕЗЕРВ
		sbRez30		=  30,		// РЕЗЕРВ
		sbRez31		=  31,		// РЕЗЕРВ
		sbUser1		=  32,		// Состояние USER1
		sbUser2		=  33,		// Состояние USER2
		sbUser3		=  34,		// Состояние USER3
		sbUser4		=  35,		// Состояние USER4
		sbRezU_LB	=  36,		// Резервное питание ЛБ
		sbVarUSec	=  37,		// Переменное напряжение в ЦБ
		sbFireRisk	=  38,		// Пожарная опасность
		sbBadBattery=  39,		// Неисправна батарея питания

		sbNoMove,				// кабина не двигалась со времени предыдущего изменения состояния
		sbNoInform,				// нет информации об устройстве
		sbNoLink,				// нет связи
		sbEnd
	};
	enum maxNumBit
	{
		mbMaxBit	= sbNoMove+1		// максимальное количество контролируемых бит в состоянии
	};
	enum bitCDC
	{
							// для ЛБ "КДК"
		sbKdkD33	=  5,	// Флаг неисправности
		sbKdkD34	=  6,	// Запрос связи
		sbKdkD35	=  7,	// Лифт отключен
		sbKdkD1		=  8,	// Фаза 1 ВА1 (380В/50Гц)
		sbKdkD2		=  9,	// Фаза 2 ВА1 (380В/50Гц)
		sbKdkD3		= 10,	// Фаза 3 ВА1 (380В/50Гц)
		sbKdkD4		= 11,	// Фаза 1 ВА2 (110В/50Гц)
		sbKdkD5		= 12,	// Фаза 2 ВА2 (110В/50Гц)
		sbKdkD6		= 13,	// Фаза 3 ВА2 (110В/50Гц)
		sbKdkD7		= 14,	// После ПР4 (110В)
		sbKdkD8		= 15,	// После ПР3 (36В)
		sbKdkD9		= 16,	// Ревизия (тублер на ЛБ)
		sbKdkD10	= 17,	// После ВК
		sbKdkD11	= 18,	// После ВНУ
		sbKdkD12	= 19,	// После СТОП
		sbKdkD13	= 20,	// После ВЛ
		sbKdkD14	= 21,	// После СПК
		sbKdkD15	= 22,	// После ДК
		sbKdkD16	= 23,	// После ДШ
		sbKdkD17	= 24,	// Двери шахты (+5В)
		sbKdkD18	= 25,	// РЗД
		sbKdkD19	= 26,	// РОД
		sbKdkD20	= 27,	// РД
		sbKdkD21	= 28,	// РТО
		sbKdkD22	= 29,	// Питание ЭМТ (=1-тормоз)
		sbKdkD23	= 30,	// Питание ЭМТ (=0-тормоз)
		sbKdkD24	= 31,	// Двери машинного помещения (триггерное)
		sbKdkD25	= 32,	// Нет фазы 1 ВА1
		sbKdkD26	= 33,	// Нет фазы 2 ВА1
		sbKdkD27	= 34,	// Нет фазы 3 ВА1
		sbKdkD28	= 35,	// 0
		sbKdkD29	= 36,	// Неисправноть в цепи привода дверей
		sbKdkD30	= 37,	// Не открыты двери после движения (РД=0&РТО=1)
		sbKdkD31	= 38,	// Разсинхранизация дверей ДК!=ДШ
		sbKdkD32	= 39,	// Неисправность в цепях главного привода
	};
	enum	bitProt
	{
		bpMECS		= 1,	// протокол MECS (OTIS)
		bpRMEM		= 2,	// протокол RMEM
	};
	enum typErrHF
	{
		tEfErrUART	= 1,	// ошибка связи по UART
		tEfBadMast	= 2,	// неправильный мастер код
		tEfTimeOut	= 3,	// превышено время ожидания ввода
	};
	CSostDev()							{ Clear();};
	CSostDev(const CSostDev& sd)		{ Clear(); *this = sd; };

	CInfoCLB	m_InfoCLB;			// информация КЛШ о блоке

	void		Clear();
	int			Len()	const			{ return(m_Len);};
	void		operator  = (const CSostDev& sl);
	operator const	BYTE *() const	{ return m_bSost;};
	bool		operator  == (const CSostDev& sl) const;
	BYTE		operator[](int nIndex) const;
	bool		tstBitDirect(int aBit) const;
	bool		tstBit(int aBit) const;
	void		setBit(int aBit);
	void		clrBit(int aBit);
	bool		SetData(const BYTE* aData, int aLen);
	bool		SetData7(const BYTE* aData, int aLen);
	int			GetData(BYTE* aData, int aMaxLen) const;
	void		Normaliz();
	typErrHF	GetCodErrHF();
	bool		IsError();
	bool		IsExtendet()			{ return m_Len == 8; };
	void		SetNoExtendet()			{ m_Len = 4; };	// установить длину 4
	int			GetStatDriv();					// прочитать состояние статистики движения
	bool		testNet() const			{ return(!tstBit(sbNoInform) && !tstBit(sbNoLink));};
	bool		EQ(const CSostDev& sNew)	const;

	CAtlString	BinStr();
	CAtlString	HexStr();

	static		bool IsNoValBit(bitLB aBit);	// бит не имеет самостоятельного значения
	static		bool IsError(bitLB aBit);
	static		UINT	naimSostRes(bitLB aBit);
	static		CAtlString	naimSost(bitLB aBit);
};
