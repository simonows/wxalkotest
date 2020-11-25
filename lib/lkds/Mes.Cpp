#include "stdafx.h"

#include <Mes.h>
#include "LoadRes.h"
#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// сообщение
//

CMes::CMes()
{
	Clear();
}

CMes::CMes(const CMes& aMes)
{
	m_iLenMes		= aMes.m_iLenMes;
	memcpy(m_bMes, aMes.m_bMes, sizeof(m_bMes));

	m_SubNet		= aMes.m_SubNet;
	m_SlGate		= aMes.m_SlGate;
	m_Flags			= aMes.m_Flags;
	m_Prior			= aMes.m_Prior;
	m_iUmOrig		= aMes.m_iUmOrig;
	m_iSlOrig		= aMes.m_iSlOrig;
	m_iUmDest		= aMes.m_iUmDest;
	m_iSlDest		= aMes.m_iSlDest;
	m_iHopCount		= aMes.m_iHopCount;
	m_iDev			= aMes.m_iDev;
	m_iTypMes		= aMes.m_iTypMes;
	m_TypPrefix		= aMes.m_TypPrefix;
	m_iAsk			= aMes.m_iAsk;
	m_TypMsgDisp	= aMes.m_TypMsgDisp;
	m_TypAlert		= aMes.m_TypAlert;
	m_Sost			= aMes.m_Sost;
}

CMes::CMes(int aLen, const BYTE* aBody)
{
	SetDann(aLen, aBody);
}

bool CMes::SetDann(int aLen, const BYTE* aBody)
{
	Clear();

	if(aLen <= mxLen)
	{
		m_iLenMes	= aLen;
		memcpy(m_bMes, aBody, aLen);
		UnPack();

		return true;
	}
	return false;
}

CMes::CMes(int aUmDest, int aSlDest, int aUmOrig, int aSlOrig, typMes aTypMes, BYTE aMask)
{
	Clear();

	m_iUmOrig		= aUmOrig;
	m_iSlOrig		= aSlOrig;
	m_iUmDest		= aUmDest;
	m_iSlDest		= aSlDest;
	m_iTypMes		= aTypMes;
	m_iDev			= 0;
	m_iAsk			= amNone;
	m_TypMsgDisp	= tdNone;
	m_TypAlert		= taNone;

	memset(m_bMes,  0, mxLen);
	SetAdr(&m_bMes[0], m_iUmOrig, m_iSlOrig);

	SetAdr(&m_bMes[2], m_iUmDest, m_iSlDest);

	m_bMes[5]	= m_iTypMes;
	m_bMes[6]	= aMask;

	m_iLenMes = 7; 
}

CMes::CMes(int aUmDest, int aSlDest, int aUmOrig, int aSlOrig, typMes aTypMes, const BYTE* aMask, int aLen)
{
	Clear();

	m_iUmOrig		= aUmOrig;
	m_iSlOrig		= aSlOrig;
	m_iUmDest		= aUmDest;
	m_iSlDest		= aSlDest;
	m_iTypMes		= aTypMes;
	m_iDev			= 0;
	m_iAsk			= amNone;
	m_TypMsgDisp	= tdNone;
	m_TypAlert		= taNone;

	memset(m_bMes, 0, mxLen);
	SetAdr(&m_bMes[0], m_iUmOrig, m_iSlOrig);

	SetAdr(&m_bMes[2], m_iUmDest, m_iSlDest);

	m_bMes[5]	= m_iTypMes;

	memcpy(&m_bMes[6], aMask, aLen);

	m_iLenMes = 6+aLen;
}

