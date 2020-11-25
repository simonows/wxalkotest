#include "stdafx.h"

#include <Um.h>
#include <Pult.h>
#include <Disp.h>
#include <Xml.h>
#include "resource.h"

const	TCHAR* CUM::s_Um		= _T("Um");
const	TCHAR* CUM::s_Number	= _T("Number");
const	TCHAR* CUM::s_NoAdmin	= _T("NoAdmin");
const	TCHAR* CUM::s_Note		= _T("Note");

/////////////////////////////////////////////////////////////////////////////
// класс УМ, вместе с подключенными к нему КЛШ

void	CUM::Clear()
{
	m_bLink			= false;
	m_bNoAdmin		= false;
	m_pPult			= NULL;
	m_NumUM			= 0;
	m_Note.Empty();
	m_FlagModify	= false;
	m_TabSlot.clear();
}

CUM::CUM( CUM &aUM) :  m_pParam(NULL)
{
	Clear();

	m_NumUM		= aUM.m_NumUM;
	m_bNoAdmin	= aUM.m_bNoAdmin;

	for(list<CSlot*>::iterator i = aUM.m_TabSlot.begin(); i != aUM.m_TabSlot.end(); ++i)
	{
		CSlot* ItO = *i;
		CSlot* ItN = new CSlot(*ItO);

		ItN->m_pUM = this;

		m_TabSlot.push_back( ItN );
	}
}

CUM::~CUM()
{
	if(m_pParam != NULL) delete m_pParam;
}

CSlot* CUM::FindInpLink()
{
	for(list<CSlot*>::iterator i = m_TabSlot.begin(); i != m_TabSlot.end(); ++i)
	{
		if((*i)->m_pLink)
		{
			if((*i)->IsInpLink()) return(*i);
		}
	}
	return(NULL);
}

bool	CUM::ExistOutLink()
{
	for(list<CSlot*>::iterator i = m_TabSlot.begin(); i != m_TabSlot.end(); ++i)
	{
		if((*i)->IsLine()	&& (*i)->m_pLink)				return true;
		if((*i)->IsEther()	&& (*i)->m_TabLinkNet.size()>0)	return true;
	}
	return false;
}

void	CUM::SetAddr()
{
	for(list<CSlot*>::iterator i = m_TabSlot.begin(); i != m_TabSlot.end(); ++i)
	{
		(*i)->SetAddr();
	}
}

void CUM::MarkLink()
{
	m_bLink = true;

	for(list<CSlot*>::iterator i = m_TabSlot.begin(); i != m_TabSlot.end(); ++i)
	{
		CSlot* ItS = *i;
		
		if(ItS->IsLine()) 
		{
			if(ItS->m_pLink != NULL) ItS->m_pLink->m_pUM->MarkLink();
		}
		else if(ItS->IsEther())
		{
			for(list<CLinkNet*>::iterator m = ItS->m_TabLinkNet.begin(); m != ItS->m_TabLinkNet.end(); ++m)
			{
				CLinkNet* ItLink = *m;

				if(ItLink->m_pLink) ItLink->m_pLink->m_pUM->MarkLink();
			}
		}
	}
}

void CUM::ReLink()
{
	m_bLink = true;

	for(list<CSlot*>::iterator i = m_TabSlot.begin(); i != m_TabSlot.end(); ++i)
	{
		CSlot* ItS = *i;
		
		if(ItS->m_TypSlot == CSlot::tsCLLi)
		{
			ItS->ReLinkOne(CSlot::tsCLLi, CSlot::tsCLLo);
		}
		else if(ItS->m_TypSlot == CSlot::tsBRDGi)
		{
			ItS->ReLinkOne(CSlot::tsBRDGi, CSlot::tsBRDGo);
		}
		else if(ItS->m_TypSlot == CSlot::tsCRCi)
		{
			ItS->ReLinkEth(CSlot::tsCRCi, CSlot::tsCRCo);
		}
		else if(ItS->m_TypSlot == CSlot::tsGSMi)
		{
			ItS->ReLinkEth(CSlot::tsGSMi, CSlot::tsGSMo);
		}
		else if(ItS->m_TypSlot == CSlot::tsNETi)
		{
			ItS->ReLinkEth(CSlot::tsNETi, CSlot::tsNETo);
		}
		else if(ItS->m_TypSlot == CSlot::tsCLLEi)
		{
			ItS->ReLinkEth(CSlot::tsCLLEi, CSlot::tsCLLEo);
		}
		else if(ItS->m_TypSlot == CSlot::tsCLLMi)
		{
			ItS->ReLinkEth(CSlot::tsCLLMi, CSlot::tsCLLMo);
		}
		else if(ItS->m_TypSlot == CSlot::tsCLLRi)
		{
			ItS->ReLinkEth(CSlot::tsCLLRi, CSlot::tsCLLRo);
		}
	}
}

