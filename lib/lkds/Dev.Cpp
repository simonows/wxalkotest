#include "stdafx.h"
#include "resource.h"

#include <Dev.h>
#include <slot.h>
#include <Pult.h>
#include <Disp.h>
#include <Xml.h>

/////////////////////////////////////////////////////////////////////////////
// класс устройства на шине
//

const	TCHAR* CDev::s_Dev	= _T("Dev");
const	TCHAR* CDev::s_Addr	= _T("Addr");

CDev::CDev(int aAdrBus, CSlot* apSlot, typDev aTypDev, CParam* aParam) 
	: m_AdrBus(aAdrBus)
	, m_CodRestart(0)
	, m_TypDev(aTypDev)
	, m_pSlot(apSlot)
	, m_LvlKey(0)
	, m_PrefKey(0)
	, m_NumKey(0)
	, m_LastTyp(CMes::tmNone)
	, m_LastCod(CMes::amNone)
	, m_pParam(aParam)
	, m_BitProt(0)
	, m_AskSost(true)
	, m_TypExt(teNone)
	, m_LvlAsk10(laNone)
	, m_SuppStat(false)
	, m_SuppPnt(false)
	, m_SuppIAP(false)
	, m_SuppIONVRAM(false)
	, m_SuppSrv18(false)
	, m_FlagModify(false)
	, m_IsFullWho(false)
	, m_Version(0)
	, m_IsActNVRAM(false)
{
	m_LastMes = COleDateTime::GetCurrentTime();
	m_TimSost = COleDateTime::GetCurrentTime();
	m_TimRestart.SetStatus(COleDateTime::null);
	m_TimKey.SetStatus(COleDateTime::null);
}

CDev::CDev( const CDev &aDev) : m_pParam(NULL)
{
	Clear();

	m_AdrBus		= aDev.m_AdrBus;
	m_TypDev		= aDev.m_TypDev;
	m_TypExt		= aDev.m_TypExt;
	m_Version		= aDev.m_Version;
	m_BitProt		= aDev.m_BitProt;
	m_SuppStat		= aDev.m_SuppStat;
	m_SuppPnt		= aDev.m_SuppPnt;
	m_SuppIAP		= aDev.m_SuppIAP;
	m_SuppIONVRAM	= aDev.m_SuppIONVRAM;
	m_SuppSrv18		= aDev.m_SuppSrv18;
	m_IsFullWho		= aDev.m_IsFullWho;
	m_IsActNVRAM	= aDev.m_IsActNVRAM;
}

CDev::~CDev()
{
	if(m_pParam != NULL) delete m_pParam;
}

void CDev::Clear()
{
	m_AdrBus		= 0;
	m_LastMes.SetStatus(COleDateTime::null);
	m_TimSost.SetStatus(COleDateTime::null);
	m_TimRestart.SetStatus(COleDateTime::null);
	m_CodRestart	= 0;
	m_TypDev		= tdNone;
	m_TypExt		= teNone;
	m_BitProt		= 0;
	m_pSlot			= NULL;		// указатель на блок, к которому подключено устройство
	m_NvRam.Clear();
	m_TimKey.SetStatus(COleDateTime::null);
	m_LvlKey		= 0;
	m_PrefKey		= 0;
	m_NumKey		= 0;
	m_Sost.Clear();
	m_SostDZ.Clear();
	m_AskDirect.Clear();
	m_LastTyp		= CMes::tmNone;
	m_LastCod		= CMes::amNone;
	m_LvlAsk10		= laNone;
	m_AskSost		= true;
	m_SuppStat		= false;
	m_SuppPnt		= false;
	m_SuppIAP		= false;
	m_SuppIONVRAM	= false;
	m_SuppSrv18		= false;
	m_FlagModify	= false;
	m_IsFullWho		= false;
	m_Version		= 0;
}

void	CDev::ClearForLost()
{
	m_TypDev		= tdNone;
	m_TypExt		= teNone;
	m_BitProt		= 0;
	m_NvRam.Clear();
	m_TimKey.SetStatus(COleDateTime::null);
	m_LvlKey		= 0;
	m_PrefKey		= 0;
	m_NumKey		= 0;
//	m_Sost.Clear();
	m_SostDZ.Clear();
	m_AskDirect.Clear();
	m_LastTyp		= CMes::tmNone;
	m_LastCod		= CMes::amNone;
	m_LvlAsk10		= laNone;
	m_AskSost		= true;
	m_SuppStat		= false;
	m_SuppPnt		= false;
	m_SuppIAP		= false;
	m_SuppIONVRAM	= false;
	m_SuppSrv18		= false;
	m_IsFullWho		= false;
	m_Version		= 0;
}

void CDev::Put()
{
	m_pSlot->Put();
}

void CDev::AddMes(int aPrior, CMes::typMes aTypMes, CMes::askFromMain aAsk, const BYTE* aMask, int aLen, int aDuration)
{
	m_pSlot->AddMes(aPrior, aTypMes, aAsk, m_AdrBus, aMask, aLen, aDuration);
}