CMes::CMes(int aUmDest, int aSlDest, int aUmOrig, int aSlOrig, int aDriv, typMes aTypMes, askFromMain aAsk, const BYTE* aMask, int aLen, int aDuration)
{
	Clear();

	m_iUmOrig		= aUmOrig;
	m_iSlOrig		= aSlOrig;
	m_iUmDest		= aUmDest;
	m_iSlDest		= aSlDest;
	m_iTypMes		= aTypMes;
	m_iDev			= aDriv;
	m_iAsk			= aAsk;
	m_TypMsgDisp	= tdNone;
	m_TypAlert		= taNone;

	memset(m_bMes, 0, mxLen);
	SetAdr(&m_bMes[0], m_iUmOrig, m_iSlOrig);

	SetAdr(&m_bMes[2], m_iUmDest, m_iSlDest);

	m_bMes[5]	= m_iTypMes;

	SetAdr(&m_bMes[6], m_iDev, m_iAsk);

	if(m_iAsk > 0xF) m_bMes[6] |= 0x20;

	m_iLenMes	= 8;

	if(aMask != NULL && m_iLenMes+aLen < mxLen)
	{
		memcpy(&m_bMes[8], aMask, aLen);

		m_iLenMes += aLen;
	}

	if(aDuration >= 0 && aDuration <= 255)
	{
		SetAdr(&m_bMes[m_iLenMes], aDuration, 0);

		m_iLenMes += 2;
	}
}

CMes::CMes(int aUmDest, int aSlDest, int aUmOrig, int aSlOrig, typAlert aTypAlert, const BYTE* aMask, int aLen)
{
	int	LenAlert;
	int	Alert	= aTypAlert;

	Clear();

	m_iUmOrig		= aUmOrig;
	m_iSlOrig		= aSlOrig;
	m_iUmDest		= aUmDest;
	m_iSlDest		= aSlDest;

	SetAdr(&m_bMes[0], m_iUmOrig, m_iSlOrig);
	SetAdr(&m_bMes[2], m_iUmDest, m_iSlDest);

	m_bMes[5]	= tmSostDisp;
	m_bMes[6]	= tdAlert;

	for(LenAlert = 6; LenAlert > 0; LenAlert--)
	{
		if( ( (Alert >> (6*(LenAlert-1)) ) & 0x3F ) != 0) break;
	}
	for(int i = 0; i < LenAlert; i++)
	{
		m_bMes[i+7]	= (Alert >> (6*i)) & 0x3F;
	}

	int	Len6	= (aLen/3)*4;

	switch(aLen%3)
	{
	case 1:	Len6 += 2; break;
	case 2:	Len6 += 3; break;
	};

	if(aMask && Len6 > 0 && Len6 < mxLen-(7+6))
	{
		m_iLenMes	= 7 + 6; 
		Conv8To6(aLen, aMask, &m_bMes[m_iLenMes], Len6);
		m_iLenMes	+= Len6;
	}
	else
	{
		m_iLenMes = 7 + LenAlert; 
	}
}

CMes::CMes(int aUmDest, int aSlDest, int aUmOrig, int aSlOrig, jobNVRAM aJobNVRAM, long aAddr, int aLen, const BYTE* aBody)
{
	Clear();

	m_iUmOrig		= aUmOrig;
	m_iSlOrig		= aSlOrig;
	m_iUmDest		= aUmDest;
	m_iSlDest		= aSlDest;

	SetAdr(&m_bMes[0], m_iUmOrig, m_iSlOrig);
	SetAdr(&m_bMes[2], m_iUmDest, m_iSlDest);

	m_bMes[5]	= tmGetNVREx;
	m_bMes[6]	= (BYTE)(aAddr & 0x3F);
	m_bMes[7]	= (BYTE)((aAddr>>6) & 0x3F);
	m_bMes[8]	= aLen & 0x3F;
	m_bMes[9]	= aJobNVRAM;

	m_iLenMes	= 10;

	if(aJobNVRAM == jnSet && aBody != NULL)
	{
//		if(aLen >= 0 && aLen <= 30 && (aLen % 3) == 0)
		if(aLen >= 0 && aLen <= 33)
		{
			int tetrad	= aLen/3;

			if( (aLen%3) != 0 ) tetrad++;

			for(int i = 0; i < tetrad; i++)
			{
				m_bMes[m_iLenMes++]	= (aBody[i*3+0] & 0x3F);
				m_bMes[m_iLenMes++]	= ((aBody[i*3+0] >> 6) & 0x3) | (((aBody[i*3+1] << 2) & 0x3C));
				m_bMes[m_iLenMes++]	= ((aBody[i*3+1] >> 4) & 0xF) | (((aBody[i*3+2] << 4) & 0x30));
				m_bMes[m_iLenMes++]	= ((aBody[i*3+2] >> 2) & 0x3F);
			}
		}
	}
}

