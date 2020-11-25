#include "stdafx.h"

#include "Pack.h"

static const BYTE Step3[256]= {
   0x00, 0x43, 0x86, 0xC5, 0x4F, 0x0C, 0xC9, 0x8A,
   0x9E, 0xDD, 0x18, 0x5B, 0xD1, 0x92, 0x57, 0x14,
   0x7F, 0x3C, 0xF9, 0xBA, 0x30, 0x73, 0xB6, 0xF5,
   0xE1, 0xA2, 0x67, 0x24, 0xAE, 0xED, 0x28, 0x6B,
   0xFE, 0xBD, 0x78, 0x3B, 0xB1, 0xF2, 0x37, 0x74,
   0x60, 0x23, 0xE6, 0xA5, 0x2F, 0x6C, 0xA9, 0xEA,
   0x81, 0xC2, 0x07, 0x44, 0xCE, 0x8D, 0x48, 0x0B,
   0x1F, 0x5C, 0x99, 0xDA, 0x50, 0x13, 0xD6, 0x95,
   0xBF, 0xFC, 0x39, 0x7A, 0xF0, 0xB3, 0x76, 0x35,
   0x21, 0x62, 0xA7, 0xE4, 0x6E, 0x2D, 0xE8, 0xAB,
   0xC0, 0x83, 0x46, 0x05, 0x8F, 0xCC, 0x09, 0x4A,
   0x5E, 0x1D, 0xD8, 0x9B, 0x11, 0x52, 0x97, 0xD4,
   0x41, 0x02, 0xC7, 0x84, 0x0E, 0x4D, 0x88, 0xCB,
   0xDF, 0x9C, 0x59, 0x1A, 0x90, 0xD3, 0x16, 0x55,
   0x3E, 0x7D, 0xB8, 0xFB, 0x71, 0x32, 0xF7, 0xB4,
   0xA0, 0xE3, 0x26, 0x65, 0xEF, 0xAC, 0x69, 0x2A,
   0x3D, 0x7E, 0xBB, 0xF8, 0x72, 0x31, 0xF4, 0xB7,
   0xA3, 0xE0, 0x25, 0x66, 0xEC, 0xAF, 0x6A, 0x29,
   0x42, 0x01, 0xC4, 0x87, 0x0D, 0x4E, 0x8B, 0xC8,
   0xDC, 0x9F, 0x5A, 0x19, 0x93, 0xD0, 0x15, 0x56,
   0xC3, 0x80, 0x45, 0x06, 0x8C, 0xCF, 0x0A, 0x49,
   0x5D, 0x1E, 0xDB, 0x98, 0x12, 0x51, 0x94, 0xD7,
   0xBC, 0xFF, 0x3A, 0x79, 0xF3, 0xB0, 0x75, 0x36,
   0x22, 0x61, 0xA4, 0xE7, 0x6D, 0x2E, 0xEB, 0xA8,
   0x82, 0xC1, 0x04, 0x47, 0xCD, 0x8E, 0x4B, 0x08,
   0x1C, 0x5F, 0x9A, 0xD9, 0x53, 0x10, 0xD5, 0x96,
   0xFD, 0xBE, 0x7B, 0x38, 0xB2, 0xF1, 0x34, 0x77,
   0x63, 0x20, 0xE5, 0xA6, 0x2C, 0x6F, 0xAA, 0xE9,
   0x7C, 0x3F, 0xFA, 0xB9, 0x33, 0x70, 0xB5, 0xF6,
   0xE2, 0xA1, 0x64, 0x27, 0xAD, 0xEE, 0x2B, 0x68,
   0x03, 0x40, 0x85, 0xC6, 0x4C, 0x0F, 0xCA, 0x89,
   0x9D, 0xDE, 0x1B, 0x58, 0xD2, 0x91, 0x54, 0x17   };

BYTE CPackMes::CalcCrc (const BYTE* mes, int len)
{
	BYTE	crc = 0;

	for(int i = 0; i < len; i++)
		crc = Step3[(crc ^ mes[i]) & 0xFF];

	return crc;
}

/////////////////////////////////////////////////////////////////////////////
// пакет сообщений

CPackMes::CPackMes()
{
	memset(m_bBuf, 0, mxLen);

	Clear();
}

CPackMes::CPackMes(int len, const BYTE* buf, int aPrior)
{
	m_InsTime.SetStatus(COleDateTime::null);
	m_Prior		= aPrior;
	m_iLen		= len<mxLen? len: mxLen;
	m_iIndex	= 0;
	m_iIdent	= 0;
	m_bIdent	= false;
	m_iReply	= 0;
	m_SubNet	= 0;

	memmove(m_bBuf,	buf, m_iLen);
	m_bBuf[m_iLen] = CalcCrc(m_bBuf, m_iLen) & 0x7F;

	UnPack();
}