CSlot*	CUM::DispSlot()
{
	for(list<CSlot*>::iterator i = m_TabSlot.begin(); i != m_TabSlot.end(); ++i)
	{
		if((*i)->m_TypSlot == CSlot::tsMon) return(*i);
	}
	return(NULL);
}

void	CUM::SetAdmPult()
{
	for(list<CSlot*>::iterator i = m_TabSlot.begin(); i != m_TabSlot.end(); ++i)
	{
		if((*i)->m_TypSlot != CSlot::tsMon) (*i)->SetAdmPult();
	}
}

int	CUM::TotlDisp()
{
	int Total = 0;

	for(list<CSlot*>::iterator i = m_TabSlot.begin(); i != m_TabSlot.end(); ++i)
	{
		if((*i)->m_TypSlot == CSlot::tsMon) Total++;
	}
	return(Total);
}

void	CUM::SetTimeConf()
{
	for(list<CSlot*>::iterator i = m_TabSlot.begin(); i != m_TabSlot.end(); ++i)
	{
		if((*i)->IsDevice()) (*i)->SetTimeConf();
	}
}

int	CUM::TotlCLL()
{
	int Total = 0;

	for(list<CSlot*>::iterator i = m_TabSlot.begin(); i != m_TabSlot.end(); ++i)
	{
		CSlot* ItS = *i;
		
		if(ItS->m_TypSlot == CSlot::tsCLLo || 
		   ItS->m_TypSlot == CSlot::tsCLLi ||
		   ItS->m_TypSlot == CSlot::tsBRDGo|| 
		   ItS->m_TypSlot == CSlot::tsBRDGi||
		   ItS->m_TypSlot == CSlot::tsCRCo || 
		   ItS->m_TypSlot == CSlot::tsCRCi ||
		   ItS->m_TypSlot == CSlot::tsGSMo || 
		   ItS->m_TypSlot == CSlot::tsGSMi ||
		   ItS->m_TypSlot == CSlot::tsCLLEo||
		   ItS->m_TypSlot == CSlot::tsCLLEi||
		   ItS->m_TypSlot == CSlot::tsCLLMo||
		   ItS->m_TypSlot == CSlot::tsCLLMi||
		   ItS->m_TypSlot == CSlot::tsCLLRo||
		   ItS->m_TypSlot == CSlot::tsCLLRi||
		   ItS->m_TypSlot == CSlot::tsNETo || 
		   ItS->m_TypSlot == CSlot::tsNETi) Total++;
	}
	return(Total);
}


bool	CUM::IsLocal()
{
	return(m_NumUM == m_pPult->m_RootUM);
}

void	CUM::XMLRemove()
{
	if(m_pPult)
	{
		CComPtr<IXMLDOMNode>	spOldNode;

		m_pPult->m_XMLNode->removeChild(m_XMLNode, &spOldNode);
	}
}

void	CUM::XMLUpdate()
{
	try
	{
		HRESULT	hr;
		CString	buf;

		CComQIPtr<IXMLDOMElement>	spElem	= m_XMLNode;
		if(!spElem)					throw ErrorXML(IDS_XML_ERRQI, 0, s_Um);

		hr	= spElem->setAttribute(CComBSTR(s_Number), CComVariant(m_NumUM));
		if(FAILED(hr))				throw ErrorXML(IDS_XML_ERRAPPEND, hr, s_Um, s_Number);

		hr	= spElem->setAttribute(CComBSTR(s_NoAdmin), CComVariant(m_bNoAdmin?1:0));
		if(FAILED(hr))				throw ErrorXML(IDS_XML_ERRAPPEND, hr, s_Um, s_NoAdmin);

		hr	= spElem->setAttribute(CComBSTR(s_Note), CComVariant(m_Note));
		if(FAILED(hr))				throw ErrorXML(IDS_XML_ERRAPPEND, hr, s_Um, s_Note);

		if(m_pParam) m_pParam->XMLUpdate();
	}
	catch(ErrorXML aErr)
	{
		throw aErr;
	}
}

