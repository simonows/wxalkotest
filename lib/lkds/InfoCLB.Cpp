#include "stdafx.h"

#include <InfoCLB.h>

/////////////////////////////////////////////////////////////////////////////
// класс информации КЛШ о блоке 

void CInfoCLB::Clear()					
{ 
	m_Len	= 0;
	memset(m_bSost,0,mxLen);
}

bool CInfoCLB::tstBit(int aBit) const
{
	if(aBit >= 0)
	{
		int	nByte = aBit / 6;
		int nBit  = 1 << aBit % 6;

		if(nByte < m_Len)
		{
			return((m_bSost[nByte] & nBit) != 0);
		}
	}
	return(false);
}

void CInfoCLB::setBit(int aBit)
{
	if(aBit >= 0)
	{
		int	nByte = aBit / 6;
		int nBit  = 1 << aBit % 6;

		if(nByte < mxLen)
		{
			m_bSost[nByte] |= nBit;
		}
	}
}

void CInfoCLB::clrBit(int aBit)
{
	if(aBit >= 0)
	{
		int	nByte = aBit / 6;
		int nBit  = 1 << aBit % 6;

		if(nByte < mxLen)
		{
			m_bSost[nByte] &= ~nBit;
		}
	}
}

void CInfoCLB::operator=(const CInfoCLB& sl)
{
	m_Len	= sl.m_Len;
	memcpy(m_bSost, sl.m_bSost, mxLen);
}

BYTE CInfoCLB::operator[](int nIndex) const
{
	if(nIndex >= 0 && nIndex < mxLen)	return(m_bSost[nIndex]);
	else								return 0;
}

void	CInfoCLB::SetMove()
{
	BYTE	Data[2] = {0x01, 0x00};

	SetData(Data, sizeof(Data));
}

bool CInfoCLB::SetData(const BYTE* aData, int aLen)
{
	Clear();

	if(aLen > 0 && aLen <= mxLen)
	{
		m_Len = aLen;
		memcpy(m_bSost, aData, m_Len);
		return true;
	}
	return false;
}

int	CInfoCLB::GetData(BYTE* aData, int aMaxLen) const
{
	int	len = 0;

	if(aMaxLen >= Len())
	{
		memcpy(aData, m_bSost, Len());
		len	+= Len();
	}
	return len;
}

CAtlString CInfoCLB::BinStr(void)	const
{
	CAtlString	buf;

	for(int i = 0; i < m_Len; i++)
	{
		buf += (m_bSost[i] & 0x20)? '1': '0';
		buf += (m_bSost[i] & 0x10)? '1': '0';
		buf += (m_bSost[i] & 0x08)? '1': '0';
		buf += (m_bSost[i] & 0x04)? '1': '0';
		buf += (m_bSost[i] & 0x02)? '1': '0';
		buf += (m_bSost[i] & 0x01)? '1': '0';

		buf += ' ';
	}

	buf.Trim();

	return(buf);
}

CAtlString	CInfoCLB::HexStr()	const
{
	CAtlString	buf;

	for(int i = 0; i < m_Len; i++)
		buf.AppendFormat(_T(" %02X"), m_bSost[i]&0xFF);

	buf.Trim();

	return(buf);
}

bool CInfoCLB::NewError(const CInfoCLB& sNew) const
{
	return	(!tstBit(b5NoInform) && sNew.tstBit(b5NoInform)) ||
			(!tstBit(b5NoLink)   && sNew.tstBit(b5NoLink));
		   
}

bool CInfoCLB::EQ(const CInfoCLB& sl)	const
{
	return(	(tstBit(b5NoInform) == sl.tstBit(b5NoInform)) && 
			(tstBit(b5NoLink)   == sl.tstBit(b5NoLink)));
}

bool CInfoCLB::operator  == (const CInfoCLB& sl) const
{
	if(m_Len == sl.m_Len)
	{
		return m_Len? memcmp(m_bSost, sl.m_bSost, m_Len) == 0: true;
	}
	return false;
}