CPackMes::CPackMes(int aOrig, int aDest, int aPrior)
{
	m_InsTime.SetStatus(COleDateTime::null);
	m_Prior		= aPrior;
	m_iLen		= 0;
	m_iIndex	= 0;
	m_bBuf[m_iLen++]	= ((aOrig << 4) & 0xF0) | (aDest & 0xF);

	m_iIdent	= 0;
	m_bIdent	= false;
	m_iReply	= 0;
	m_SubNet	= 0;

	m_bBuf[m_iLen] = CalcCrc(m_bBuf, m_iLen) & 0x7F;
}

void CPackMes::SetIdent(int aIdent)
{
	if(m_iLen >= 1)
	{
		if(m_iLen > 1)
		{
			if((m_bBuf[1] & 0xC0) != 0x80)
			{
				memmove(&m_bBuf[2], &m_bBuf[1], m_iLen-1);
				m_iLen++;
			}
		}
		else
		{
			m_iLen++;
		}
		m_bBuf[1]	= (aIdent & 0x3F) | 0x80;
		m_iIdent	= aIdent;
		m_bIdent	= true;
		m_bBuf[m_iLen] = CalcCrc(m_bBuf, m_iLen) & 0x7F;
	}
}

void CPackMes::operator=(const CPackMes& pk)
{
	m_InsTime	= pk.m_InsTime;
	m_Prior		= pk.m_Prior;
	m_iIndex	= 0;
	m_iLen		= pk.m_iLen;
	m_iReply	= pk.m_iReply;
	m_iIdent	= pk.m_iIdent;
	m_bIdent	= pk.m_bIdent;
	m_SubNet	= pk.m_SubNet;
	memmove(m_bBuf,		pk.m_bBuf,	mxLen);
	memmove(m_bReply,	pk.m_bReply,mxLen);
}

void CPackMes::UnPack()
{
	m_iIndex	= 1;
	m_iIdent	= 0;
	m_bIdent	= false;
	m_iReply	= 0;

	if((m_bBuf[m_iIndex] & 0xC0) == 0x80)
	{
		m_iIdent	= m_bBuf[m_iIndex] & 0x3F;
		m_bIdent	= true;
		m_iIndex++;
	}

	for(int i = m_iIndex; i < m_iLen; i++)
	{
		if((m_bBuf[i] & 0xC0) == 0x40)
		{
			m_bReply[m_iReply++] = m_bBuf[i] & 0x3F;
			memmove(&m_bBuf[i], &m_bBuf[i+1], m_iLen-i-1);
			m_iLen--;
			i--;
		}
	}
}

bool CPackMes::IsValid()
{
	int Index =	m_iIndex;

	while(Index < m_iLen)
	{
		int LenMes = m_bBuf[Index];

		if( LenMes+Index <= m_iLen && LenMes > 5 && LenMes < 60)
		{
			Index += LenMes;
		}
		else
		{
			break;
		}
	}
	return(Index==m_iLen);
}

bool	CPackMes::IsEmpty()
{
	return( m_iLen <= 1);
}

void CPackMes::Clear()
{
	m_iIdent	= 0;
	m_bIdent	= false;
	m_iReply	= 0;

	m_iLen		= 1;
	m_iIndex	= 0;
	m_InsTime.SetStatus(COleDateTime::null);
	m_Prior		= 255;
	m_SubNet	= 0;

	m_bBuf[m_iLen] = CalcCrc(m_bBuf, m_iLen) & 0x7F;
}

void CPackMes::SetAddr(int aOrig, int aDest)
{
	m_bBuf[0] = ((aOrig << 4) & 0xF0) | (aDest & 0xF);

	Clear();
}

void	CPackMes::SetDest(int aDest)
{
	m_bBuf[0] &= ~0xF;
	m_bBuf[0] |= aDest&0xF;
}

void CPackMes::AddIdent(int aIdent)
{
	m_bBuf[m_iLen++] = (aIdent & 0x3F) | 0x80;
}

bool CPackMes::AddRepl(BYTE aRepl)
{
	//if(m_iLen < mxLen ) 4.7.0.4 (13.01.2014)
	if(m_iLen < (mxLen-1) )
	{
		if(m_iLen > 1 && (m_bBuf[m_iLen-1] & 0xC0) == 0x40) m_bBuf[m_iLen-1] = (aRepl & 0x3F) | 0x40;
		else												m_bBuf[m_iLen++] = (aRepl & 0x3F) | 0x40;

		CalcCrc();
		return true;
	}
	else
	{
		if(m_iLen > 1 && (m_bBuf[m_iLen-1] & 0xC0) == 0x40) 
		{
			m_bBuf[m_iLen-1] = (aRepl & 0x3F) | 0x40;

			CalcCrc();
			return true;
		}
		return false;
	}
}

int	CPackMes::GetOrig()
{
	if(m_iLen > 0)	return((m_bBuf[0] >> 4) & 0xF);
	else			return(0);
}

int	CPackMes::GetDest()
{
	if(m_iLen > 0)	return(m_bBuf[0] & 0xF);
	else			return(0);
}

