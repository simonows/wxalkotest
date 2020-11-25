#include "stdafx.h"

#include <sost.h>
#include "LoadRes.h"
#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// класс ответа оконечного устройства
//

void CSostDev::Clear()					
{ 
	m_Len	= 0;
	m_InfoCLB.Clear();
	memset(m_bSost,0,mxLen);
}

bool CSostDev::tstBitDirect(int aBit) const
{
	if(aBit >= 0 && aBit < mxLen*6)
	{
		int	nByte = aBit / 6;
		int nBit  = 1 << aBit % 6;

		return((m_bSost[nByte] & nBit) != 0);
	}
	return(false);
}

bool CSostDev::tstBit(int aBit) const
{
	if(aBit == sbNoMove)		return m_InfoCLB.tstBit(CInfoCLB::b5ExistMove);
	else if(aBit == sbNoInform)	return m_InfoCLB.tstBit(CInfoCLB::b5NoInform);
	else if(aBit == sbNoLink)	return m_InfoCLB.tstBit(CInfoCLB::b5NoLink);
	else if(aBit >= 0 && aBit < mxLen*6)
	{
		int	nByte = aBit / 6;
		int nBit  = 1 << aBit % 6;

		return((m_bSost[nByte] & nBit) != 0);
	}
	return(false);
}

void CSostDev::setBit(int aBit)
{
	if(aBit == sbNoMove)		return m_InfoCLB.setBit(CInfoCLB::b5ExistMove);
	else if(aBit == sbNoInform)	return m_InfoCLB.setBit(CInfoCLB::b5NoInform);
	else if(aBit == sbNoLink)	return m_InfoCLB.setBit(CInfoCLB::b5NoLink);
	else if(aBit >= 0 && aBit < mxLen*6)
	{
		int	nByte = aBit / 6;
		int nBit  = 1 << aBit % 6;

		m_bSost[nByte] |= nBit;
	}
}

void CSostDev::clrBit(int aBit)
{
	if(aBit == sbNoMove)		return m_InfoCLB.clrBit(CInfoCLB::b5ExistMove);
	else if(aBit == sbNoInform)	return m_InfoCLB.clrBit(CInfoCLB::b5NoInform);
	else if(aBit == sbNoLink)	return m_InfoCLB.clrBit(CInfoCLB::b5NoLink);
	else if(aBit >= 0 && aBit < mxLen*6)
	{
		int	nByte = aBit / 6;
		int nBit  = 1 << aBit % 6;

		m_bSost[nByte] &= ~nBit;
	}
}

void CSostDev::operator=(const CSostDev& sl)
{
	//Clear();
	//m_Len		= sl.m_Len;
	//memcpy(m_bSost, sl.m_bSost, mxLen);

	// убрано 28.10.2011 memset(m_bSost, 0, mxLen);	// 28.09.2011

	if(sl.m_Len > m_Len)m_Len = sl.m_Len;
	memcpy(m_bSost, sl.m_bSost, sl.m_Len);

	m_InfoCLB	= sl.m_InfoCLB;
}

BYTE CSostDev::operator[](int nIndex) const
{
	if(nIndex >= 0 && nIndex < m_Len)	return(m_bSost[nIndex]);
	else								return 0;
}

bool	CSostDev::operator  == (const CSostDev& sl) const
{
	if(m_Len == sl.m_Len)
	{
		if(m_Len)	
		{
			if(memcmp(m_bSost, sl.m_bSost, m_Len) == 0)
			{
				return m_InfoCLB == sl.m_InfoCLB;
			}
		}
		else
		{

			return m_InfoCLB == sl.m_InfoCLB;
		}
	}
	return false;
}

bool CSostDev::EQ(const CSostDev& sl)	const
{
	if(testNet() && sl.testNet())
	{
		if(Len() == sl.Len())
		{
			CSostDev	S1(*this), S2(sl);

			S1.clrBit(CSostDev::sbRez1);
			S2.clrBit(CSostDev::sbRez1);

			S1.clrBit(CSostDev::sbOnDriver);
			S2.clrBit(CSostDev::sbOnDriver);

			for(int i = 0; i < Len(); i++)
				if( S1[i] != S2[i]) 
					return false;

			return true;
		}
		else
		{
			return false;
		}
	}
	else if(!testNet() && !sl.testNet())
	{
		return m_InfoCLB.EQ(sl.m_InfoCLB);
	}
	else
	{
		return false;
	}
}