void CDev::AddMes(int aPrior, CMes::typService aService, int aDuration, CMes::askFromMain aAsk, const BYTE* aData, int aLen)
{
	m_pSlot->AddMes(aPrior, aService, aDuration, m_AdrBus, aAsk, aData, aLen);
}

void	CDev::AddMes(int aPrior, CMes::typPrefix aTypPrefix)
{
	m_pSlot->AddMes(aPrior, aTypPrefix, m_AdrBus);
}

void	CDev::AddMes(int aPrior, CMes::typPrefix aTypPrefix, CMes::askFromMain aAsk, int aLen, const BYTE* aBody)
{
	m_pSlot->AddMes(aPrior, aTypPrefix, m_AdrBus, aAsk, aLen, aBody);
}

void	CDev::AddMes(int aPrior, CMes::subServ18 aSubServ18, const BYTE* aBody, int aLen)
{
	m_pSlot->AddMes(aPrior, m_AdrBus, aSubServ18, aBody, aLen);
}

bool	CDev::SetParDev(const CMes& Mes)
{
	if(Mes.m_iAsk == CMes::amNone)
	{
		CDev::typExt TypExt	= static_cast<CDev::typExt>(Mes[3+2]);

		if(Mes.Len() >= 30 && Mes[4+2] == CMes::saParamLU)
		{
			m_IsFullWho	= true;

			TypExt		= static_cast<CDev::typExt>(Mes[3+2] | (Mes[5+2]<<6));
			m_Version	= ((Mes[7+2]&0xFF)*10 + (Mes[8+2]&0xFF)) | ((Mes[9+2]&0xFF)<<8);
			m_IsActNVRAM= (Mes[18+2]&0x01) != 0;
		}
		else if(Mes.Len() == 14)
		{
			m_IsFullWho	= false;

			if(m_TypDev	!= static_cast<CDev::typDev>(Mes[0+2]) || m_TypExt	!= TypExt)	//+ 26.09.2017 
			{																			// сменился тип блока или тип платы расширения
				m_Version		= 0;
				m_IsActNVRAM	= false;
			}																			//- 26.09.2017
		}
		else
		{
			if(m_IsFullWho)	return false;
		}

		bool rc = m_TypDev	!= static_cast<CDev::typDev>(Mes[0+2]) || m_TypExt	!= TypExt;

		m_TypDev		= static_cast<CDev::typDev>(Mes[0+2]);
		m_TypExt		= TypExt;
		m_BitProt		=  (Mes[1+2] >> 2) & 0x3;
		m_SuppStat		= ((Mes[1+2]) & 0x20) != 0;
		m_SuppSrv18		= ((Mes[2+2]) & 0x04) != 0;
		m_SuppIONVRAM	= ((Mes[2+2]) & 0x08) != 0;
		m_SuppIAP		= ((Mes[2+2]) & 0x10) != 0;
		m_SuppPnt		= ((Mes[2+2]) & 0x20) != 0;

		return rc ;
	}
	return false;
}

bool	CDev::GetFloor(const CMes& Mes, int& aCurFloor, int& aMaxFloor, int& aErrFloor1, int& aErrFloor2)
{
	bool	rc = false;

	if(Mes.m_iAsk == CMes::amNumEtaz)
	{
		int	LenData = Mes.LenData() - 2;

		if(LenData >= 10)
		{
			aCurFloor	=((Mes[4+2]&0xFF) << 1) | (Mes[0+2]&0x01);
			aMaxFloor	=(Mes[5+2]&0xFF) << 1;
			aErrFloor1	= Mes[2+2]&0xFF;
			aErrFloor2	= Mes[3+2]&0xFF;
				
			rc = true;
		}
		else if(LenData >= 6)
		{
			if((Mes[1+2]&0xFF) != 0)		// указана высота
			{
				aCurFloor	= Mes[0+2]&0xFF;
				aMaxFloor	= Mes[1+2]&0xFF;
				aErrFloor1	= Mes[2+2]&0xFF;
				aErrFloor2	= Mes[3+2]&0xFF;
				
				rc = true;
			}
		}
	}
	return rc;
}


bool	CDev::IsKnovExt()
{
	switch(m_TypDev)
	{
	case tdLBO:
	case tdShulk:
	case tdUKL:
	case tdRSUL:
		{
			return(m_TypExt != teNone);
		}
/*	case tdNone:
		{
			return(false);
		} */
	}

	return(true);
}

bool	CDev::IsExtended()
{
	switch(m_LvlAsk10)
	{
	case laExtended:
	case laFull:
		return true;
	};
	return false;
}

bool	CDev::IsV6()
{
	return IsV6(m_TypDev);
}

bool	CDev::IsV6(int aTyp)
{
	return aTyp==tdLB6X || aTyp==tdLB6Pro || aTyp==tdLB7;
}

bool	CDev::IsPro()
{
	return IsPro(m_TypDev);
}

