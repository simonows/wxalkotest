#include "stdafx.h"

#include <pult.h>
#include <Xml.h>
#include <Disp.h>
#include "LoadRes.h"
#include "resource.h"

const	TCHAR* CPult::s_Pult		= _T("Pult");
const	TCHAR* CPult::s_Number		= _T("Number");
const	TCHAR* CPult::s_Name		= _T("Name");
const	TCHAR* CPult::s_Default		= _T("Default");

/////////////////////////////////////////////////////////////////////////////
// класс диспетчерского пульта, вместе с подключенными к нему УМ
//

CPult::CPult(int aNumPult)
	: m_Number(aNumPult)
{
	m_RootUM	= 0;
	m_RootSLOT	= 0;
	m_bCross	= true;
	m_sNaim.Empty();
	m_pParam	= NULL;
	m_pDisp		= 0;
	m_bIsDefault= false;

	m_TabUM.m_pPult = this;
}

CPult::~CPult()
{
	if(m_pParam != NULL) delete m_pParam;
}

void CPult::InitEmpty(int aUMDisp, int aSlotDisp)
{
	if(m_TabUM.size() == 0)
	{
		CUM*	ItUM   = new CUM(aUMDisp, NULL);

		CSlot*	ItSlot = new CSlot(aSlotDisp, ItUM, CSlot::tsMon, NULL);
		ItUM->m_TabSlot.push_back( ItSlot );

		m_TabUM.push_back( ItUM );

		m_sNaim = _S_(IDS_DEF_NAME_DISP);
	}

	m_TabUM.m_pPult = this;

	for(list<CUM*>::iterator i = m_TabUM.begin(); i != m_TabUM.end(); ++i)
	{
		(*i)->m_pPult = this;
	}

	m_TabUM.SetLink();
}

void CPult::ReLink()
{
	for(list<CUM*>::iterator i = m_TabUM.begin(); i != m_TabUM.end(); ++i)
	{
		(*i)->m_bLink = false;
	}

	CUM* pDisp = m_TabUM.FindUM(m_RootUM);

	if(pDisp) pDisp->ReLink();
}

void	CPult::XMLRemove()
{
	if(m_pDisp)
	{
		CComPtr<IXMLDOMNode>	spOldNode;

		m_pDisp->m_XMLNode->removeChild(m_XMLNode, &spOldNode);
	}
}

void	CPult::XMLUpdate()
{
	try
	{
		HRESULT	hr;
		CString	buf;

		CComQIPtr<IXMLDOMElement>	spElem	= m_XMLNode;
		if(!spElem)					throw ErrorXML(_T("Error query interfaces"), 0, s_Pult);

		hr	= spElem->setAttribute(CComBSTR(s_Number), CComVariant(m_Number));
		if(FAILED(hr))				throw ErrorXML(_T("Error addition"), hr, s_Pult, s_Number);

		hr	= spElem->setAttribute(CComBSTR(s_Name), CComVariant(m_sNaim));
		if(FAILED(hr))				throw ErrorXML(_T("Error addition"), hr, s_Pult, s_Name);

		hr	= spElem->setAttribute(CComBSTR(s_Default), CComVariant(m_bIsDefault?1:0));
		if(FAILED(hr))				throw ErrorXML(_T("Error addition"), hr, s_Pult, s_Default);

		if(m_pParam) m_pParam->XMLUpdate();
	}
	catch(ErrorXML aErr)
	{
		throw aErr;
	}
}

void	CPult::XMLAdd()
{
	try
	{
		HRESULT	hr;

		CComPtr<IXMLDOMNode>	spNewNode;
		hr	= m_pDisp->m_XMLDOM->createNode(CComVariant(NODE_ELEMENT),
											CComBSTR(s_Pult),
											NULL,
											&spNewNode);
		if(FAILED(hr))				throw ErrorXML(_T("Error create"), hr, s_Pult);

		hr	= m_pDisp->m_XMLNode->appendChild(spNewNode, &m_XMLNode);
		if(FAILED(hr))				throw ErrorXML(_T("Error addition"), hr, s_Pult);
		if(!m_XMLNode)				throw ErrorXML(_T("Error addition"), hr, s_Pult);

		XMLUpdate();
	}
	catch(ErrorXML aErr)
	{
		throw aErr;
	}
}

