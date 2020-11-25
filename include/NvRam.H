#pragma once

/////////////////////////////////////////////////////////////////////////////
// CNvRam содержание NVRAM

class CNvRam
{
	enum
	{
		lenNVRAM	= 64,				// длина в 16 битных словах
	};
	int				m_iIndex;			// номер текущего слова для приема из КЛШ
	unsigned short	m_shBuf[lenNVRAM];	// содержимое
public:

	CNvRam()	{ Clear();};

	void	Clear();

	int		NextIndex();
	bool	GetWord(int aIndex, unsigned short& aWord);
	bool	SetWord(int aIndex, unsigned short  aWord);
	bool	GetLowByte(int aIndex, BYTE& aByte);
	bool	GetHigByte(int aIndex, BYTE& aByte);
	bool	ExistDev(int aNum, int& aFlagSet);
	static	int	NumWordLB(int aNum);
	//bool	RemoveDev(int aNum);

	void	ClrMask(int aIndex);
	void	SetMask(int aIndex);
	bool	GetMask(int aIndex);

	operator const void*()	{ return(m_shBuf);};
	void	operator  = (const BYTE *lpby);
	int		operator == (CNvRam& sl);
	int		Len()	{ return(sizeof(m_shBuf));};

	long	m_lMaska[2];		// маска известности слов

	static	BYTE	s_Tab[lenNVRAM];	// таблица переходов для версии 1.7
};