bool	CDev::IsPro(int aTyp)
{
	return aTyp==tdLB6Pro || aTyp==tdLB7;
}

bool	CDev::IsIT()
{
	return m_TypDev==tdIT || m_TypDev==tdIT60 || m_TypDev==tdIT7;
}

CAtlString	CDev::TypLB()
{
	switch(m_TypDev)
	{
	case tdLBC:
		{
			return(S(IDS_LB_CDC));
		}
	case tdLBO:
		{
			return(S(IDS_LB_54OTIS));
		}
	case tdShulk:
		{
			switch(m_TypExt)
			{
			case te17g:
			case te17a:
			case te17b:
			case te25g:
			case te25a:
			case te25b:
				{
					return(S(IDS_LB_54S17));
				}
			case te32g:
			case te32a:
			case te32b:
				{
					return(S(IDS_LB_54S32));
				}
			default:
				{
					return(S(IDS_LB_54SULK));
				}
			}
			break;
		}
	case tdUKL:
		{
			return(S(IDS_LB_54UKL));
		}
	case tdRSUL:
		{
			switch(m_TypExt)
			{
			case teMogil:
				{
					return(S(IDS_LB_54RSULMOG));
				}
			case teMPPL:
				{
					return(S(IDS_LB_54RSULMPPL));
				}
			default:
				{
					return(S(IDS_LB_54RSUL));
				}
			}
			break;
		}
	case tdLB5X:
		{
			return(S(IDS_LB_55));
		}
	case tdLB6X:
		{
			switch(m_TypExt)
			{
			case te6Load:		return S(IDS_LB_60LOAD);
			case te6Mog:		return S(IDS_LB_60UUL);
			case te6MEL:		return S(IDS_LB_60MEL);
			case te6MPPL:		return S(IDS_LB_60MPPL);
			case te6MPPL2:		return S(IDS_LB_60MPPL2);
			case te6Otis:		return S(IDS_LB_60OTIS);
			case te6S17:		return S(IDS_LB_60S17); 
			case te6S32:		return S(IDS_LB_60S32);
			case te6UCL:		return S(IDS_LB_60UKL);
			case te6Rele:		return S(IDS_LB_60REL);
			case te6Sodi:		return S(IDS_LB_60SODIMAS);
			case te6LGDI:		return S(IDS_LB_60LG_DI);
			case te6LGDSS:		return S(IDS_LB_60LG_DSS);
			case teEscal:		return S(IDS_LB_60ESCAL);
			case te6AXEL:		return S(IDS_LB_60AXEL);
			case te6ELEX:		return S(IDS_LB_60ELEX);
			case te6SPUL:		return S(IDS_LB_60SPUL);
			case te6THYSSEN:	return S(IDS_LB_60THYSSEN);
			case te6OLIMP:		return S(IDS_LB_60OLIMP);
			case te6Doppler:	return S(IDS_LB_60DOPPLER);
			case te6THYSSEN3:	return S(IDS_LB_60LS3);
			case te6EXPRESS:	return S(IDS_LB_60EXPRESS);
			case te6BLT:		return S(IDS_LB_60BLT);
			case te6THYS_50:	return S(IDS_LB_60TAC50);
			case te6LISA:		return S(IDS_LB_60LISA);
			case te6Schind:		return S(IDS_LB_60SCHINDLER);
			case te6PLK:		return S(IDS_LB_60PKL);
			case te6UELR:		return S(IDS_LB_60UELR);
			case te6THYSMCI:	return S(IDS_LB_60MCI);
			case te6ORONA:		return S(IDS_LB_60ORONA);
			case te6ARKEL:		return S(IDS_LB_60ARKEL);
			case te6BG15:		return S(IDS_LB_60BG15);
			case te6VEGA:		return S(IDS_LB_60VEGA);
			case te6SodQI:		return S(IDS_LB_60SODQI);
			case te6TR:			return S(IDS_LB_60TR);
			case te6DMG:		return S(IDS_LB_60DMG);
			case te6MIK_EL:		return S(IDS_LB_60MIK_EL);
			case te6KONE:		return S(IDS_LB_60KONE);
			case te6S9:			return S(IDS_LB_60S9);
			case te6HYUNDAI:	return S(IDS_LB_60HYUNDAI);
			case te6NICE:		return S(IDS_LB_60NICE);
			case te6VASSLER:	return S(IDS_LB_60VASSLER);
			case te6FST2:		return S(IDS_LB_60FST2);
			case te6CANNY:		return S(IDS_LB_60CANNY);
			case te6SILVER:		return S(IDS_LB_60SILVER);
			case te6Invalid:	return S(IDS_LB_60Invalid);
			case te6BETACTRL:	return S(IDS_LB_60BETACTRL);
			case te6StaGeHEL:	return S(IDS_LB_60StaGeHEL);
			case te6SK6000:		return S(IDS_LB_60SK6000);
			case te6TKL:		return S(IDS_LB_60TKL);
			case te6SUL2010:	return S(IDS_LB_60SUL2010);
			case te6Union:		return S(IDS_LB_60Union);
			case te6SH_R:		return S(IDS_LB_60SHR);
			case te6FT9X0:		return S(IDS_LB_60FT9X0);
			case te6LLC:		return S(IDS_LB_60LLC);	
			case te6KOLLMORGEN:	return S(IDS_LB_60KOLLMORGEN);
			case te6SECURLIFT:	return S(IDS_LB_60SECURLIFT);	
			case te6IMEM:		return S(IDS_LB_60IMEM);
			case te6WIPO:		return S(IDS_LB_60WIPO);
			case te6VESTNER:	return S(IDS_LB_60VESTNER);
			case te6CARLOS_SILVA:	return  S(IDS_LB_60CARLOS_SILVA);
			case te6KOYO:		return S(IDS_LB_60KOYO);
			case te6BL6:		return S(IDS_LB_60BL6);
			case te6WEBER:		return S(IDS_LB_60WEBER);
			case te6iAStar:		return S(IDS_LB_60iAStar);
			case te6GTE:		return S(IDS_LB_60GTE);
			case te6AUTINOR:	return S(IDS_LB_60AUTINOR);
			case te6THYSCMC:	return S(IDS_LB_60THYSCMC);
			case te6DAESUNG:	return S(IDS_LB_60DAESUNG);
			case te6AS380:		return S(IDS_LB_60AS380);
			case te6ECL1:		return S(IDS_LB_60ECL1);
			case te6ECLIPSE:	return S(IDS_LB_60ECLIPSE);
			case te6SKG:		return S(IDS_LB_60SKG);
			case te6ML65X:		return S(IDS_LB_60ML65X);
			case te6ML60X:		return S(IDS_LB_60ML60X);
			case te6PDAHL:		return S(IDS_LB_60PDAHL);
			case te6ULGM:		return S(IDS_LB_60ULGM);
			case te6ISLv4:		return S(IDS_LB_60ISLv4);
			case te6ML50:		return S(IDS_LB_60ML50);
			case teViaSerie:	return S(IDS_LB_60VIASERIE);
			default:
				{
					CAtlString	Ext;

					Ext.Format(_T(" (%d)"), m_TypExt);

					return S(IDS_LB_60)+Ext;
				}
			}
		}
	case tdLB6Pro:
		{
			switch(m_TypExt)
			{
			case te6Load:		return S(IDS_LB_61LOAD);
			case te6Mog:		return S(IDS_LB_61UUL);
			case te6MEL:		return S(IDS_LB_61MEL);
			case te6MPPL:		return S(IDS_LB_61MPPL);
			case te6MPPL2:		return S(IDS_LB_61MPPL2);
			case te6Otis:		return S(IDS_LB_61OTIS);
			case te6S17:		return S(IDS_LB_61S17); 
			case te6S32:		return S(IDS_LB_61S32);
			case te6UCL:		return S(IDS_LB_61UKL);
			case te6Rele:		return S(IDS_LB_61REL);
			case te6Sodi:		return S(IDS_LB_61SODIMAS);
			case te6LGDI:		return S(IDS_LB_61LG_DI);
			case te6LGDSS:		return S(IDS_LB_61LG_DSS);
			case teEscal:		return S(IDS_LB_61ESCAL);
			case te6AXEL:		return S(IDS_LB_61AXEL);
			case te6ELEX:		return S(IDS_LB_61ELEX);
			case te6SPUL:		return S(IDS_LB_61SPUL);
			case te6THYSSEN:	return S(IDS_LB_61THYSSEN);
			case te6OLIMP:		return S(IDS_LB_61OLIMP);
			case te6Doppler:	return S(IDS_LB_61DOPPLER);
			case te6THYSSEN3:	return S(IDS_LB_61LS3);
			case te6EXPRESS:	return S(IDS_LB_61EXPRESS);
			case te6BLT:		return S(IDS_LB_61BLT);
			case te6THYS_50:	return S(IDS_LB_61TAC50);
			case te6LISA:		return S(IDS_LB_61LISA);
			case te6Schind:		return S(IDS_LB_61SCHINDLER);
			case te6PLK:		return S(IDS_LB_61PKL);
			case te6UELR:		return S(IDS_LB_61UELR);
			case te6THYSMCI:	return S(IDS_LB_61MCI);
			case te6ORONA:		return S(IDS_LB_61ORONA);
			case te6ARKEL:		return S(IDS_LB_61ARKEL);
			case te6BG15:		return S(IDS_LB_61BG15);
			case te6VEGA:		return S(IDS_LB_61VEGA);
			case te6SodQI:		return S(IDS_LB_61SODQI);
			case te6TR:			return S(IDS_LB_61TR);
			case te6DMG:		return S(IDS_LB_61DMG);
			case te6MIK_EL:		return S(IDS_LB_61MIK_EL);
			case te6KONE:		return S(IDS_LB_61KONE);
			case te6S9:			return S(IDS_LB_61S9);
			case te6HYUNDAI:	return S(IDS_LB_61HYUNDAI);
			case te6NICE:		return S(IDS_LB_61NICE);
			case te6VASSLER:	return S(IDS_LB_61VASSLER);
			case te6FST2:		return S(IDS_LB_61FST2);
			case te6CANNY:		return S(IDS_LB_61CANNY);
			case te6SILVER:		return S(IDS_LB_61SILVER);
			case te6Invalid:	return S(IDS_LB_61Invalid);
			case te6BETACTRL:	return S(IDS_LB_61BETACTRL);
			case te6StaGeHEL:	return S(IDS_LB_61StaGeHEL);
			case te6SK6000:		return S(IDS_LB_61SK6000);
			case te6TKL:		return S(IDS_LB_61TKL);
			case te6SUL2010:	return S(IDS_LB_61SUL2010);
			case te6Union:		return S(IDS_LB_61Union);	
			case te6SH_R:		return S(IDS_LB_61SHR);
			case te6FT9X0:		return S(IDS_LB_61FT9X0);
			case te6LLC:		return S(IDS_LB_61LLC);	
			case te6KOLLMORGEN:	return S(IDS_LB_61KOLLMORGEN);
			case te6SECURLIFT:	return S(IDS_LB_61SECURLIFT);	
			case te6IMEM:		return S(IDS_LB_61IMEM);
			case te6WIPO:		return S(IDS_LB_61WIPO);
			case te6VESTNER:	return S(IDS_LB_61VESTNER);
			case te6CARLOS_SILVA:	return  S(IDS_LB_61CARLOS_SILVA);
			case te6KOYO:		return S(IDS_LB_61KOYO);
			case te6BL6:		return S(IDS_LB_61BL6);
			case te6WEBER:		return S(IDS_LB_61WEBER);
			case te6iAStar:		return S(IDS_LB_61iAStar);
			case te6GTE:		return S(IDS_LB_61GTE);
			case te6AUTINOR:	return S(IDS_LB_61AUTINOR);
			case te6THYSCMC:	return S(IDS_LB_61THYSCMC);
			case te6DAESUNG:	return S(IDS_LB_61DAESUNG);
			case te6AS380:		return S(IDS_LB_61AS380);
			case te6ECL1:		return S(IDS_LB_61ECL1);
			case te6ECLIPSE:	return S(IDS_LB_61ECLIPSE);
			case te6SKG:		return S(IDS_LB_61SKG);
			case te6ML65X:		return S(IDS_LB_61ML65X);
			case te6ML60X:		return S(IDS_LB_61ML60X);
			case te6PDAHL:		return S(IDS_LB_61PDAHL);
			case te6ULGM:		return S(IDS_LB_61ULGM);
			case te6ISLv4:		return S(IDS_LB_61ISLv4);
			case te6ML50:		return S(IDS_LB_61ML50);
			case teViaSerie:	return S(IDS_LB_61VIASERIE);

			default:			
				{
					CAtlString	Ext;

					Ext.Format(_T(" (%d)"), m_TypExt);

					return S(IDS_LB_61)+Ext;
				}
			}
		}
	case tdLB7:
		{
			CAtlString	Ext;
			
			Ext.Format(_T(" (%d)"), m_TypExt);

			switch(m_TypExt)
			{
			case te6Load:		Ext = S(IDS_LB_LOAD);		break;
			case te6Mog:		Ext = S(IDS_LB_UUL);		break;
			case te6MEL:		Ext = S(IDS_LB_MEL);		break;
			case te6MPPL:		Ext = S(IDS_LB_MPPL);		break;
			case te6MPPL2:		Ext = S(IDS_LB_MPPL2);		break;
			case te6Otis:		Ext = S(IDS_LB_OTIS);		break;
			case te6S17:		Ext = S(IDS_LB_S17); 		break;
			case te6S32:		Ext = S(IDS_LB_S32);		break;
			case te6UCL:		Ext = S(IDS_LB_UKL);		break;
			case te6Rele:		Ext = S(IDS_LB_REL);		break;
			case te6Sodi:		Ext = S(IDS_LB_SODIMAS);	break;
			case te6LGDI:		Ext = S(IDS_LB_LG_DI);		break;
			case te6LGDSS:		Ext = S(IDS_LB_LG_DSS);		break;
			case teEscal:		Ext = S(IDS_LB_ESCAL);		break;
			case te6AXEL:		Ext = S(IDS_LB_AXEL);		break;
			case te6ELEX:		Ext = S(IDS_LB_ELEX);		break;
			case te6SPUL:		Ext = S(IDS_LB_SPUL);		break;
			case te6THYSSEN:	Ext = S(IDS_LB_THYSSEN);	break;
			case te6OLIMP:		Ext = S(IDS_LB_OLIMP);		break;
			case te6Doppler:	Ext = S(IDS_LB_DOPPLER);	break;
			case te6THYSSEN3:	Ext = S(IDS_LB_LS3);		break;
			case te6EXPRESS:	Ext = S(IDS_LB_EXPRESS);	break;
			case te6BLT:		Ext = S(IDS_LB_BLT);		break;
			case te6THYS_50:	Ext = S(IDS_LB_TAC50);		break;
			case te6LISA:		Ext = S(IDS_LB_LISA);		break;
			case te6Schind:		Ext = S(IDS_LB_SCHINDLER);	break;
			case te6PLK:		Ext = S(IDS_LB_PKL);		break;
			case te6UELR:		Ext = S(IDS_LB_UELR);		break;
			case te6THYSMCI:	Ext = S(IDS_LB_MCI);		break;
			case te6ORONA:		Ext = S(IDS_LB_ORONA);		break;
			case te6ARKEL:		Ext = S(IDS_LB_ARKEL);		break;
			case te6BG15:		Ext = S(IDS_LB_BG15);		break;
			case te6VEGA:		Ext = S(IDS_LB_VEGA);		break;
			case te6SodQI:		Ext = S(IDS_LB_SODQI);		break;
			case te6TR:			Ext = S(IDS_LB_TR);			break;
			case te6DMG:		Ext = S(IDS_LB_DMG);		break;
			case te6MIK_EL:		Ext = S(IDS_LB_MIK_EL);		break;
			case te6KONE:		Ext = S(IDS_LB_KONE);		break;
			case te6S9:			Ext = S(IDS_LB_S9);			break;
			case te6HYUNDAI:	Ext = S(IDS_LB_HYUNDAI);	break;
			case te6NICE:		Ext = S(IDS_LB_NICE);		break;
			case te6VASSLER:	Ext = S(IDS_LB_VASSLER);	break;
			case te6FST2:		Ext = S(IDS_LB_FST2);		break;
			case te6CANNY:		Ext = S(IDS_LB_CANNY);		break;
			case te6SILVER:		Ext = S(IDS_LB_SILVER);		break;
			case te6Invalid:	Ext = S(IDS_LB_Invalid);	break;
			case te6BETACTRL:	Ext = S(IDS_LB_BETACTRL);	break;
			case te6StaGeHEL:	Ext = S(IDS_LB_StaGeHEL);	break;
			case te6SK6000:		Ext = S(IDS_LB_SK6000);		break;
			case te6TKL:		Ext = S(IDS_LB_TKL);		break;
			case te6SUL2010:	Ext = S(IDS_LB_SUL2010);	break;
			case te6Union:		Ext = S(IDS_LB_Union);		break;
			case te6SH_R:		Ext = S(IDS_LB_SHR);		break;
			case te6FT9X0:		Ext = S(IDS_LB_FT9X0);		break;
			case te6LLC:		Ext = S(IDS_LB_LLC);		break;
			case te6KOLLMORGEN:	Ext = S(IDS_LB_KOLLMORGEN);	break;
			case te6SECURLIFT:	Ext = S(IDS_LB_SECURLIFT);	break;
			case te6IMEM:		Ext = S(IDS_LB_IMEM);		break;
			case te6WIPO:		Ext = S(IDS_LB_WIPO);		break;
			case te6VESTNER:	Ext = S(IDS_LB_VESTNER);	break;
			case te6CARLOS_SILVA:	Ext = S(IDS_LB_CARLOS_SILVA);	break;
			case te6KOYO:		Ext = S(IDS_LB_KOYO);		break;
			case te6BL6:		Ext = S(IDS_LB_BL6);		break;
			case te6WEBER:		Ext = S(IDS_LB_WEBER);		break;
			case te6iAStar:		Ext = S(IDS_LB_iAStar);		break;
			case te6GTE:		Ext = S(IDS_LB_GTE);		break;
			case te6AUTINOR:	Ext = S(IDS_LB_AUTINOR);	break;
			case te6THYSCMC:	Ext = S(IDS_LB_THYSCMC);	break;
			case te6DAESUNG:	Ext = S(IDS_LB_DAESUNG);	break;
			case te6AS380:		Ext = S(IDS_LB_AS380);		break;
			case te6ECL1:		Ext = S(IDS_LB_ECL1);		break;
			case te6ECLIPSE:	Ext = S(IDS_LB_ECLIPSE);	break;
			case te6SKG:		Ext = S(IDS_LB_SKG);		break;
			case te6ML65X:		Ext = S(IDS_LB_ML65X);		break;
			case te6ML60X:		Ext = S(IDS_LB_ML60X);		break;
			case te6PDAHL:		Ext = S(IDS_LB_PDAHL);		break;
			case te6ULGM:		Ext = S(IDS_LB_ULGM);		break;
			case te6ISLv4:		Ext = S(IDS_LB_ISLv4);		break;
			case te6ML50:		Ext = S(IDS_LB_ML50);		break;
			case teViaSerie:	Ext = S(IDS_LB_VIASERIE);	break;
			case teHidral:		Ext = S(IDS_LB_HIDRAL);		break;
            case teKoneEsc:     Ext = S(IDS_LB_KONE_ESC);		break;
            case teDigilift:    Ext = S(IDS_LB_DIGILIFT);		break;
            case teGMV:         Ext = S(IDS_LB_GMV);		break;
			}
			return  S(IDS_LB_7)+_T(" ")+Ext;
		}
	case tdIT:
		{
			return(S(IDS_LB_IT));
		}
	case tdIT60:
		{
			return(S(IDS_LB_IT60));
		}
	case tdIT7:
		{
			return(S(IDS_LB_IT7));
		}
	default:
		{
			CAtlString	buf;

			buf.Format(_T("(%d) (%d)"), m_TypDev, m_TypExt);

			return buf;
		}
	}
	return _T("");
}