void	CUM::XMLAdd()
{
	if(m_pPult)
	{
		try
		{
			HRESULT	hr;

			CComPtr<IXMLDOMNode>	spNewNode;

			//hr	= m_pPult->m_XMLDOM->createNode(	CComVariant(NODE_ELEMENT),
			hr	= m_pPult->m_pDisp->m_XMLDOM->createNode(	CComVariant(NODE_ELEMENT),
															CComBSTR(s_Um),
															NULL,
															&spNewNode);
			if(FAILED(hr))				throw ErrorXML(IDS_XML_ERRCREATE, hr, s_Um);

			hr	= m_pPult->m_XMLNode->appendChild(spNewNode, &m_XMLNode);
			if(FAILED(hr))				throw ErrorXML(IDS_XML_ERRAPPEND, hr, s_Um);
			if(!m_XMLNode)				throw ErrorXML(IDS_XML_ERRAPPEND, hr, s_Um);

			XMLUpdate();
		}
		catch(ErrorXML aErr)
		{
			throw aErr;
		}
	}
}

void	CUM::XMLLoad(CComPtr<IXMLDOMNode> spNode)
{
	try
	{
		m_XMLNode	= spNode;	

		CComQIPtr<IXMLDOMElement>	spElem	= spNode;
		if(!spElem)				throw ErrorXML(IDS_XML_ERRQI, 0, s_Um);

		HRESULT		hr;
		CComVariant	value;

		value.Clear();
		hr	= spElem->getAttribute(CComBSTR(s_Number),		&value);
		if(FAILED(hr))				throw ErrorXML(IDS_XML_ERRREAD, hr, s_Um, s_Number);
		if(value.vt != VT_NULL)
		{
			hr	= value.ChangeType(VT_I4);
			if(FAILED(hr))			throw ErrorXML(IDS_XML_ERRREAD, hr, s_Um, s_Number);
			m_NumUM		= value.lVal;
		}

		value.Clear();
		hr	= spElem->getAttribute(CComBSTR(s_NoAdmin),		&value);
		if(FAILED(hr))				throw ErrorXML(IDS_XML_ERRREAD, hr, s_Um, s_NoAdmin);
		if(value.vt != VT_NULL)
		{
			hr	= value.ChangeType(VT_I4);
			if(FAILED(hr))			throw ErrorXML(IDS_XML_ERRREAD, hr, s_Um, s_NoAdmin);
			m_bNoAdmin		= value.lVal != 0;
		}

		value.Clear();
		hr	= spElem->getAttribute(CComBSTR(s_Note),		&value);
		if(FAILED(hr))				throw ErrorXML(IDS_XML_ERRREAD, hr, s_Um, s_Note);
		if(value.vt != VT_NULL)
		{
			m_Note		= value;
			m_Note.Trim();
		}

		CComPtr<IXMLDOMNode>	child;
		hr = spNode->get_firstChild(&child);
		if(FAILED(hr))				throw ErrorXML(IDS_XML_ERRFRSCHILD, hr);

		while(!(!child))
		{
			DOMNodeType	tp;

			child->get_nodeType(&tp);

			if (tp == NODE_ELEMENT)
			{
				CComBSTR	nodeName;

				child->get_nodeName(&nodeName);

				if( (static_cast<CString>(nodeName)).CompareNoCase(CSlot::s_Slot) == 0)
				{
					CString	buf;
					CSlot*	Slot = new CSlot();
					Slot->m_pUM	= this;

					Slot->XMLLoad(child);

					CSlot*	Exist	= m_TabSlot.FindBlok(Slot->m_NumSLOT);

					if(Exist)
					{
						buf.Format(_T("Slot %d:%d already exist in structure")
							,Exist->m_pUM->m_NumUM
							,Exist->m_NumSLOT);

						delete Slot;
						throw ErrorXML(buf, 0);
					}
					else
					{
						m_TabSlot.Insert(Slot);
					}
				}
				//else if( (static_cast<CString>(nodeName)).CompareNoCase(CGroup::s_Group) == 0)
				//{
				//}
			}

			CComPtr<IXMLDOMNode>	next;
			child->get_nextSibling(&next);
			if(FAILED(hr))			throw ErrorXML(IDS_XML_ERRNEXTCHILD, hr);
			child	= next;
		}
	}
	catch(ErrorXML aErr)
	{
		throw aErr;
	}
}


