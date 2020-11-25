#include "stdafx.h"

#include <SostSens.h>

/////////////////////////////////////////////////////////////////////////////
// класс ответа оконечного устройства

void CSostSens::Clear()					
{ 
	m_Len	= 0;
	memset(m_bSost, 0, mxLen);
}

bool CSostSens::tstBit(int aBit) const
{
	if(aBit >= 0 && aBit < 120)
	{
		int	nByte = aBit / 6;
		int nBit  = 1 << aBit % 6;

		return((m_bSost[nByte] & nBit) != 0);
	}
	return(false);
}

void CSostSens::setBit(int aBit)
{
	if(aBit >= 0 && aBit < 120)
	{
		int	nByte = aBit / 6;
		int nBit  = 1 << aBit % 6;

		m_bSost[nByte] |= nBit;
	}
}

void CSostSens::clrBit(int aBit)
{
	if(aBit >= 0 && aBit < 120)
	{
		int	nByte = aBit / 6;
		int nBit  = 1 << aBit % 6;

		m_bSost[nByte] &= ~nBit;
	}
}


bool CSostSens::existsNul()
{
	for(int i = 0; i < 24; i++)
	{
		if(!tstBit(i)) return true;
	}
	return false;
}

bool CSostSens::existsOne()
{
	for(int i = 0; i < 24; i++)
	{
		if(tstBit(i)) return true;
	}
	return false;
}

bool CSostSens::chngBitLvl0(bitSensLvl0 aBit, CSostSens& aNewSost) const
{
	bool	rc = false;

	if(LvlAsk10() < 0)
	{
		rc = aNewSost.LvlAsk10() >= 0;
	}
	else
	{
		if(aNewSost.LvlAsk10() >= 0)
		{
			rc = tstBit(aBit) != aNewSost.tstBit(aBit);
		}
	}
	return rc;
}

bool CSostSens::chngBitLvl1(bitSensLvl1 aBit, CSostSens& aNewSost) const
{
	bool	rc = false;

	if(LvlAsk10() < 1)
	{
		rc = aNewSost.LvlAsk10() >= 1;
	}
	else
	{
		if(aNewSost.LvlAsk10() >= 1)
		{
			rc = tstBit(aBit) != aNewSost.tstBit(aBit);
		}
	}
	return rc;
}

void CSostSens::operator=(const CMes& mes)	
{
	Clear();

	if(mes.m_iTypMes == CMes::tmSostUnitDn)
	{
		m_Len = 8;
	}
	else
	{
		int Lvl	= mes.LvlAsk10();

		if(Lvl == 0)		m_Len = 4;
		else if(Lvl == 1)	m_Len = 12;
		else if(Lvl == 2)	m_Len = 20;
	}

	memcpy(m_bSost, static_cast<const BYTE*>(mes)+2, m_Len);
}

void CSostSens::operator=(const CSostSens& sl)
{
	Clear();

	m_Len	= sl.m_Len;
	memcpy(m_bSost, sl, sizeof(m_bSost));
}

void CSostSens::SetData(const BYTE* aSost, int aLen)
{
	Clear();

	m_Len	= min(sizeof(m_bSost), aLen);

	memcpy(m_bSost, aSost, m_Len);
}

int	CSostSens::LvlAsk10() const
{
	if(m_Len < 4)		return(-1);
	else if(m_Len < 12)	return(0);
	else if(m_Len < 20)	return(1);
	else				return(2);
}

int		CSostSens::CurLvl() const
{
	if(LvlAsk10() > 0)
	{
		int CurLvl = m_bSost[4+2] & 0x3F;

		if( CurLvl < 0 || CurLvl > 32) CurLvl = 0;

		return(CurLvl);
	}
	return 0;
}

int	CSostSens::Mode() const
{
	if(LvlAsk10() > 0)
	{
		int Mode = m_bSost[4+3] & 0xF;

		return(Mode);
	}
	return 0;
}

int	CSostSens::TypDev() const
{
	if(LvlAsk10() > 0)
	{
		int Typ = m_bSost[4+4] & 0x3F;

		return(Typ);
	}
	return 0;
}

//int	CSostSens::TypExt() const
//{
//	if(LvlAsk10() > 0)
//	{
//		int Typ = m_bSost[4+5] & 0x3F;
//
//		return(Typ);
//	}
//	return 0;
//}

int		CSostSens::CodDmamge() const
{
	if(LvlAsk10() > 0)
	{
		int Cod = ((m_bSost[4+6]&0x30) >> 4) | ((m_bSost[4+7]&0x3f) << 2);

		return(Cod);
	}
	return 0;
}