void	CDev::XMLRemove()
{
	if(m_pSlot)
	{
		CComPtr<IXMLDOMNode>	spOldNode;

		m_pSlot->m_XMLNode->removeChild(m_XMLNode, &spOldNode);
	}
}

void	CDev::XMLUpdate()
{
	try
	{
		HRESULT	hr;
		CString	buf;

		CComQIPtr<IXMLDOMElement>	spElem	= m_XMLNode;
		if(!spElem)					throw ErrorXML(_T("Error query interfaces"), 0, s_Dev);

		hr	= spElem->setAttribute(CComBSTR(s_Addr), CComVariant(m_AdrBus));
		if(FAILED(hr))				throw ErrorXML(_T("Error addition"), hr, s_Dev, s_Addr);

		if(m_pParam) m_pParam->XMLUpdate();
	}
	catch(ErrorXML aErr)
	{
		throw aErr;
	}
}

void	CDev::XMLAdd()
{
	if(m_pSlot)
	{
		try
		{
			HRESULT	hr;

			CComPtr<IXMLDOMNode>	spNewNode;

			//hr	= m_pSlot->m_pUM->m_pPult->m_XMLDOM->createNode(CComVariant(NODE_ELEMENT),
			hr	= m_pSlot->m_pUM->m_pPult->m_pDisp->m_XMLDOM->createNode(	CComVariant(NODE_ELEMENT),
																			CComBSTR(s_Dev),
																			NULL,
																			&spNewNode);
			if(FAILED(hr))				throw ErrorXML(_T("Error create"), hr, s_Dev);

			hr	= m_pSlot->m_XMLNode->appendChild(spNewNode, &m_XMLNode);
			if(FAILED(hr))				throw ErrorXML(_T("Error addition"), hr, s_Dev);
			if(!m_XMLNode)				throw ErrorXML(_T("Error addition"), hr, s_Dev);

			XMLUpdate();
		}
		catch(ErrorXML aErr)
		{
			throw aErr;
		}
	}
}