CMes::CMes(int aUmDest, int aSlDest, int aUmOrig, int aSlOrig, rezNVRAM aRezNVRAM, long aAddr, int aLen, const BYTE* aBody)
{
	Clear();

	m_iUmOrig		= aUmOrig;
	m_iSlOrig		= aSlOrig;
	m_iUmDest		= aUmDest;
	m_iSlDest		= aSlDest;

	SetAdr(&m_bMes[0], m_iUmOrig, m_iSlOrig);
	SetAdr(&m_bMes[2], m_iUmDest, m_iSlDest);

	m_bMes[5]	= tmConfNVREx;
	m_bMes[6]	= (BYTE)(aAddr & 0x3F);
	m_bMes[7]	= (BYTE)((aAddr>>6) & 0x3F);
	m_bMes[8]	= aLen & 0x3F;
	m_bMes[9]	= aRezNVRAM;

	m_iLenMes	= 10;

	if(aRezNVRAM == rnGood && aBody != NULL)
	{
		if(aLen >= 0 && aLen <= 33)
		{
			int tetrad	= aLen/3;

			if( (aLen%3) != 0 ) tetrad++;

			for(int i = 0; i < tetrad; i++)
			{
				m_bMes[m_iLenMes++]	= (aBody[i*3+0] & 0x3F);
				m_bMes[m_iLenMes++]	= ((aBody[i*3+0] >> 6) & 0x3) | (((aBody[i*3+1] << 2) & 0x3C));
				m_bMes[m_iLenMes++]	= ((aBody[i*3+1] >> 4) & 0xF) | (((aBody[i*3+2] << 4) & 0x30));
				m_bMes[m_iLenMes++]	= ((aBody[i*3+2] >> 2) & 0x3F);
			}
		}
	}
}

CMes::CMes(int aUmDest, int aSlDest, int aUmOrig, int aSlOrig, typService aTypSrv, const BYTE* aBody, int aLen)
{
	Clear();

	m_iUmOrig		= aUmOrig;
	m_iSlOrig		= aSlOrig;
	m_iUmDest		= aUmDest;
	m_iSlDest		= aSlDest;

	SetAdr(&m_bMes[0], m_iUmOrig, m_iSlOrig);
	SetAdr(&m_bMes[2], m_iUmDest, m_iSlDest);

	m_bMes[5]	= tmAskServ;
	m_bMes[6]	= aTypSrv;

	memcpy(&m_bMes[7], aBody, aLen);

	m_iLenMes	= aLen+7;
}

CMes::CMes(int aUmDest, int aSlDest, int aUmOrig, int aSlOrig, typPrefix aTypPrefix, BYTE aAddr)
{
	Clear();

	m_iUmOrig	= aUmOrig;
	m_iSlOrig	= aSlOrig;
	m_iUmDest	= aUmDest;
	m_iSlDest	= aSlDest;
	m_iDev		= aAddr;
	m_iTypMes	= tmGetSostV7;
	m_TypPrefix	= aTypPrefix;

	SetAdr(&m_bMes[0], m_iUmOrig, m_iSlOrig);
	SetAdr(&m_bMes[2], m_iUmDest, m_iSlDest);

	m_bMes[5]	= m_iTypMes;
	m_bMes[6]	= m_TypPrefix;
	m_bMes[7]	= m_iDev;

	m_iLenMes	= 8;
}			

CMes::CMes(int aUmDest, int aSlDest, int aUmOrig, int aSlOrig, typPrefix aTypPrefix, BYTE aAddr, askFromMain aAsk, int aLen, const BYTE* aBody)
{
	Clear();

	m_iUmOrig	= aUmOrig;
	m_iSlOrig	= aSlOrig;
	m_iUmDest	= aUmDest;
	m_iSlDest	= aSlDest;
	m_iDev		= aAddr;
	m_iTypMes	= tmGetSostDV7;
	m_TypPrefix	= aTypPrefix;
	m_iAsk		= aAsk;

	SetAdr(&m_bMes[0], m_iUmOrig, m_iSlOrig);
	SetAdr(&m_bMes[2], m_iUmDest, m_iSlDest);

	m_bMes[5]	= m_iTypMes;
	m_bMes[6]	= m_TypPrefix;
	m_bMes[7]	= m_iDev;
	m_bMes[8]	= m_iAsk;

	memcpy(&m_bMes[9], aBody, aLen);

	m_iLenMes	= aLen+9;
}

