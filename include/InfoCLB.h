#pragma once

/////////////////////////////////////////////////////////////////////////////
// класс информации КЛШ о блоке 

class CInfoCLB
{
	int		m_Len;
	enum	maxLen { mxLen	= 2 };
	BYTE	m_bSost[mxLen];
public:
	enum bitV5			// биты доп.информации для шины v5
	{
		b5ExistMove	= 0,	// признак наличия движения
		b5NoInform	= 10,	// нет информации об устройстве
		b5NoLink	= 11,	// нет связи
	};
	CInfoCLB()							{ Clear();};
	void		Clear();
	int			Len()		const		{ return(m_Len);};
	void		operator  = (const CInfoCLB& sl);
	bool		operator  == (const CInfoCLB& sl) const;
	bool		tstBit(int aBit) const;
	void		setBit(int aBit);
	void		clrBit(int aBit);
	bool		SetData(const BYTE* aData, int aLen);
	int			GetData(BYTE* aData, int aMaxLen) const;
	void		SetMove();
	BYTE		operator[](int nIndex) const;
	bool		testNet()	const		{ return(!tstBit(b5NoInform) && !tstBit(b5NoLink));};
	bool		NewError(const CInfoCLB& sNew)	const;
	bool		EQ(const CInfoCLB& sNew)	const;

	CAtlString	BinStr()	const;
	CAtlString	HexStr()	const;
};