bool CPackMes::GetMes(CMes& aMes)
{
	aMes.Clear();

	if(m_iIndex < m_iLen)
	{
		int	LenMes = m_bBuf[m_iIndex];

		if( LenMes+m_iIndex <= m_iLen && LenMes > 5 && LenMes < 60)
		{
			aMes.SetDann(LenMes-1, &m_bBuf[m_iIndex+1]);
			aMes.m_Prior	= m_Prior;
			aMes.m_SubNet	= m_SubNet;
			m_iIndex += LenMes;
			return true;
		}
	}
	aMes.Clear();
	return false;
}

void	CPackMes::SetOrigMes(int aUM, int aSlot)
{
	BegGet();

	if(GetOrig() == 0xF)
	{
		m_bBuf[0] &= ~0xF0;
		m_bBuf[0] |= (aSlot&0xF) << 4;
	}
	while(m_iIndex < m_iLen)
	{
		int	LenMes = m_bBuf[m_iIndex];

		if( LenMes+m_iIndex <= m_iLen && LenMes > 5 && LenMes < 60)
		{
			int	UMOrig		= 0;
			int	SlotOrig	= 0;

			CMes::GetAdr(&m_bBuf[m_iIndex+1], UMOrig, SlotOrig);
			if(UMOrig==0xFF && SlotOrig==0xF) 
			{
				CMes::SetAdr(&m_bBuf[m_iIndex+1], aUM, aSlot);

				if(	m_bBuf[m_iIndex+6] == CMes::tmAskServ &&	// подписка
					m_bBuf[m_iIndex+7] == CMes::tsREQ)
				{
					CMes::GetAdr(&m_bBuf[m_iIndex+8], UMOrig, SlotOrig);

					if(UMOrig==0xFF && SlotOrig==0xF) 
					{
						CMes::SetAdr(&m_bBuf[m_iIndex+8], aUM, aSlot);
					}
				}
			}
			m_iIndex += LenMes;
		}
		else
		{
			break;
		}
	}
}

bool CPackMes::AddMes(const CMes& aMes)
{
	//if(m_iLen + aMes.Len() < (mxLen-2)) 4.7.0.4 (13.01.2014)
	if(m_iLen + aMes.Len() < (mxLen-3))
	{
		m_bBuf[m_iLen++] = aMes.Len()+1;
		memcpy(&m_bBuf[m_iLen], aMes, aMes.Len());
		m_iLen += aMes.Len();
		m_bBuf[m_iLen] = CalcCrc(m_bBuf, m_iLen) & 0x7F;
		return(true);
	}
	return(false);
}

void CPackMes::CalcCrc()
{
	m_bBuf[m_iLen] = CalcCrc(m_bBuf, m_iLen) & 0x7F;
}

bool CPackMes::IsNoMes()
{
	return( m_iLen <= 5);
}

void CPackMes::DelIdent()
{
	if(m_iLen > 1)
	{
		if((m_bBuf[1] & 0xC0) == 0x80)
		{
			memmove(&m_bBuf[1], &m_bBuf[2], m_iLen-2);

			m_iLen--;
			m_iIdent	= 0;
			m_bIdent	= false;

			BegGet();
			CalcCrc();
		}
	}
}

bool CPackMes::Link(CPackMes& pk)
{
	CMes		Mes;
	bool		existLink	= false;
	int			iIdent		= m_iIdent;
	bool		bIdent		= m_bIdent;
	int			pk_iIdent	= pk.m_iIdent;
	bool		pk_bIdent	= pk.m_bIdent;
	CPackMes	ostPack(pk.GetOrig(),	pk.GetDest());

	pk.BegGet();
	while(pk.GetMes(Mes))
	{
		if(AddMes(Mes)) 
		{
			existLink = true;
		}
		else
		{
			do
			{
				if(!ostPack.AddMes(Mes))
				{
					if(bIdent)	SetIdent(iIdent);
					else		DelIdent();

					return(false);
				}
			}
			while(pk.GetMes(Mes));

			break;
		}
	}

	pk	= ostPack;

	if(bIdent)	SetIdent(iIdent);
	else		DelIdent();

	if(pk_bIdent)	pk.SetIdent(pk_iIdent);
	else			pk.DelIdent();
	
	return(existLink);
}

void CPackMes::BegGet()
{
	m_iIndex	= 1;
	m_iIdent	= 0;	
	m_bIdent	= false;

	if((m_bBuf[m_iIndex] & 0xC0) == 0x80)
	{
		m_iIdent	= m_bBuf[m_iIndex] & 0x3F;
		m_bIdent	= true;
		m_iIndex++;
	}
}

void	CPackMes::SetPrior(int aPrior)
{
	m_Prior	= aPrior;
}

CAtlString	CPackMes::Str(const TCHAR* aTitle)
{
	CString	buf(aTitle? aTitle: _T(""));
	
	for(int i = 0; i < m_iLen; i++)
	{
		buf.AppendFormat(_T(" %02X"), m_bBuf[i]&0xFF);
	}
	return buf;
}