void	CUM::XMLPostLoad()
{
	try
	{

		if(m_pParam) m_pParam->XMLLoad();

		for(list<CSlot*>::iterator i = m_TabSlot.begin(); i != m_TabSlot.end(); ++i)
		{
			(*i)->XMLPostLoad();
		}
	}
	catch(ErrorXML aErr)
	{
		throw aErr;
	}
}

/////////////////////////////////////////////////////////////////////////////
// класс перечня УМ в системе

CUMTab::CUMTab()
{
	m_pPult		= NULL;

	m_Lock = CreateEvent(NULL, FALSE, TRUE, NULL); 
}

CUMTab::~CUMTab() 
{ 
	WaitForSingleObject(m_Lock, INFINITE);

	Clear();

	CloseHandle(m_Lock);
}

void	CUMTab::Clear()
{
	for(list<CUM*>::iterator i = begin(); i != end(); ++i)
	{
		delete *i;
	}
	clear();
}

CUM* CUMTab::FindUM(int aNumUM)
{
	for(list<CUM*>::iterator i = begin(); i != end(); ++i)
	{
		if((*i)->m_NumUM == aNumUM) return *i;
	}
	return( NULL);
}

CSlot* CUMTab::FindBlok(int aNumUM, int aNumSLOT)
{
	CUM* ItUM = FindUM(aNumUM);

	if(ItUM)	return(ItUM->m_TabSlot.FindBlok(aNumSLOT));
	else		return(NULL);
}

CDev* CUMTab::FindDev(int aNumUM, int aNumSLOT, int aAdrBus)
{
	CSlot*	ItSlot	= FindBlok(aNumUM, aNumSLOT);

	if(ItSlot)	return(ItSlot->m_TabDev.FindDev(aAdrBus));
	else		return(NULL);
}

CSlot* CUMTab::FindBlokIns(int aNumUM, int aNumSLOT)
{
	CUM*	ItUM = FindUM(aNumUM);

	if(ItUM == NULL)
	{
		ItUM	= new CUM(aNumUM, NULL);
		ItUM->m_pPult = m_pPult;
		push_back(ItUM);
	}
		
	CSlot*	ItSlot	= ItUM->m_TabSlot.FindBlok(aNumSLOT);

	if(ItSlot == NULL)
	{
		if( (aNumSLOT >= 0 && aNumSLOT <= 7) || (aNumSLOT >= 10 && aNumSLOT <= 14) )
		{
			ItSlot	= new CSlot(aNumSLOT, ItUM, CSlot::tsCLB, NULL);

			ItUM->m_TabSlot.push_back( ItSlot);
			ItSlot->SetAddr();

			return(ItSlot);
		}
	}
	return(ItSlot);
}

CSlot* CUMTab::DispSlot()
{
	for(list<CUM*>::iterator i = begin(); i != end(); ++i)
	{
		CSlot* ItDisp = (*i)->DispSlot();

		if(ItDisp) return(ItDisp);
	}
	return(NULL);
}

int	CUMTab::FindLostUM()
{
	for(list<CUM*>::iterator i = begin(); i != end(); ++i)
		(*i)->m_bLink = false;

	CUM* pUM = FindUM(m_pPult->m_RootUM);

	if(pUM)	pUM->MarkLink();

	for(list<CUM*>::iterator i = begin(); i != end(); ++i)
	{
		if(!(*i)->m_bLink) return((*i)->m_NumUM);
	}

	return(-1);
}