CMes::CMes(int aUmDest, int aSlDest, int aUmOrig, int aSlOrig, int aDriv, subServ18 aSubServ18, const BYTE* aBody, int aLen)
{
	m_SubNet		= 0;
	m_iUmOrig		= aUmOrig;
	m_iSlOrig		= aSlOrig;
	m_iUmDest		= aUmDest;
	m_iSlDest		= aSlDest;
	m_iTypMes		= tmGetSostD;
	m_iDev			= aDriv;
	m_iAsk			= amServ18;
	m_TypMsgDisp	= tdNone;
	m_TypAlert		= taNone;
	m_Sost.Clear();

	memset(m_bMes, 0, mxLen);
	SetAdr(&m_bMes[0], m_iUmOrig, m_iSlOrig);

	SetAdr(&m_bMes[2], m_iUmDest, m_iSlDest);

	m_bMes[5]	= m_iTypMes;
	SetAdr(&m_bMes[6], m_iDev, m_iAsk);
	if(m_iAsk > 0xF) m_bMes[6] |= 0x20;

	BYTE	Body8[60];

	memset(Body8, 0, sizeof(Body8));
	Body8[0]	= aSubServ18;
	memcpy(Body8+1, aBody, min(sizeof(Body8)-1, aLen));

	int	Len	= min(sizeof(Body8), aLen+1);
	
	//int Len6 = Conv8To6(Len, Body8, &m_bMes[8], mxLen-8); 19.01.2014

	int Len6	= Len/3;
	if( (Len%3) != 0 ) Len6++;
	Len6		*= 4;
	Conv8To6(Len, Body8, &m_bMes[8], mxLen-8);

	Len6	= max(4,Len6);

	m_iLenMes = 8+Len6;
}

void CMes::Clear()
{
	m_SubNet		= 0;
	m_Flags			= 0;
	m_SlGate		= 0;
	m_Prior			= 255;
	m_iLenMes		= 0;
	m_iUmOrig		= 0;
	m_iSlOrig		= 0;
	m_iUmDest		= 0;
	m_iSlDest		= 0;
	m_iHopCount		= 0;
	m_iDev			= 0;
	m_iTypMes		= tmNone;
	m_iAsk			= amNone;
	m_TypMsgDisp	= tdNone;
	m_TypAlert		= taNone;
	m_TypPrefix		= tpxNone;
	m_Sost.Clear();

	memset(m_bMes, 0, mxLen);
}

void CMes::SetOrig(int aUm, int aSlot)
{
	m_iUmOrig	= aUm;
	m_iSlOrig	= aSlot;

	SetAdr(&m_bMes[0], m_iUmOrig, m_iSlOrig);
}

void CMes::SetDest(int aUm, int aSlot)
{
	m_iUmDest	= aUm;
	m_iSlDest	= aSlot;

	SetAdr(&m_bMes[2], m_iUmDest, m_iSlDest);
}

void CMes::SetDev(int aDev)
{
	m_iDev	= aDev;

	SetAdr(&m_bMes[6], m_iDev, m_iAsk);

	if(m_iAsk > 0xF) m_bMes[6] |= 0x20;
}