void	CPult::XMLLoad(CComPtr<IXMLDOMNode> spNode)
{
	try
	{
		m_XMLNode	= spNode;	

		CComQIPtr<IXMLDOMElement>	spElem	= spNode;
		if(!spElem)				throw ErrorXML(_T("Error query interfaces"), 0, s_Pult);

		HRESULT		hr;
		CComVariant	value;

		value.Clear();
		hr	= spElem->getAttribute(CComBSTR(s_Number),		&value);
		if(FAILED(hr))				throw ErrorXML(_T("Error read"), hr, s_Pult, s_Number);
		if(value.vt != VT_NULL)
		{
			hr	= value.ChangeType(VT_I4);
			if(FAILED(hr))			throw ErrorXML(_T("Error read"), hr, s_Pult, s_Number);
			m_Number	= value.lVal;
		}

		value.Clear();
		hr = spElem->getAttribute(CComBSTR(s_Name), &value);
		if(FAILED(hr))			throw ErrorXML(_T("Error read"), hr, s_Pult, s_Name);
		if(value.vt != VT_NULL)
		{
			m_sNaim = value; 
			m_sNaim.Trim();
		}

		value.Clear();
		hr	= spElem->getAttribute(CComBSTR(s_Default),		&value);
		if(FAILED(hr))				throw ErrorXML(_T("Error read"), hr, s_Pult, s_Default);
		if(value.vt != VT_NULL)
		{
			hr	= value.ChangeType(VT_I4);
			if(FAILED(hr))			throw ErrorXML(_T("Error read"), hr, s_Pult, s_Default);
			m_bIsDefault	= value.lVal != 0;
		}

		CComPtr<IXMLDOMNode>	child;
		hr = spNode->get_firstChild(&child);
		if(FAILED(hr))				throw ErrorXML(_T("CPult::XMLLoad get_firstChild"), hr);

		while(!(!child))
		{
			DOMNodeType	tp;

			child->get_nodeType(&tp);

			if (tp == NODE_ELEMENT)
			{
				CComBSTR	nodeName;

				child->get_nodeName(&nodeName);

				if( (static_cast<CString>(nodeName)).CompareNoCase(CUM::s_Um) == 0)
				{
					CString	buf;
					CUM*	Um = new CUM();
					Um->m_pPult	= this;

					Um->XMLLoad(child);

					CUM*	Exist	= m_TabUM.FindUM(Um->m_NumUM);

					if(Exist)
					{
						buf.Format(_T("Node %d already exist in structure")
							,Exist->m_NumUM);

						delete Um;
						throw ErrorXML(buf, 0);
					}
					else
					{
						m_TabUM.Insert(Um);
					}
				}
				//else if( (static_cast<CString>(nodeName)).CompareNoCase(CGroup::s_Group) == 0)
				//{
				//}
			}

			CComPtr<IXMLDOMNode>	next;
			child->get_nextSibling(&next);
			if(FAILED(hr))			throw ErrorXML(_T("CPult::XMLLoad get_nextSibling"), hr);
			child	= next;
		}
	}
	catch(ErrorXML aErr)
	{
		throw aErr;
	}
}

void	CPult::XMLPostLoad()
{
	try
	{

		if(m_pParam) m_pParam->XMLLoad();

		for(list<CUM*>::iterator i = m_TabUM.begin(); i != m_TabUM.end(); ++i)
		{
			(*i)->XMLPostLoad();
		}
	}
	catch(ErrorXML aErr)
	{
		throw aErr;
	}
}


void	CPult::XMLRelease()
{
	for(list<CUM*>::iterator i = m_TabUM.begin(); i != m_TabUM.end(); ++i)
	{
		CUM*	Um	= *i;

		for(list<CSlot*>::iterator j = Um->m_TabSlot.begin(); j != Um->m_TabSlot.end(); ++j)
		{
			CSlot*	Slot = *j;

			for(list<CDev*>::iterator k = Slot->m_TabDev.begin(); k != Slot->m_TabDev.end(); ++k)
			{
				CDev* Dev = *k;

				if(!(!Dev->m_XMLNode)) Dev->m_XMLNode.Release();
			}
			Slot->m_TabLinkNet.XMLRelease();
			Slot->m_TabLinkCLL.XMLRelease();
			if(!(!Slot->m_XMLNode)) Slot->m_XMLNode.Release();
		}
		if(!(!Um->m_XMLNode)) Um->m_XMLNode.Release();
	}
	if(!(!m_XMLNode)) m_XMLNode.Release();
}

bool	CPult::IsModify()
{
	if(m_FlagModify) return true;

	for(list<CUM*>::iterator i = m_TabUM.begin(); i != m_TabUM.end(); ++i)
	{
		CUM*	Um	= *i;

		if(Um->m_FlagModify) return true;

		for(list<CSlot*>::iterator j = Um->m_TabSlot.begin(); j != Um->m_TabSlot.end(); ++j)
		{
			CSlot*	Slot = *j;

			if(Slot->m_FlagModify) return true;

			for(list<CDev*>::iterator k = Slot->m_TabDev.begin(); k != Slot->m_TabDev.end(); ++k)
			{
				CDev* Dev = *k;

				if(Dev->m_FlagModify) return true;
			}
		}
	}
	return false;
}

void	CPult::ClearModify()
{
	m_FlagModify = false;

	for(list<CUM*>::iterator i = m_TabUM.begin(); i != m_TabUM.end(); ++i)
	{
		CUM*	Um	= *i;

		Um->m_FlagModify = false;

		for(list<CSlot*>::iterator j = Um->m_TabSlot.begin(); j != Um->m_TabSlot.end(); ++j)
		{
			CSlot*	Slot = *j;

			Slot->m_FlagModify = false;

			for(list<CDev*>::iterator k = Slot->m_TabDev.begin(); k != Slot->m_TabDev.end(); ++k)
			{
				CDev* Dev = *k;

				Dev->m_FlagModify = false;
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// все пульты (подсети)

CPult*	CPultTab::FindPult(int aNumber)
{
	for(list<CPult*>::iterator i = begin(); i != end(); ++i)
	{
		if((*i)->m_Number == aNumber) return *i;
	}
	return NULL;
}

bool	CPultTab::Insert(CPult* aPult)
{
	if(FindPult(aPult->m_Number) == NULL)
	{
		push_back(aPult);
		return true;
	}
	else
	{
		return false;
	}
}

void	CPultTab::Clear()
{
	for(list<CPult*>::iterator i = begin(); i != end(); ++i)
	{
		delete *i;
	}
	clear();
}

/////////////////////////////////////////////////////////////////////////////
// сравниватель <

struct LTpPult : public binary_function<CPult*, CPult*, bool>
{ 
	bool operator()(const CPult* _Left, const CPult* _Right) const { return (_Left->m_Number < _Right->m_Number); }
};

void	CPultTab::ReOrder()	
{ 
	sort(LTpPult());

	for(iterator i = begin(); i != end(); ++i)	(*i)->ReOrder();
}