CSlot* CUMTab::RootSlot(int aNumUM)
{
	CSlot*	ItRoot	= NULL;
	CUM*	ItUM	= FindUM(aNumUM);
	int		CountLoop = 0;
	const	int maxCount = 100;

	if(ItUM)	
	{
		//while(ItUM->m_NumUM != ItUM->m_pPult->m_RootUM) 16.01.2015
		while(ItUM->m_NumUM != ItUM->m_pPult->m_RootUM && CountLoop < maxCount)
		{
			CSlot*	ItS	 = ItUM->FindInpLink();

			if(ItS == 0)			return 0;
			if(ItS->m_pLink == 0)	return 0;

			ItRoot	= ItS->m_pLink;
			ItUM	= ItS->m_pLink->m_pUM;
			CountLoop++;
		}
		if(CountLoop >= maxCount) return 0;	// 20.01.2015
	}
	return ItRoot;
}

int	CUMTab::TotlDisp()
{
	int Total = 0;

	for(list<CUM*>::iterator i = begin(); i != end(); ++i)
	{
		Total += (*i)->TotlDisp();
	}
	return(Total);
}

bool	CUMTab::Ggs(CMes::askFromMain aAsk, int aNumUM, int aNumSLOT, int dev, DWORD aIP, DWORD aPort, int aNumWavIn,  int aNumWavOut)
{
	bool	rc = false;

	CSlot*	ItSlot	= FindBlok(aNumUM, aNumSLOT);

	if(ItSlot != NULL)
	{
		rc = ItSlot->Ggs(aAsk, dev, aIP, aPort, aNumWavIn,  aNumWavOut);
	}
	return(rc);
}

void CUMTab::GetSost()
{
	for(list<CUM*>::iterator i = begin(); i != end(); ++i)
	{
		(*i)->GetSost();
	}
}

void	CUMTab::SetAddr()
{
	for(list<CUM*>::iterator i = begin(); i != end(); ++i)
	{
		(*i)->SetAddr();
	}
}

void	CUMTab::SetAdmPult()
{
	for(list<CUM*>::iterator i = begin(); i != end(); ++i)
	{
		(*i)->SetAdmPult();
	}
}

void	CUMTab::SetOne(CSlot* ItSlot, CSlot::typSlot aTypInp)
{
	CUM*	pLinkUM		= FindUM(ItSlot->m_NumUMi);

	if(!pLinkUM)
	{
		push_back( pLinkUM = new CUM(ItSlot->m_NumUMi, NULL));
		pLinkUM->m_pPult = m_pPult;
	}

	CSlot*	pLinkSlot	= pLinkUM->m_TabSlot.FindBlok(ItSlot->m_NumSLOTi);

	if(pLinkSlot)
	{
		pLinkSlot->m_TypSlot = aTypInp;
	}
	else
	{
		pLinkSlot = new CSlot(ItSlot->m_NumSLOTi, pLinkUM, aTypInp, NULL);
		pLinkUM->m_TabSlot.push_back( pLinkSlot );
	}
	pLinkSlot->m_pLink	= ItSlot;
	ItSlot->m_pLink		= pLinkSlot;
}

void	CUMTab::SetLink(CSlot* ItSlot, CSlot::typSlot aTypInp)
{
	for(list<CLinkNet*>::iterator i = ItSlot->m_TabLinkNet.begin(); i != ItSlot->m_TabLinkNet.end(); ++i)
	{
		CLinkNet*	ItLink		= *i;
		CUM*		pLinkUM		= FindUM(ItLink->m_NumUMi);

		if(!pLinkUM)
		{
			push_back( pLinkUM = new CUM(ItLink->m_NumUMi, NULL));
			pLinkUM->m_pPult = m_pPult;
		}

		CSlot*		pLinkSlot	= pLinkUM->m_TabSlot.FindBlok(ItLink->m_NumSLOTi);

		if(pLinkSlot)
		{
			pLinkSlot->m_TypSlot = aTypInp;
		}
		else
		{
			pLinkSlot = new CSlot(ItLink->m_NumSLOTi, pLinkUM, aTypInp, NULL);
			pLinkUM->m_TabSlot.push_back( pLinkSlot );
		}
		pLinkSlot->m_pLink		= ItSlot;
		pLinkSlot->m_NumUMi		= ItSlot->m_pUM->m_NumUM;
		pLinkSlot->m_NumSLOTi	= ItSlot->m_NumSLOT;

		ItLink->m_pLink = pLinkSlot;
	}
}