void CMes::UnPack()
{
	m_iUmOrig		= 0;
	m_iSlOrig		= 0;
	m_iUmDest		= 0;
	m_iSlDest		= 0;
	m_iDev			= 0;
	m_iTypMes		= tmNone;
	m_iAsk			= amNone;
	m_TypMsgDisp	= tdNone;
	m_TypAlert		= taNone;
	m_TypPrefix		= tpxNone;

	GetAdr(&m_bMes[0], m_iUmOrig, m_iSlOrig);

	GetAdr(&m_bMes[2], m_iUmDest, m_iSlDest);

	m_iHopCount	= m_bMes[4];

	if(m_iLenMes > 5) m_iTypMes = (typMes)(m_bMes[5] & 0xFF);

	if(m_iTypMes == tmAskServ || m_iTypMes == tmAnswServ)
	{
		GetAdr(&m_bMes[6+5], m_iDev, (int&)m_iAsk);

		if( (m_iDev & 0x80) != 0)
		{
			m_iDev &= 0x7F;
			m_iAsk = (askFromMain)(m_iAsk | 0x10);
		}
	}
	else if(m_iTypMes == tmSostDisp)
	{
		if(m_iLenMes > 6)
		{
			m_TypMsgDisp = (typMsgDisp)m_bMes[6];
		}
		if(m_TypMsgDisp == tdAlert)
		{
			int	Alert	 = 0;
			int	LenAlert = m_iLenMes - 7;
			if(LenAlert > 6) LenAlert = 6;

			for(int i = 0; i < LenAlert; i++)
			{
				Alert |= m_bMes[i+7] << (6*i);
			}
			m_TypAlert = (typAlert)Alert;
		}
	}
	else if(m_iTypMes == tmSostDevV7	||
			m_iTypMes == tmSostDevDV7	||
			m_iTypMes == tmGetSostV7	||
			m_iTypMes == tmGetSostDV7)
	{
		if(m_iLenMes > 6)	m_TypPrefix	= static_cast<typPrefix>(m_bMes[6]);
		if(m_iLenMes > 7)	m_iDev		= m_bMes[7];
		if(m_iLenMes > 8)	m_iAsk		= static_cast<askFromMain>(m_bMes[8]&0x1F);
	}
	else
	{
		if(m_iLenMes > 7)
		{
			GetAdr(&m_bMes[6], m_iDev, (int&)m_iAsk);

			if( m_iTypMes == tmSostUnitD || m_iTypMes == tmGetSostD)
			{
				if( (m_iDev & 0x80) != 0)
				{
					m_iDev &= 0x7F;
					m_iAsk = (askFromMain)(m_iAsk | 0x10);
				}
			} 
		}
		if(m_iLenMes > 8)
		{
			if(m_iTypMes == tmSostUnit || m_iTypMes == tmSostUnitD)
			{
				m_Sost.SetData(&m_bMes[8], m_iLenMes-8);

				if( m_iTypMes == tmSostUnit) m_Sost.Normaliz();
			}
		}
	}
}

int	CMes::IncHopCount()
{
	m_iHopCount++;

	m_bMes[4]	= m_iHopCount;

	return(m_iHopCount);
}

bool CMes::IsGood()
{
	if(	m_iTypMes == tmSostUnitD ||
		m_iTypMes == tmSostDevDV7)
	{
		return( (m_bMes[m_iLenMes-1] & 0x20) == 0);
	}
	return true;
}

void CMes::SetAdr(BYTE* aAdr, int aUM, int aSlot)
{
	aAdr[0]	=  (aUM >> 2) & 0x3F;
	aAdr[1]	= ((aUM << 4) & 0x30) | (aSlot & 0xF);
}

void CMes::GetAdr(const BYTE *aAdr, int& aUM, int& aSlot)
{
	aUM   = ((aAdr[0]<<2) & 0xFC) | ((aAdr[1]>>4) & 0x3);
	aSlot = aAdr[1] & 0xF;
}

bool CMes::GetNVRAM(int& aAdr, unsigned short& aValue)
{
	aAdr	= 0;
	aValue	= 0;

	if(m_iTypMes == tmConfNVR)
	{
		aAdr	=	m_bMes[6+0]&0x3F;

		aValue	=	(   ((m_bMes[6+3]&0x3C) << 2) | ((m_bMes[6+2]&0x3C) >> 2) ) | 
					( ( ((m_bMes[6+2]&0x03) << 6) |  (m_bMes[6+1]&0x3F) ) << 8 );

		return true;
	}
	return false;
}