void	CDev::XMLLoad(CComPtr<IXMLDOMNode> spNode)
{
	try
	{
		m_XMLNode	= spNode;	

		CComQIPtr<IXMLDOMElement>	spElem	= spNode;
		if(!spElem)				throw ErrorXML(_T("Error query interfaces"), 0, s_Dev);

		HRESULT		hr;
		CComVariant	value;

		value.Clear();
		hr	= spElem->getAttribute(CComBSTR(s_Addr),		&value);
		if(FAILED(hr))				throw ErrorXML(_T("Error read"), hr, s_Dev, s_Addr);
		if(value.vt != VT_NULL)
		{
			hr	= value.ChangeType(VT_I4);
			if(FAILED(hr))			throw ErrorXML(_T("Error read"), hr, s_Dev, s_Addr);
			m_AdrBus		= value.lVal;
		}
	}
	catch(ErrorXML aErr)
	{
		throw aErr;
	}
}

void	CDev::XMLPostLoad()
{
	try
	{
		if(m_pParam) m_pParam->XMLLoad();
	}
	catch(ErrorXML aErr)
	{
		throw aErr;
	}
}

/////////////////////////////////////////////////////////////////////////////
// устройства, подключенные к блоку
//

const	TCHAR*	CDevTab::s_TabDev	= _T("TabDev");

