#include "stdafx.h"

#include <SostDZ.H>

/////////////////////////////////////////////////////////////////////////////
// CSostDZ описание подключение терминала

CSostDZ::CSostDZ( CSostDZ &aSost)
{
	Clear();

	memcpy(m_bSost, aSost.m_bSost, sizeof(m_bSost));
	m_iLen		= aSost.m_iLen;
	m_iLvlWarn	= aSost.m_iLvlWarn;
	m_bIn		= aSost.m_bIn;
	m_bOut		= aSost.m_bOut;
	m_bTrg		= aSost.m_bTrg;
	m_bInvers	= aSost.m_bInvers;
}

void	CSostDZ::Clear()
{
	memset(m_bSost, 0, sizeof(m_bSost));
	m_iLen		= 0;
	m_iLvlWarn	= 0;
	m_bIn		= true;
	m_bOut		= false;
	m_bTrg		= false;
	m_bInvers	= false;
}

bool CSostDZ::NextSost(int& aNumBit)
{
	for(; m_CurBit < 24; m_CurBit++)	// возможно сделать цикл до m_iLen
	{
		int	nByte = m_CurBit / 6;
		int nBit  = 1 << m_CurBit % 6;

		if((m_bSost[nByte] & nBit) != 0)
		{
			aNumBit = m_CurBit++;
			return(true);
		}
	}
	return(false);
}

bool CSostDZ::tstBit(int aBit)
{
	if(aBit >= 0 && aBit < 24)
	{
		int	nByte = aBit / 6;
		int nBit  = 1 << aBit % 6;

		return((m_bSost[nByte] & nBit) != 0);
	}
	return(false);
}

bool CSostDZ::ExistSost()
{
	for(int j = 0; j < 24; j++)	// возможно сделать цикл до m_iLen
	{
		int	nByte = j / 6;
		int nBit  = 1 << j % 6;

		if((m_bSost[nByte] & nBit) != 0)
		{
			return(true);
		}
	}
	return(false);
}


void CSostDZ::operator=(const CMes& mes)	
{
	memcpy(m_bSost, static_cast<const BYTE*>(mes)+2, sizeof(m_bSost));
}

void CSostDZ::operator=(const CSostDZ& sl)
{
	memcpy(m_bSost, sl, sizeof(m_bSost));
}

void CSostDZ::operator=(const BYTE *lpby)
{
	memcpy(m_bSost, lpby, sizeof(m_bSost));
}


void CSostDZ::SetData(const BYTE* aSost, int aLen)
{
	Clear();

	memcpy(m_bSost, aSost, min(sizeof(m_bSost), aLen));
}

void CSostDZ::SetMode(const BYTE *lpby)
{
	m_iLen		= lpby[0] & 0x1F;
	m_iLvlWarn	= ((lpby[1] & 0x3F) >> 2) | ((lpby[2] & 0x1) << 4);
	m_bIn		= lpby[1] & 0x2? true: false;
	m_bOut		= lpby[1] & 0x1? true: false;
	m_bTrg		= lpby[2] & 0x2? true: false;
	m_bInvers	= lpby[2] & 0x4? true: false;
}

int	CSostDZ::operator == (CSostDZ& sl) 
{
	return(memcmp(*this, sl, 4)==0);
}

int	CSostDZ::operator != (CSostDZ& sl) 
{
	return( !(*this == sl));
}

int	CSostDZ::operator == (const BYTE * sl) 
{
	return(memcmp(*this, sl, 4)==0);
}

int	CSostDZ::operator != (const BYTE * sl) 
{
	return( !(*this == sl));
}

bool CSostDZ::existsNul()
{
	for(int i = 0; i < 24; i++)
	{
		if(!tstBit(i)) return true;
	}
	return false;
}

bool CSostDZ::existsOne()
{
	for(int i = 0; i < 24; i++)
	{
		if(tstBit(i)) return true;
	}
	return false;
}