CMes::rezNVRAM	CMes::GetNVRAM(long& aAdrNVRAM, int& aLen, BYTE* aBuf, int aMaxLen)
{
	aAdrNVRAM	= 0;
	aLen		= 0;

	if(m_iTypMes == tmConfNVREx || m_iTypMes == tmGetNVREx)
	{
		if(m_bMes[9] == rnGood || m_iTypMes == tmGetNVREx)
		{
			aAdrNVRAM	= (m_bMes[6] & 0x3F) | ((m_bMes[7]<<6) & 0x0FC0);
			aLen		= m_bMes[8] & 0x3F;

			int		index	= 0;
			int		Count4	= (m_iLenMes - 10) /4;

			if( ((m_iLenMes - 10)%4) != 0) Count4++;

			memset(aBuf, 0, aMaxLen);

			for(int i = 0; i < Count4; i++)
			{
				if(index >= aMaxLen) break;
				aBuf[index++] =  (m_bMes[10+i*4+0]&0x3F)       | ((m_bMes[10+i*4+1]&0x3)  << 6);
				if(index >= aMaxLen) break;
				aBuf[index++] = ((m_bMes[10+i*4+1]&0x3C) >> 2) | ((m_bMes[10+i*4+2]&0xF)  << 4);
				if(index >= aMaxLen) break;
				aBuf[index++] = ((m_bMes[10+i*4+2]&0x30) >> 4) | ((m_bMes[10+i*4+3]&0x3F) << 2);
			}
		}
		return((CMes::rezNVRAM)m_bMes[9]);
	}
	else
	{
		return(rnErrMes);
	}
}

bool	CMes::GetLbNVRAM(int& aAddr, unsigned short& aVal, bool& aIsWord)
{

	if(m_iLenMes >= 12)
	{
		if(((m_bMes[6+3])&0x2) == 0)
		{	// режим слово
			aAddr=	((m_bMes[6+2])&0x3F) | (((m_bMes[6+3])&0x1) << 6 );

			aVal=	(   (((m_bMes[6+3])&0x3C) >> 2) | (((m_bMes[6+4])&0x0F) << 4) ) | 
					( ( (((m_bMes[6+4])&0x30) >> 4) |  ((m_bMes[6+5])&0x3F) << 2) << 8 );

			aIsWord = true;
		}
		else
		{	// режим байт

			aAddr=	( (m_bMes[6+2])&0x3F)			| 
				    (((m_bMes[6+3])&0x1)  <<  6 )	|
					(((m_bMes[6+3])&0x3C) <<  7 )	|
					(((m_bMes[6+4])&0x3)  << 11 );

			aVal=	(((m_bMes[6+4])&0x30) >>  4 )	| 
					(((m_bMes[6+5])&0x3F) <<  2 );

			aIsWord = false;
		}
		return true;
	}
	else
	{
		aAddr	= 0;
		aVal	= 0;
		aIsWord = true;

		return false;
	}
}

int		CMes::GetIAP(BYTE* aBuf, int aMaxLen)
{
	int	index	= 0;
	int	perf	= m_iTypMes==tmSostUnitD?12:11;
	int	Count4	= (m_iLenMes - perf) /4;

	if( ((m_iLenMes - perf)%4) != 0) Count4++;

	memset(aBuf, 0, aMaxLen);

	for(int i = 0; i < Count4; i++)
	{
		if(index >= aMaxLen) break;
		aBuf[index++] =  (m_bMes[perf+i*4+0]&0x3F)       | ((m_bMes[perf+i*4+1]&0x3)  << 6);
		if(index >= aMaxLen) break;
		aBuf[index++] = ((m_bMes[perf+i*4+1]&0x3C) >> 2) | ((m_bMes[perf+i*4+2]&0xF)  << 4);
		if(index >= aMaxLen) break;
		aBuf[index++] = ((m_bMes[perf+i*4+2]&0x30) >> 4) | ((m_bMes[perf+i*4+3]&0x3F) << 2);
	}

	return(index);
}

int		CMes::GetAlert(void* aBuf, int aMaxLen)
{
	int	Len6	= m_iLenMes-(7+6);

	if(Len6 > 0)
	{
		int	Len8	= (Len6/4)*3;

		switch(Len6%4)
		{
		case 1:	Len8 += 1;	break;
		case 2:	Len8 += 2;	break;
		case 3:	Len8 += 3;	break;
		};

		Conv6To8(Len6, 1+6, Len8, static_cast<BYTE*>(aBuf), aMaxLen);

		return Len8;
	}
	return 0;
}

int		CMes::LvlAsk10() const
{
	if(m_iTypMes == tmSostUnitD && m_iAsk == amValLift)
	{
		if(m_iLenMes < 8+4+8+2)			return(0);
		else if(m_iLenMes < 8+4+8+8+2)	return(1);
		else							return(2);
	}
	return(-1);
}