/////////////////////////////////////////////////////////////////////////////
// сравниватель <

struct LTpDev : public binary_function<CDev*, CDev*, bool>
{ 
	bool operator()(const CDev* _Left, const CDev* _Right) const { return (_Left->m_AdrBus < _Right->m_AdrBus); }
};


void	CDevTab::ReOrder()	
{ 
	sort(LTpDev()); 
}


CDev*	CDevTab::FindDev(int aAdrBus)
{
	for(list<CDev*>::iterator i = begin(); i != end(); ++i)
	{
		if((*i)->m_AdrBus == aAdrBus)
		{
			return( *i );
		}
	}

	return NULL;
}

bool	CDevTab::Insert(CDev* aDev)
{
	if(FindDev(aDev->m_AdrBus) == NULL)
	{
		push_back(aDev);
		return true;
	}
	else
	{
		return false;
	}
}

void	CDevTab::XMLRemove()
{
	if(m_pSlot)
	{
		CComPtr<IXMLDOMNode>	spOldNode;

		m_pSlot->m_XMLNode->removeChild(m_XMLNode, &spOldNode);
	}
}

void	CDevTab::XMLUpdate()
{
	try
	{
		//HRESULT	hr;
		//CString	buf;

		CComQIPtr<IXMLDOMElement>	spElem	= m_XMLNode;
		if(!spElem)					throw ErrorXML(_T("Error query interfaces"), 0, s_TabDev);
	}
	catch(ErrorXML aErr)
	{
		throw aErr;
	}
}