bool	CSostSens::chngCurLvl(CSostSens& aNewSost) const
{
	bool	rc = false;

	if(LvlAsk10() < 1)
	{
		rc = aNewSost.LvlAsk10() >= 1;
	}
	else
	{
		if(aNewSost.LvlAsk10() >= 1)
		{
			rc = CurLvl() != aNewSost.CurLvl();
		}
	}
	return rc;
}

bool	CSostSens::chngMode(CSostSens& aNewSost) const
{
	bool	rc = false;

	if(LvlAsk10() < 1)
	{
		rc = aNewSost.LvlAsk10() >= 1;
	}
	else
	{
		if(aNewSost.LvlAsk10() >= 1)
		{
			rc = Mode() != aNewSost.Mode();
		}
	}
	return rc;
}

bool	CSostSens::chngTypDev(CSostSens& aNewSost) const
{
	bool	rc = false;

	if(LvlAsk10() < 1)
	{
		rc = aNewSost.LvlAsk10() >= 1;
	}
	else
	{
		if(aNewSost.LvlAsk10() >= 1)
		{
			rc = TypDev() != aNewSost.TypDev();
		}
	}
	return rc;
}

//bool	CSostSens::chngTypExt(CSostSens& aNewSost) const
//{
//	bool	rc = false;
//
//	if(LvlAsk10() < 1)
//	{
//		rc = aNewSost.LvlAsk10() >= 1;
//	}
//	else
//	{
//		if(aNewSost.LvlAsk10() >= 1)
//		{
//			rc = TypExt() != aNewSost.TypExt();
//		}
//	}
//	return rc;
//}

bool	CSostSens::chngCodDmamge(CSostSens& aNewSost) const
{
	bool	rc = false;

	if(LvlAsk10() < 1)
	{
		rc = aNewSost.LvlAsk10() >= 1;
	}
	else
	{
		if(aNewSost.LvlAsk10() >= 1)
		{
			rc = CodDmamge() != aNewSost.CodDmamge();
		}
	}
	return rc;
}

bool	CSostSens::TestAsk(int aNumLvl) const
{
	if(LvlAsk10() > 1)
	{
		aNumLvl--;

		if(aNumLvl >= 0 && aNumLvl < 24)
		{
			int	nByte = aNumLvl / 6;
			int nBit  = 1 << aNumLvl % 6;

			return((m_bSost[4+8+nByte] & nBit) != 0);
		}
	}
	return false;
}

bool	CSostSens::TestCmd(int aNumLvl) const
{
	if(LvlAsk10() > 1)
	{
		aNumLvl--;

		if(aNumLvl >= 0 && aNumLvl < 24)
		{
			int	nByte = aNumLvl / 6;
			int nBit  = 1 << aNumLvl % 6;

			return((m_bSost[4+8+4+nByte] & nBit) != 0);
		}
	}
	return false;
}

long	CSostSens::ValAsk() const
{
	long	Val = 0;

	if(LvlAsk10() > 1)
	{
		Val |= (long) (m_bSost[4+8+0]&0x3F);
		Val |= (long)((m_bSost[4+8+1]&0x3F) << 6);
		Val |= (long)((m_bSost[4+8+2]&0x3F) << 12);
		Val |= (long)((m_bSost[4+8+3]&0x3F) << 18);
	}
	return Val;
}

long	CSostSens::ValCmd() const
{
	long	Val = 0;

	if(LvlAsk10() > 1)
	{
		Val |= (long) (m_bSost[4+8+4+0]&0x3F);
		Val |= (long)((m_bSost[4+8+4+1]&0x3F) << 6);
		Val |= (long)((m_bSost[4+8+4+2]&0x3F) << 12);
		Val |= (long)((m_bSost[4+8+4+3]&0x3F) << 18);
	}
	return Val;
}

bool	CSostSens::chngValAsk(CSostSens& aNewSost) const
{
	bool	rc = false;

	if(LvlAsk10() < 2)
	{
		rc = aNewSost.LvlAsk10() >= 2;
	}
	else
	{
		if(aNewSost.LvlAsk10() >= 2)
		{
			rc = ValAsk() != aNewSost.ValAsk();
		}
	}
	return rc;
}

bool	CSostSens::chngValCmd(CSostSens& aNewSost) const
{
	bool	rc = false;

	if(LvlAsk10() < 2)
	{
		rc = aNewSost.LvlAsk10() >= 2;
	}
	else
	{
		if(aNewSost.LvlAsk10() >= 2)
		{
			rc = ValCmd() != aNewSost.ValCmd();
		}
	}
	return rc;
}
