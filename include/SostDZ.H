#pragma once

#include <Mes.h>

/////////////////////////////////////////////////////////////////////////////
// CSostDZ состояние DZ цепочки

class CSostDZ
{
	int		m_CurBit;	// номер текущего бита состояния при просмотре DZ цепочки
public:
	BYTE	m_bSost[4];	// состояние DZ цепи
	int		m_iLen;		// длина DZ цепи
	int		m_iLvlWarn;	// уровень порога Warning DZ
	bool	m_bIn;		// режим In DZ
	bool	m_bOut;		// режим Out DZ
	bool	m_bTrg;		// триггерность состояния Warning DZ
	bool	m_bInvers;	// инверсия датчиков DZ

	CSostDZ()	{ Clear();};
	CSostDZ( CSostDZ &aSost);

	void		Clear();
	
	void		OpenScan(){ m_CurBit = 0;};	// начало сканирования состояния DZ цепочки
	bool		NextSost(int& aNumBit);		// дать следующее состояние DZ цепочки
	bool		tstBit(int aBit);			// проверка установки состояния
	bool		ExistSost();				// проверка наличия установленных состояний DZ
	bool		existsNul();
	bool		existsOne();
	void		SetMode(const BYTE *lpby);	// установить режимы DZ цепочки
	void		operator  = (const BYTE *lpby);
	void		operator  = (const CSostDZ& sl);
	void		operator  = (const CMes& mes);
	void		SetData(const BYTE* aSost, int aLen);
	operator	const	BYTE *() const	{ return(m_bSost);};
	operator	const	void *() const	{ return(m_bSost);};
	int			operator == (CSostDZ& sl);
	int			operator != (CSostDZ& sl);
	int			operator == (const BYTE *lpby);
	int			operator != (const BYTE *lpby);
};

