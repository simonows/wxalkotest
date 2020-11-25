#pragma once

#include <Mes.h>

/////////////////////////////////////////////////////////////////////////////
// значение датчиков оконечного устройства

class CSostSens
{
protected:
	enum	maxLen { mxLen	= 36 };
	BYTE	m_bSost[mxLen];
	int		m_Len;
public:
	enum bitSensLvl0
	{
		bSnBegin	= 0,	// начало
		bSnBP		= 0,	// ВП – напряжение в цепи управления
		bSnSTOP1	= 1,	// СТОП1 – до кнопки СТОП
		bSnSTOP2	= 2,	// СТОП2 – после кнопки СТОП
		bSnDK1		= 3,	// ДК1 – до контакта дверей кабины
		bSnDK2		= 4,	// ДК2 – после контакта дверей кабины
		bSnRKD		= 5,	// РКД – реле контроля дверей (конец цепи безопасности)
		bSnROD		= 6,	// РОД – реле привода дверей на открытие
		bSnRZD		= 7,	// РЗД – реле привода дверей на закрытие
		bSnRD		= 8,	// РД – реле движения
		bSnABL		= 9,	// АБЛ – автоматическая блокировка УПЛ/УЛЖ (бит пользователя)
		bSnMP		= 10,	// МП – датчик проникновения в машинное помещение
		bSnCall		= 11,	// ВЫЗОВ – кнопка ВЫЗОВ в лифте и МП.
		bSnUKSL		= 12,	// УКСЛ – наличие импульсов от УКСЛ
		bSnRIN		= 13,	// РИН – обнаружен Речевой Информатор
		bSbOnLift	= 14,	// ВКЛ – включение лифта
		bSbAkMP		= 15,	// ВЫЗОВ МП – вызов диспетчера из МП
		bUSER1		= 16,	// USER1 – вход пользователя 1
		bUSER2		= 17,	// USER2 – вход пользователя 2
		bUSER3		= 18,	// USER3 – вход пользователя 3
		bUSER4		= 19,	// USER4 – вход пользователя 4
		b220V 		= 22,	// 220В – наличие сетевого питания
		bOUT1 		= 23,	// OUT1 – состояние токового выхода 1

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
	enum bitSensLvl1
	{
		bSnSostDors	= 24,	// Состояние дверей 0- закрыты, 1- открыты
		bSnMoveUp	= 25,	// Признак движения вверх
		bSnMoveDown	= 26,	// Признак движения вниз
		bSn15Kg		= 27,	// Есть 15кг в кабине
		bSn90Pers	= 28,	// Есть 90% загрузки кабины
		bSn110Pers	= 29,	// Есть 110% загрузки кабины
		bSnCrash	= 30,	// Наличие аварии на станции управления
		bSnTuneStop	= 31,	// Датчик точной остановки
		bSnDevUKSL	= 32,	// Импульсы от аппаратного УКСЛ
	};
	enum bitUT
	{
		bSnUtUser1	= 0,	// USER1 – вход пользователя 1	
		bSnUtUser2	= 1,	// USER2 – вход пользователя 2	
		bSnUtUser3	= 2,	// USER3 – вход пользователя 3	
		bSnUtUser4	= 3,	// USER4 – вход пользователя 4	
		bSnUtUser5	= 4,	// USER5 – вход пользователя 5 (MP)	
		bSnUtUser6	= 5,	// USER6 – вход пользователя 6 (CALL)	
		bSnUtUser7	= 6,	// USER7 – вход пользователя 7 (ABL)	
		bSnUtUser8	= 7,	// USER8 – вход пользователя 8 (UKSL)	
		bSnUtAsk	= 8,	// ВЫЗОВ – кнопка ВЫЗОВ на терминале	
		bSnUtOn		= 9,	// Кнопка «Вкл.»	Rev 2.0
		bSnUt220v	= 11,	// 220В – наличие сетевого питания	Rev 2.0
	};
	CSostSens()							{ Clear();};

	void		Clear();
	operator	const	BYTE *() const	{ return(m_bSost);};
	operator	const	void *() const	{ return(m_bSost);};
	int			Len()  const			{ return(m_Len);};
	void		operator  = (const CMes& mes);
	void		operator  = (const CSostSens& sl);
	void		SetData(const BYTE* aSost, int aLen);
	int			LvlAsk10() const;
	bool		tstBit(int aBit) const;
	void		setBit(int aBit);
	void		clrBit(int aBit);
	bool		existsNul();
	bool		existsOne();
	bool		chngBitLvl0(bitSensLvl0 aBit, CSostSens& aNewSost) const;

// методы Extended запроса 10

	int			CurLvl() const;				// текщий этаж
	int			Mode() const;				// режим работы лифта
	int			TypDev() const;				// тип лифтового блока
	//int			TypExt() const;				// модуля расширения
	int			CodDmamge() const;			// код аварии

	bool		chngBitLvl1(bitSensLvl1 aBit, CSostSens& aNewSost) const;
	bool		chngCurLvl(CSostSens& aNewSost) const;
	bool		chngMode(CSostSens& aNewSost) const;
	bool		chngTypDev(CSostSens& aNewSost) const;
	//bool		chngTypExt(CSostSens& aNewSost) const;
	bool		chngCodDmamge(CSostSens& aNewSost) const;

// методы Full запроса 10

	bool		TestAsk(int aNumLvl) const;	// вызовы с этажей
	bool		TestCmd(int aNumLvl) const;	// панель приказов
	long		ValAsk() const;				// вызовы с этажей
	long		ValCmd() const;				// панель приказов

	bool		chngValAsk(CSostSens& aNewSost) const;
	bool		chngValCmd(CSostSens& aNewSost) const;
};

