#pragma once

/////////////////////////////////////////////////////////////////////////////
// класс состояния КЛШ
//

class CSostCLB
{
	enum maxLen
	{
		mxLen = 4
	};
	BYTE	m_bSost[mxLen];		// состояние
public:
	enum sostByte0
	{
		sbRestart	=  0x1,		// рестарт блока
		sbBadBuf	=  0x2,		// нарушение буферной памяти
		sbBadRam	=  0x4,		// нарущение RAM
		sbBadLink	=  0x8,		// нарущение линий связи
		sbOnGGS		=  0x10,	// включен разговорный тракт
		sbOnTrass	=  0x20,	// включение режима слежения
	};
	enum sostByte2
	{
		sbKey		=  0x1,		// вставка ключа
		sbChngNet	=  0x2,		// изменился состав сети (для КРК)
		sbFailPwr	=  0x4,		// неисправен источник 60В (для КЛШ)
		sbRezPwr	=  0x8,		// резерное питание (для всех узловых устройств)
		sbBadBatt	=  0x10,	// Неисправность аккумуляторной батареи
		sbRezLink	=  0x20,	// работа на резервном канале
	};
	enum sostByte3
	{
		sbIAP		=  0x1,		// поддержка IAP
		sbSelQuest	=  0x2,		// выделенный опрос
		sbLLCON		=  0x4,		// поддержка LLCON
		sbLLBAT		=  0x8,		// поддержка LLBAT
		sbETH		=  0x10,	// поддержка сервиса состояния Ethernet интерфейса КСЛа
	};
	enum typRestart
	{
		trOn		=  0,		// включение электропитания
		trPult		=  1,		// от диспетчерского пульта
		trTimer		=  2,		// аварийный таймер
	};
	
	CSostCLB()									{ Clear();};
	CSostCLB(const BYTE *lpby);
	void		Clear()							{ memset(m_bSost,0,mxLen);};
	operator	const	BYTE *() const	{ return(m_bSost);};
	operator	const	void *() const			{ return(m_bSost);};
	int			Len()							{ return(mxLen);};
	void		operator  = (const BYTE *lpby)	{ if(lpby != 0) memcpy(m_bSost,lpby,mxLen);};
	bool		testSost(sostByte0 aSost)		{ return((m_bSost[0] & aSost) != 0);};
	bool		testSost2(sostByte2 aSost)		{ return((m_bSost[2] & aSost) != 0);};
	bool		testSost3(sostByte3 aSost)		{ return((m_bSost[3] & aSost) != 0);};
	void		setSost(sostByte0 aSost)		{ m_bSost[0] |= aSost; };
	void		setSost2(sostByte2 aSost)		{ m_bSost[2] |= aSost; };
	void		setSost3(sostByte3 aSost)		{ m_bSost[3] |= aSost; };
	void		clrSost(sostByte0 aSost)		{ m_bSost[0] &= ~aSost; };
	void		clrSost2(sostByte2 aSost)		{ m_bSost[2] &= ~aSost; };
	void		clrSost3(sostByte3 aSost)		{ m_bSost[3] &= ~aSost; };
	int			operator == (CSostCLB& sb);
	int			operator != (CSostCLB& sb);
};

