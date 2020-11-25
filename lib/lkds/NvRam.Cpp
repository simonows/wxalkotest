#include "stdafx.h"

#include "NvRam.H"

/////////////////////////////////////////////////////////////////////////////
// CNvRam описание подключение терминала

BYTE CNvRam::s_Tab[lenNVRAM] = { 0, 8,16, 1, 2, 3, 4, 5, 6, 7, 9,10,11,12,13,14,
								15,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,
								32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,
								48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63};

void	CNvRam::Clear()
{
	m_iIndex		= 0;

	memset(m_shBuf,  0, sizeof(m_shBuf));
	memset(m_lMaska, 0, sizeof(m_lMaska));
}

int		CNvRam::NextIndex()
{
	int	Index = 0;

	if(m_iIndex >= lenNVRAM)	m_iIndex = 0;

	Index = s_Tab[m_iIndex++];

	return(Index);
}

void	CNvRam::ClrMask(int aIndex)
{
	if(aIndex >= 0 && aIndex < lenNVRAM)
	{
		m_lMaska[aIndex/32] &= ~((long)1 << aIndex%32);
	}
}

void	CNvRam::SetMask(int aIndex)
{
	if(aIndex >= 0 && aIndex < lenNVRAM)
	{
		m_lMaska[aIndex/32] |= (long)1 << aIndex%32;
	}
}

bool	CNvRam::GetMask(int aIndex)
{
	if(aIndex >= 0 && aIndex < lenNVRAM)
	{
		return((m_lMaska[aIndex/32] & ((long)1 << aIndex%32)) != 0);
	}
	return(false);
}

bool	CNvRam::GetWord(int aIndex, unsigned short& aWord)
{
	aWord	= 0;

	if(aIndex >= 0 && aIndex < lenNVRAM)
	{
		aWord = m_shBuf[aIndex];
		return(GetMask(aIndex));
	}
	return(false);
}

bool	CNvRam::SetWord(int aIndex, unsigned short aWord)
{
	if(aIndex >= 0 && aIndex < lenNVRAM)
	{
		m_shBuf[aIndex] = aWord;
		SetMask(aIndex);
		return(true);
	}
	return(false);
}

bool	CNvRam::GetLowByte(int aIndex, BYTE& aByte)
{
	if(aIndex >= 0 && aIndex < lenNVRAM)
	{
		aByte = (BYTE)(m_shBuf[aIndex] & 0xFF);
		return(GetMask(aIndex));
	}
	return(false);
}

bool	CNvRam::GetHigByte(int aIndex, BYTE& aByte)
{
	if(aIndex >= 0 && aIndex < lenNVRAM)
	{
		aByte = (BYTE)((m_shBuf[aIndex] >> 8) & 0xFF);
		return(GetMask(aIndex));
	}
	return(false);
}

int		CNvRam::NumWordLB(int aNum)
{
	return (aNum / 16) + 32;
}

bool	CNvRam::ExistDev(int aNum, int& aFlagSet)
{
	bool	rc = false;

	if(aNum >=0 && aNum <= 255)
	{
		unsigned short	Word = 0;

		if(GetWord(NumWordLB(aNum), Word))
		{
			aFlagSet = (Word & (1 << (aNum % 16)))? 1: 0;

			rc = true;
		}
	}
	return(rc);
}
/*
bool	CNvRam::RemoveDev(int aNum)
{
	bool	rc = false;

	if(aNum >=0 && aNum <= 255)
	{
		unsigned short	Word	= 0;
		int				NumWord	= NumWordLB(aNum);

		if(GetWord(NumWord, Word))
		{
			unsigned short	Maska	= 1 << (aNum % 16);

			Word	&= ~Maska;

			SetWord(NumWord, Word);

			rc = true;
		}
	}
	return(rc);
}
*/
void	CNvRam::operator=(const BYTE *lpby)	
{ 
	if(lpby != NULL) memcpy(m_shBuf, lpby, sizeof(m_shBuf));
}

int	CNvRam::operator == (CNvRam& sl) 
{
	return(memcmp(*this, sl, sizeof(m_shBuf))==0);
}