void	CDevTab::XMLAdd()
{
	if(m_pSlot)
	{
		try
		{
			HRESULT	hr;

			CComPtr<IXMLDOMNode>	spNewNode;

			//hr	= m_pSlot->m_pUM->m_pPult->m_XMLDOM->createNode(	CComVariant(NODE_ELEMENT),
			hr	= m_pSlot->m_pUM->m_pPult->m_pDisp->m_XMLDOM->createNode(	CComVariant(NODE_ELEMENT),
																			CComBSTR(s_TabDev),
																			NULL,
																			&spNewNode);
			if(FAILED(hr))				throw ErrorXML(_T("Error create"), hr, s_TabDev);

			hr	= m_pSlot->m_XMLNode->appendChild(spNewNode, &m_XMLNode);
			if(FAILED(hr))				throw ErrorXML(_T("Error addition"), hr, s_TabDev);
			if(!m_XMLNode)				throw ErrorXML(_T("Error addition"), hr, s_TabDev);

			XMLUpdate();
		}
		catch(ErrorXML aErr)
		{
			throw aErr;
		}
	}
}

void	CDevTab::XMLLoad(CComPtr<IXMLDOMNode> spNode)
{
	try
	{
		m_XMLNode	= spNode;	

		CComQIPtr<IXMLDOMElement>	spElem	= spNode;
		if(!spElem)				throw ErrorXML(_T("Error query interfaces"), 0, s_TabDev);

		HRESULT		hr;
		CComVariant	value;

		CComPtr<IXMLDOMNode>	child;
		hr = spNode->get_firstChild(&child);
		if(FAILED(hr))				throw ErrorXML(_T("CDevTab::XMLLoad get_firstChild"), hr);

		while(!(!child))
		{
			DOMNodeType	tp;

			child->get_nodeType(&tp);

			if (tp == NODE_ELEMENT)
			{
				CComBSTR	nodeName;

				child->get_nodeName(&nodeName);

				if( (static_cast<CString>(nodeName)).CompareNoCase(CDev::s_Dev) == 0)
				{
					CString	buf;
					CDev*	Dev		= new CDev();
					Dev->m_pSlot	= m_pSlot;

					Dev->XMLLoad(child);

					CDev*	Exist	= FindDev(Dev->m_AdrBus);

					if(Exist)
					{
						buf.Format(_T("Device %d:%d:%d already exist in structute")
							,Exist->m_pSlot->m_pUM->m_NumUM
							,Exist->m_pSlot->m_NumSLOT
							,Exist->m_AdrBus);

						delete Dev;
						throw ErrorXML(buf, 0);
					}
					else
					{
						Insert(Dev);
					}
				}
			}

			CComPtr<IXMLDOMNode>	next;
			child->get_nextSibling(&next);
			if(FAILED(hr))			throw ErrorXML(_T("CDevTab::XMLLoad get_nextSibling"), hr);
			child	= next;
		}
	}
	catch(ErrorXML aErr)
	{
		throw aErr;
	}
}

CAtlString	CDev::DesignLB(int aDesign)
{
	switch(aDesign)
	{
	case 0:	return _T("AT89S8252");
	case 1:	return _T("uPSD");
	case 2:	return _T("CM3");
	}

	CString buf;

	buf.Format(_T("%d"), aDesign);

	return buf;
}

CAtlString	CDev::TypCodec(int aCodec)
{
	switch(aCodec)
	{
	case 0:	return _T("-");
	case 1:	return _T("CMX");
	case 2:	return _T("CT8022");
	case 3:	return _T("STM32F103");
	case 4:	return _T("STM32F405");
	}

	CString buf;

	buf.Format(_T("%d"), aCodec);

	return buf;
}

CAtlString	CDev::S(UINT aID)
{
	extern HINSTANCE s_hInstRes;

	TCHAR	buf[1024];

#if defined(WIN32_PLATFORM_PSPC) || defined(WIN32_PLATFORM_WFSP)
	if(!::LoadString(s_hInstRes, aID, buf, _countof(buf)))	_stprintf(buf, _T("S%d"), aID);
#else
	if(!::LoadString(s_hInstRes, aID, buf, _countof(buf)))	_stprintf_s(buf, _countof(buf), _T("S%d"), aID);
#endif

	return buf;
}