bool	CMes::TestBit(bitExtended aBit)
{
	if(LvlAsk10() > 0)
	{
		if(aBit >= beOpen && aBit < beEnd)
		{
			int	nByte = aBit / 6;
			int nBit  = 1 << aBit % 6;

			return((m_bMes[8+4+nByte] & nBit) != 0);
		}
	}
	return false;
}

int		CMes::CurLvl()
{
	if(LvlAsk10() > 0)
	{
		int CurLvl = m_bMes[8+4+2] & 0x3F;

		//if( CurLvl < 0 || CurLvl > 32) CurLvl = 0;

		return(CurLvl);
	}
	return 0;
}

CMes::modeLift	CMes::Mode()
{
	if(LvlAsk10() > 0)
	{
		int Mode = m_bMes[8+4+3] & 0xF;

		return((CMes::modeLift)Mode);
	}
	return mlNorma;
}

//int	CMes::TypDev()
//{
//	if(LvlAsk10() > 0)
//	{
//		int Typ = m_bMes[8+4+4] & 0x3F;
//
//		return(Typ);
//	}
//	return 0;
//}

//int	CMes::TypExt()
//{
//	if(LvlAsk10() > 0)
//	{
//		int Typ = m_bMes[8+4+5] & 0x3F;
//
//		return(Typ);
//	}
//	return 0;
//}

int		CMes::CodDmamge()
{
	if(LvlAsk10() > 0)
	{
		int Cod = ((m_bMes[8+4+6]&0x30) >> 4) | ((m_bMes[8+4+7]&0x3f) << 2);

		return(Cod);
	}
	return 0;
}

bool	CMes::TestAsk(int aNumLvl)
{
	if(LvlAsk10() > 1)
	{
		aNumLvl--;

		if(aNumLvl >= 0 && aNumLvl < 24)
		{
			int	nByte = aNumLvl / 6;
			int nBit  = 1 << aNumLvl % 6;

			return((m_bMes[8+4+8+nByte] & nBit) != 0);
		}
	}
	return false;
}

bool	CMes::TestCmd(int aNumLvl)
{
	if(LvlAsk10() > 1)
	{
		aNumLvl--;

		if(aNumLvl >= 0 && aNumLvl < 24)
		{
			int	nByte = aNumLvl / 6;
			int nBit  = 1 << aNumLvl % 6;

			return((m_bMes[8+4+8+4+nByte] & nBit) != 0);
		}
	}
	return false;
}

CAtlString   CMes::nameMode(modeLift aMode)
{
	switch(aMode)
	{
	case mlNorma:		return _S_(IDS_MODE_NORMA);
	case mlLoading:		return _S_(IDS_MODE_LOADING);
	case mlFire:		return _S_(IDS_MODE_FIRE);
	case mlRevision:	return _S_(IDS_MODE_REVISION);
	case mlDrivMP:		return _S_(IDS_MODE_DRIVMP);
	case mlMP1:			return _S_(IDS_MODE_MP1);
	case mlParam:		return _S_(IDS_MODE_PARAM);
	case mlMP2:			return _S_(IDS_MODE_MP2);
	case mlCorrRun:		return _S_(IDS_MODE_CORRRUN);
	case mlMornMode:	return _S_(IDS_MODE_MORNMODE);
	case mlEvenMode:	return _S_(IDS_MODE_EVENMODE);
	case mlGuide:		return _S_(IDS_MODE_GUIDE);
	case mlRemotOff:	return _S_(IDS_MODE_REMOTEOFF);
	case mlDamage:		return _S_(IDS_MODE_DAMAGE);
	case mlSeismic:		return _S_(IDS_MODE_SEISMIC);
	case mlHospital:	return _S_(IDS_MODE_HOSPITAL);
	}
	return _T("");
}

bool CMes::Conv6To8(int aLen6, const BYTE* aDann6, int aLen8, BYTE* aDann8, int aMaxLen8)
{
	int	i		= 0;
	int	index	= 0;
	int	Count4	= aLen6/4;

	if( (aLen6%4) != 0) Count4++;

	memset(aDann8, 0, aMaxLen8);

	for(i = 0; i < Count4; i++)
	{
		if(index >= aMaxLen8) break;
		aDann8[index++] =  (aDann6[i*4+0]&0x3F)       | ((aDann6[i*4+1]&0x3)  << 6);
		if(index >= aMaxLen8) break;
		aDann8[index++] = ((aDann6[i*4+1]&0x3C) >> 2) | ((aDann6[i*4+2]&0xF)  << 4);
		if(index >= aMaxLen8) break;
		aDann8[index++] = ((aDann6[i*4+2]&0x30) >> 4) | ((aDann6[i*4+3]&0x3F) << 2);
	}

	return i == Count4;
}