CAtlString CSostDev::BinStr(void)
{
	CAtlString	buf;

	for(int i = 0; i < mxLen; i++)
	{
		buf += (m_bSost[i] & 0x20)? '1': '0';
		buf += (m_bSost[i] & 0x10)? '1': '0';
		buf += (m_bSost[i] & 0x08)? '1': '0';
		buf += (m_bSost[i] & 0x04)? '1': '0';
		buf += (m_bSost[i] & 0x02)? '1': '0';
		buf += (m_bSost[i] & 0x01)? '1': '0';

		buf += ' ';
	}

	buf += m_InfoCLB.BinStr();

	return(buf);
}

CAtlString	CSostDev::HexStr()
{
	CAtlString	buf;

	for(int i = 0; i < m_Len; i++)
		buf.AppendFormat(_T("%02X "), m_bSost[i]&0xFF);

	buf	+= m_InfoCLB.HexStr();

	return(buf);
}

void CSostDev::Normaliz()
{
	if(!testNet()) 
	{
		m_Len = 0;
		memset(m_bSost, 0, mxLen);
	}
}

CSostDev::typErrHF	CSostDev::GetCodErrHF()
{
	return static_cast<typErrHF>(((m_bSost[1]>>2)&0x0F) | ((m_bSost[2]<<4)&0x10));
}

bool CSostDev::SetData(const BYTE* aBuf, int aLen)
{
	bool	rc = false;

	Clear();

	if(aBuf)
	{
		if(aLen == 6)
		{
			m_Len = 4;

			memcpy(m_bSost, aBuf, m_Len);
			m_InfoCLB.SetData(aBuf+m_Len, 2);

			rc	= true;
		}
		else if(aLen == 10)
		{
			m_Len = 8;

			memcpy(m_bSost, aBuf, m_Len);
			m_InfoCLB.SetData(aBuf+m_Len, 2);

			rc	= true;
		}
	}
	return rc;
}

bool CSostDev::SetData7(const BYTE* aData, int aLen)
{
    Clear();

    int Len = aLen - 1;

    if( (Len%4) == 0)
    {
        if(Len > 0 && Len <= mxLen)
        {
            m_Len = Len;
            memcpy(m_bSost, aData, m_Len);
            return m_InfoCLB.SetData(aData+m_Len, 1);
        }
    }
    return false;
}

int	CSostDev::GetData(BYTE* aData, int aMaxLen) const
{
	if(Len() == 0)
	{
		int	len = 0;

		if((m_InfoCLB.Len() > 0) && (aMaxLen >= 4 + m_InfoCLB.Len()))
		{
			len	+= 4;

			aData[0] = 0;
			aData[1] = 0;
			aData[2] = 0;
			aData[3] = 0;

			len += m_InfoCLB.GetData(aData+len, aMaxLen-len);
		}
		return len;
	}
	else
	{
		int	len = 0;

		if(aMaxLen >= Len() + m_InfoCLB.Len())
		{
			len	+= Len();
			memcpy(aData, m_bSost, Len());

			len += m_InfoCLB.GetData(aData+len, aMaxLen-len);
		}
		return len;
	}
}

bool CSostDev::IsError()
{
	for(bitLB Bit = sbBegin; Bit < mbMaxBit; Bit = static_cast<bitLB>(Bit+1))
	{
		if(tstBit(Bit) && IsError(Bit)) return true;
	}
	return false;
}

bool CSostDev::IsError(bitLB aBit)
{
	switch(aBit)
	{
	case sbRevers:		return(true);
	case sbInpMP:		return(true);
	case sbNonU:		return(true);
	case sbKeyStop:		return(true);
	case sbOpen:		return(true);
	case sbDestrSec:	return(true);
	case sbBadSenrUB:	return(true);
	case sbKZSecur:		return(true);
	case sbBadMove:		return(true);
	case sbEmergDrv:	return(true);
	case sbEmergOpn:	return(true);
	case sbInpInnr:		return(true);
	case sbBadUB:		return(true);
	case sbEmrgUPL:		return(true);
	case sbOpenMP:		return(true);
	case sbLiftStop:	return(true);
	case sbNoJobDK:		return(true);
	case sbOpenInnr:	return(true);
	case sbNoMove:		return(true);
	case sbNoInform:	return(true);
	case sbNoLink:		return(true);
	case sbLockRD:		return(true);
	case sbVarUSec:		return(true);
	case sbFireRisk:	return(true);
	}
	return(false);
}


bool CSostDev::IsNoValBit(bitLB aBit)
{
	switch(aBit)
	{
	case sbAskMP:		return(true);
	case sbStatL:		return(true);
	case sbStatH:		return(true);
	}
	return(false);
}