int	CUMTab::SetLink()
{
	for(list<CUM*>::iterator i = begin(); i != end(); ++i)
	{
		CUM *ItUM = *i;

		for(list<CSlot*>::iterator j = ItUM->m_TabSlot.begin(); j != ItUM->m_TabSlot.end(); ++j)
		{
			CSlot *ItSlot = *j;

			if(ItSlot->m_TypSlot == CSlot::tsCLLo)
			{
				SetOne(ItSlot, CSlot::tsCLLi);
			}
			else if(ItSlot->m_TypSlot == CSlot::tsBRDGo)
			{
				SetOne(ItSlot, CSlot::tsBRDGi);
			}
			else if(ItSlot->m_TypSlot == CSlot::tsCRCo)
			{
				SetLink(ItSlot, CSlot::tsCRCi);
			}
			else if(ItSlot->m_TypSlot == CSlot::tsGSMo)
			{
				SetLink(ItSlot, CSlot::tsGSMi);
			}
			else if(ItSlot->m_TypSlot == CSlot::tsNETo)
			{
				SetLink(ItSlot, CSlot::tsNETi);
			}
			else if(ItSlot->m_TypSlot == CSlot::tsCLLEo)
			{
				SetLink(ItSlot, CSlot::tsCLLEi);
			}
			else if(ItSlot->m_TypSlot == CSlot::tsCLLMo)
			{
				SetLink(ItSlot, CSlot::tsCLLMi);
			}
			else if(ItSlot->m_TypSlot == CSlot::tsCLLRo)
			{
				SetLink(ItSlot, CSlot::tsCLLRi);
			}
		}
	}
	
	CSlot* Slot = m_pPult->m_TabUM.DispSlot();

	if(Slot != NULL)
	{
		m_pPult->m_RootUM	= Slot->m_pUM->m_NumUM;
		m_pPult->m_RootSLOT	= Slot->m_NumSLOT;

		SetAddr();
	}
	return(1);

}


void	CUMTab::RenameUM(int aOldUM, int aNewUM)
{
	for(list<CUM*>::iterator i = begin(); i != end(); ++i)
	{
		CUM *ItUM = *i;

		for(list<CSlot*>::iterator j = ItUM->m_TabSlot.begin(); j != ItUM->m_TabSlot.end(); ++j)
		{
			CSlot *ItSlot = *j;

			if(ItSlot->m_NumUMi == aOldUM) ItSlot->m_NumUMi = aNewUM;

			if(ItSlot->IsLine())
			{
				if(ItSlot->m_NumUMi == aOldUM) ItSlot->m_NumUMi = aNewUM;
			}
			else if(ItSlot->IsEther())
			{
				for(list<CLinkNet*>::iterator k = ItSlot->m_TabLinkNet.begin(); k != ItSlot->m_TabLinkNet.end(); ++k)
				{
					CLinkNet*	ItLink	= *k;

					if(ItLink->m_NumUMi == aOldUM) 
					{
						ItLink->m_NumUMi = aNewUM;
						ItLink->XMLUpdate();
					}
				}
			}
		}
	}
}

void	CUMTab::Wait()
{
	WaitForSingleObject(m_Lock, INFINITE);		// ожидание освобождения структуры для доступа
}

void	CUMTab::Release()
{
	SetEvent(m_Lock);							// освобождение структуры для доступа
}


bool	CUMTab::Insert(CUM* aUm)
{
	if(FindUM(aUm->m_NumUM) == NULL)
	{
		push_back(aUm);
		return true;
	}
	else
	{
		return false;
	}
}

bool	CUMTab::IsNoAdmin(int aNumUM)
{
	CUM*	UM = FindUM(aNumUM);

	if(UM) return UM->m_bNoAdmin;

	return true;
}

/////////////////////////////////////////////////////////////////////////////
// сравниватель <

struct LTpUM : public binary_function<CUM*, CUM*, bool>
{ 
	bool operator()(const CUM* _Left, const CUM* _Right) const { return (_Left->m_NumUM < _Right->m_NumUM); }
};

void	CUMTab::ReOrder()	
{ 
	sort(LTpUM());

	for(iterator i = begin(); i != end(); ++i)	(*i)->ReOrder();
}