bool CMes::Conv6To8(int aLen6, int aPosBeg, int aLen8, BYTE* aDann8, int aMaxLen8)
{
	return Conv6To8(aLen6, &m_bMes[6+aPosBeg], aLen8, aDann8, aMaxLen8);
}

BYTE CMes::operator[](int nIndex) const
{
	if(nIndex >= 0 && nIndex < m_iLenMes-6)	return(m_bMes[nIndex+6]);
	else									return 0;
}

bool CMes::Conv8To6(int aLen8, const BYTE* aDann8, BYTE* aDann6, int aMaxLen6)
{
	int i		= 0;
	int	index	= 0;
	int tetrad	= aLen8/3;

	if( (aLen8%3) != 0 ) tetrad++;

	memset(aDann6, 0, aMaxLen6);

	for(i = 0; i < tetrad; i++)
	{
		aDann6[index++]	= (aDann8[i*3+0] & 0x3F);
		if(index >= aMaxLen6) break;
		aDann6[index++]	= ((aDann8[i*3+0] >> 6) & 0x3) | (((aDann8[i*3+1] << 2) & 0x3C));
		if(index >= aMaxLen6) break;
		aDann6[index++]	= ((aDann8[i*3+1] >> 4) & 0xF) | (((aDann8[i*3+2] << 4) & 0x30));
		if(index >= aMaxLen6) break;
		aDann6[index++]	= ((aDann8[i*3+2] >> 2) & 0x3F);
		if(index >= aMaxLen6) break;
	}

	return i == tetrad;
}

void	CMes::SetTypMes(typMes aTypMes)
{
	m_iTypMes = aTypMes;
	m_bMes[5] = aTypMes;
}

CAtlString	CMes::Str(const TCHAR* aTitle)
{
	CString	buf(aTitle? aTitle: _T(""));
	
	for(int i = 0; i < m_iLenMes; i++)
	{
		buf.AppendFormat(_T(" %02X"), m_bMes[i]&0xFF);
	}
	return buf;
}

bool	CMes::SetByte(int aIndexData, BYTE aByte)
{
	if(aIndexData >= 0 && aIndexData < LenData())
	{
		m_bMes[6+aIndexData] = aByte;

		return true;
	}

	return false;
}

bool CMes::GetSTAT(long &TotOn, long &TimOn, long &TotDor, long &TimDor)
{
	if(m_iLenMes > 14)
	{
		TotOn	= 0;
		TimOn	= 0;
		TotDor	= 0;
		TimDor	= 0;

		int		index	= 0;
		int		Count4	= 6;
		int		MaxLen	= 16;
		union
		{
			BYTE	byte[16];
			struct
			{
				long	TotOn;
				long	TimOn;
				long	TotDor;
				long	TimDor;
			} Value;
		} Param;

		memset(&Param, 0, sizeof(Param));

		for(int i = 0; i < Count4; i++)
		{
			if(index >= MaxLen) break;
			Param.byte[index] =  (m_bMes[16+i*4+0]&0x3F)       | ((m_bMes[16+i*4+1]&0x3)  << 6);
			index++;

			if(index >= MaxLen) break;
			Param.byte[index] = ((m_bMes[16+i*4+1]&0x3C) >> 2) | ((m_bMes[16+i*4+2]&0xF)  << 4);
			index++;

			if(index >= MaxLen) break;
			Param.byte[index] = ((m_bMes[16+i*4+2]&0x30) >> 4) | ((m_bMes[16+i*4+3]&0x3F) << 2);
			index++;
		}

		TotOn	= Param.Value.TotOn;
		TimOn	= Param.Value.TimOn;
		TotDor	= Param.Value.TotDor;
		TimDor	= Param.Value.TimDor;

		return true;
	}
	return false;
}