int	CSostDev::GetStatDriv()
{
	int	Val = 0;

	if(tstBit(sbStatL)) Val |= 0x01;
	if(tstBit(sbStatH)) Val |= 0x02;

	return Val;
}

UINT	CSostDev::naimSostRes(bitLB aBit)
{
	UINT	StrID = 0;

	switch(aBit)
	{
	case sbKeyServ:		StrID = IDS_LU_SOST_INS_KEY;		break;
	case sbRestart:		StrID = IDS_LU_SOST_RESTART;		break;			
	case sbRevers:		StrID = IDS_LU_SOST_REVERS;			break;			
	case sbInpMP:		StrID = IDS_LU_SOST_INP_MP;			break;			
	case sbNonU:		StrID = IDS_LU_SOST_NO_U;			break;			
	case sbKeyStop:		StrID = IDS_LU_SOST_KEY_STOP;		break;		
	case sbOpen:		StrID = IDS_LU_SOST_OPEN_ELEV;		break;		
	case sbDestrSec:	StrID = IDS_LU_SOST_BREAK_CHAIN;	break;		
	case sbBadSenrUB:	StrID = IDS_LU_SOST_BAD_SENSOR;		break;		
	case sbKZSecur:		StrID = IDS_LU_SOST_SHORT_CHAIN;	break;		
	case sbAsk:			StrID = IDS_LU_SOST_CALL;			break;			
	case sbBadMove:		StrID = IDS_LU_SOST_BAD_MOVE;		break;		
	case sbEmergDrv:	StrID = IDS_LU_SOST_DAMAGE_UKSL;	break;		
	case sbEmergOpn:	StrID = IDS_LU_SOST_DAMAGE_DOOR;	break;		
	case sbInpInnr:		StrID = IDS_LU_SOST_INP_INNER;		break;		
	case sbBadUB:		StrID = IDS_LU_SOST_BAD_UB;			break;			
	case sbEmrgUPL:		StrID = IDS_LU_SOST_DAMAGE_LOCK;	break;		
	case sbOpenMP:		StrID = IDS_LU_SOST_OPEN_MP;		break;			
	case sbLiftStop:	StrID = IDS_LU_SOST_BETWEEN_FLOOR;	break;	
	case sbNoJobDK:		StrID = IDS_LU_SOST_NO_JOB_DK;		break;		
	case sbOpenInnr:	StrID = IDS_LU_SOST_NO_CLOSE_DOOR;	break;	
	case sbOnDriver:	StrID = IDS_LU_SOST_ON_DRIVER;		break;		
	case sbRez1:		StrID = IDS_LU_SOST_REZ1;			break;			
	case sbBadSemi:		StrID = IDS_LU_SOST_BAD_SEMI;		break;		
	case sbLockRD:		StrID = IDS_LU_SOST_LOCK_RD;		break;			
	case sbAskMP:		StrID = IDS_LU_SOST_ASK_MP;			break;			
	case sbBadGGS:		StrID = IDS_LU_SOST_FAULT_GGS;		break;		
	case sbStatL:		StrID = IDS_LU_SOST_STAT_LOW;		break;		
	case sbStatH:		StrID = IDS_LU_SOST_STAT_HIGH;		break;		
	case sbRez29:		StrID = IDS_LU_SOST_REZ29;			break;			
	case sbRez30:		StrID = IDS_LU_SOST_REZ30;			break;			
	case sbRez31:		StrID = IDS_LU_SOST_REZ31;			break;			
	case sbUser1:		StrID = IDS_LU_SOST_USER1;			break;			
	case sbUser2:		StrID = IDS_LU_SOST_USER2;			break;			
	case sbUser3:		StrID = IDS_LU_SOST_USER3;			break;			
	case sbUser4:		StrID = IDS_LU_SOST_USER4;			break;			
	case sbRezU_LB:		StrID = IDS_LU_SOST_REZ_POWER;		break;		
	case sbVarUSec:		StrID = IDS_LU_SOST_VAR_VOLT;		break;
	case sbFireRisk:	StrID = IDS_LU_SOST_FIRE_RISK;		break;
	case sbBadBattery:	StrID = IDS_LU_SOST_BAD_BATTERY;	break;

	case sbNoMove:		StrID = IDS_LU_SOST_IDLE_HOUR;		break;
	case sbNoInform:	StrID = IDS_LU_SOST_NO_INFORM;		break;
	case sbNoLink:		StrID = IDS_LU_SOST_NO_LINK;		break;	
	}
	return StrID;
}

CAtlString	CSostDev::naimSost(bitLB aBit)
{
	//UINT	StrID = 0;

	//switch(aBit)
	//{
	//case sbKeyServ:		StrID = IDS_LU_SOST_INS_KEY;		break;
	//case sbRestart:		StrID = IDS_LU_SOST_RESTART;		break;			
	//case sbRevers:		StrID = IDS_LU_SOST_REVERS;			break;			
	//case sbInpMP:		StrID = IDS_LU_SOST_INP_MP;			break;			
	//case sbNonU:		StrID = IDS_LU_SOST_NO_U;			break;			
	//case sbKeyStop:		StrID = IDS_LU_SOST_KEY_STOP;		break;		
	//case sbOpen:		StrID = IDS_LU_SOST_OPEN_ELEV;		break;		
	//case sbDestrSec:	StrID = IDS_LU_SOST_BREAK_CHAIN;	break;		
	//case sbBadSenrUB:	StrID = IDS_LU_SOST_BAD_SENSOR;		break;		
	//case sbKZSecur:		StrID = IDS_LU_SOST_SHORT_CHAIN;	break;		
	//case sbAsk:			StrID = IDS_LU_SOST_CALL;			break;			
	//case sbBadMove:		StrID = IDS_LU_SOST_BAD_MOVE;		break;		
	//case sbEmergDrv:	StrID = IDS_LU_SOST_DAMAGE_UKSL;	break;		
	//case sbEmergOpn:	StrID = IDS_LU_SOST_DAMAGE_DOOR;	break;		
	//case sbInpInnr:		StrID = IDS_LU_SOST_INP_INNER;		break;		
	//case sbBadUB:		StrID = IDS_LU_SOST_BAD_UB;			break;			
	//case sbEmrgUPL:		StrID = IDS_LU_SOST_DAMAGE_LOCK;	break;		
	//case sbOpenMP:		StrID = IDS_LU_SOST_OPEN_MP;		break;			
	//case sbLiftStop:	StrID = IDS_LU_SOST_BETWEEN_FLOOR;	break;	
	//case sbNoJobDK:		StrID = IDS_LU_SOST_NO_JOB_DK;		break;		
	//case sbOpenInnr:	StrID = IDS_LU_SOST_NO_CLOSE_DOOR;	break;	
	//case sbOnDriver:	StrID = IDS_LU_SOST_ON_DRIVER;		break;		
	//case sbRez1:		StrID = IDS_LU_SOST_REZ1;			break;			
	//case sbBadSemi:		StrID = IDS_LU_SOST_BAD_SEMI;		break;		
	//case sbLockRD:		StrID = IDS_LU_SOST_LOCK_RD;		break;			
	//case sbAskMP:		StrID = IDS_LU_SOST_ASK_MP;			break;			
	//case sbBadGGS:		StrID = IDS_LU_SOST_FAULT_GGS;		break;		
	//case sbStatL:		StrID = IDS_LU_SOST_STAT_LOW;		break;		
	//case sbStatH:		StrID = IDS_LU_SOST_STAT_HIGH;		break;		
	//case sbRez29:		StrID = IDS_LU_SOST_REZ29;			break;			
	//case sbRez30:		StrID = IDS_LU_SOST_REZ30;			break;			
	//case sbRez31:		StrID = IDS_LU_SOST_REZ31;			break;			
	//case sbUser1:		StrID = IDS_LU_SOST_USER1;			break;			
	//case sbUser2:		StrID = IDS_LU_SOST_USER2;			break;			
	//case sbUser3:		StrID = IDS_LU_SOST_USER3;			break;			
	//case sbUser4:		StrID = IDS_LU_SOST_USER4;			break;			
	//case sbRezU_LB:		StrID = IDS_LU_SOST_REZ_POWER;		break;		
	//case sbVarUSec:		StrID = IDS_LU_SOST_VAR_VOLT;		break;
	//case sbFireRisk:	StrID = IDS_LU_SOST_FIRE_RISK;		break;
	//case sbBadBattery:	StrID = IDS_LU_SOST_BAD_BATTERY;	break;

	//case sbNoMove:		StrID = IDS_LU_SOST_IDLE_HOUR;		break;
	//case sbNoInform:	StrID = IDS_LU_SOST_NO_INFORM;		break;
	//case sbNoLink:		StrID = IDS_LU_SOST_NO_LINK;		break;	
	//}
	//return _S_(StrID);
	return _S_(naimSostRes(aBit));
}
