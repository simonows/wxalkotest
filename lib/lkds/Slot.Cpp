#include "stdafx.h"

#include <slot.h>
#include <Um.h>
#include <Disp.h>
#include <Xml.h>
#include <Idea.h>
#include "LoadRes.h"
#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// класс MAC адреса

CAtlString	CMacAddr::GetString()
{
	CAtlString	buf;
	buf.Format(_T("%02X%02X%02X%02X%02X%02X"), f0&0xFF, f1&0xFF, f2&0xFF, f3&0xFF, f4&0xFF, f5&0xFF);

	return buf;
}

bool	CMacAddr::SetString(const TCHAR* aStr)
{
	bool	rc	= false;
	size_t	Len = _tcslen(aStr);

	if(Len == 12)
	{
		size_t	i;
		for(i = 0; i < Len; i++)
		{
			if(!_tcschr(_T("0123456789abcdefABCDEF"), aStr[i])) break;
		}

		if(i == Len)
		{
			Clear();

			int	F0=0,F1=0,F2=0,F3=0,F4=0,F5=0;

#if defined(WIN32_PLATFORM_PSPC) || defined(WIN32_PLATFORM_WFSP)
			_stscanf(aStr, _T("%02X%02X%02X%02X%02X%02X"), &F0, &F1, &F2, &F3, &F4, &F5);
#else
			_stscanf_s(aStr, _T("%02X%02X%02X%02X%02X%02X"), &F0, &F1, &F2, &F3, &F4, &F5);
#endif

			f0 = F0&0xFF;
			f1 = F1&0xFF;
			f2 = F2&0xFF;
			f3 = F3&0xFF;
			f4 = F4&0xFF;
			f5 = F5&0xFF;

			rc	= true;
		}
	}
	return rc;
}

/////////////////////////////////////////////////////////////////////////////
// класс IP адреса

bool	CIpAddr::IsValid()
{
	if(f0 > 0 && f0 < 255 && f3 > 0 && f3 < 255)	return true;
	else											return false;
}

bool	CIpAddr::IsNul()
{
	return f0==0 && f1==0 && f2==0 && f3==0;
}

CAtlString	CIpAddr::GetString()
{
	CAtlString	buf;
	buf.Format(_T("%d.%d.%d.%d"), f0&0xFF, f1&0xFF, f2&0xFF, f3&0xFF);

	return buf;
}

bool	CIpAddr::SetString(const TCHAR* aStr)
{
	bool	rc	= false;

	Clear();

	int	F0=0,F1=0,F2=0,F3=0;

#if defined(WIN32_PLATFORM_PSPC) || defined(WIN32_PLATFORM_WFSP)
	_stscanf(aStr, _T("%d.%d.%d.%d"), &F0, &F1, &F2, &F3);
#else
	_stscanf_s(aStr, _T("%d.%d.%d.%d"), &F0, &F1, &F2, &F3);
#endif

	f0 = F0&0xFF;
	f1 = F1&0xFF;
	f2 = F2&0xFF;
	f3 = F3&0xFF;

	rc	= true;

	return rc;
}

DWORD	CIpAddr::GetValue()
{
	union
	{
		DWORD	lValue;
		BYTE	bValue[4];
	} Value;

	Value.bValue[0]	= f0;
	Value.bValue[1]	= f1;
	Value.bValue[2]	= f2;
	Value.bValue[3]	= f3;

	return Value.lValue;
}

DWORD	CIpAddr::GetValueW()
{
	union
	{
		DWORD	lValue;
		BYTE	bValue[4];
	} Value;

	Value.bValue[0]	= f3;
	Value.bValue[1]	= f2;
	Value.bValue[2]	= f1;
	Value.bValue[3]	= f0;

	return Value.lValue;
}

bool	CIpAddr::SetValue(DWORD aValue)
{
	union
	{
		DWORD	lValue;
		BYTE	bValue[4];
	} Value;

	Value.lValue = aValue;

	f0 = Value.bValue[0];
	f1 = Value.bValue[1];
	f2 = Value.bValue[2];
	f3 = Value.bValue[3];

	return true;
}

bool	CIpAddr::SetValueW(DWORD aValue)
{
	union
	{
		DWORD	lValue;
		BYTE	bValue[4];
	} Value;

	Value.lValue = aValue;

	f3 = Value.bValue[0];
	f2 = Value.bValue[1];
	f1 = Value.bValue[2];
	f0 = Value.bValue[3];

	return true;
}

bool CIpAddr::TestModifyIP(DWORD IP, int iField, int iValue)
{
	return (((IP>>((3-iField)*8)))&0xFF) != iValue;
}

/////////////////////////////////////////////////////////////////////////////
// класс IP порта

unsigned int CIpPort::GetValue()
{
	return( (h<<8) | l );
}

bool	CIpPort::SetValue(unsigned int aValue)
{
	l = aValue&0xFF;
	h = (aValue>>8)&0xFF;

	return true;
}

bool	CIpPort::operator == (CIpPort& p)
{
	return(l==p.l && h==p.h);
}

/////////////////////////////////////////////////////////////////////////////
// класс логической исходящей связи КСЛ
//

const	TCHAR*	CLinkNet::s_LinkNet	= _T("LinkNet");
const	TCHAR*	CLinkNet::s_UM		= _T("UM");
const	TCHAR*	CLinkNet::s_Slot	= _T("Slot");

CLinkNet::CLinkNet(CLinkNetTab* aTabLinkNet, CParam* aParam) 
	: m_pParam(aParam) 
	, m_pTabLinkNet(aTabLinkNet)
{
	Clear();
}

CLinkNet::CLinkNet( CLinkNet &aLink) : m_pParam(NULL)
{
	Clear();

	m_NumUMi		= aLink.m_NumUMi;
	m_NumSLOTi		= aLink.m_NumSLOTi;
	m_pTabLinkNet	= aLink.m_pTabLinkNet;
}

CLinkNet::~CLinkNet()
{
	if(m_pParam != NULL) delete m_pParam;
}
void	CLinkNet::Clear()
{
	m_NumUMi		= 0;
	m_NumSLOTi		= 0;
	m_pLink			= NULL;
	m_FlagModify	= false;
}

void	CLinkNet::XMLRemove()
{
	if(m_pTabLinkNet)
	{
		CComPtr<IXMLDOMNode>	spOldNode;

		HRESULT	hr = m_pTabLinkNet->m_XMLNode->removeChild(m_XMLNode, &spOldNode);
	}
}

void	CLinkNet::XMLUpdate()
{
	HRESULT	hr;
	CString	buf;

	CComQIPtr<IXMLDOMElement>	spElem	= m_XMLNode;
	if(!spElem)					throw ErrorXML(IDS_XML_ERRQI, 0, s_LinkNet);

	hr	= spElem->setAttribute(CComBSTR(s_UM), CComVariant(m_NumUMi));
	if(FAILED(hr))				throw ErrorXML(IDS_XML_ERRAPPEND, hr, s_LinkNet, s_UM);

	hr	= spElem->setAttribute(CComBSTR(s_Slot), CComVariant(m_NumSLOTi));
	if(FAILED(hr))				throw ErrorXML(IDS_XML_ERRAPPEND, hr, s_LinkNet, s_Slot);

	if(m_pParam) m_pParam->XMLUpdate();
}

void	CLinkNet::XMLAdd()
{
	if(m_pTabLinkNet)
	{
		HRESULT	hr;

		CComPtr<IXMLDOMNode>	spNewNode;

		//hr	= m_pTabLinkNet->m_pSlot->m_pUM->m_pPult->m_XMLDOM->createNode(	CComVariant(NODE_ELEMENT),
		hr	= m_pTabLinkNet->m_pSlot->m_pUM->m_pPult->m_pDisp->m_XMLDOM->createNode(	CComVariant(NODE_ELEMENT),
																						CComBSTR(s_LinkNet),
																						NULL,
																						&spNewNode);
		if(FAILED(hr))				throw ErrorXML(IDS_XML_ERRCREATE, hr, s_LinkNet);

		hr	= m_pTabLinkNet->m_XMLNode->appendChild(spNewNode, &m_XMLNode);
		if(FAILED(hr))				throw ErrorXML(IDS_XML_ERRAPPEND, hr, s_LinkNet);
		if(!m_XMLNode)				throw ErrorXML(IDS_XML_ERRAPPEND, hr, s_LinkNet);

		XMLUpdate();
	}
}

void	CLinkNet::XMLLoad(CComPtr<IXMLDOMNode> spNode)
{
	m_XMLNode	= spNode;	

	CComQIPtr<IXMLDOMElement>	spElem	= spNode;
	if(!spElem)				throw ErrorXML(IDS_XML_ERRQI, 0, s_LinkNet);

	HRESULT		hr;
	CComVariant	value;

	value.Clear();
	hr	= spElem->getAttribute(CComBSTR(s_UM),		&value);
	if(FAILED(hr))				throw ErrorXML(IDS_XML_ERRREAD, hr, s_LinkNet, s_UM);
	if(value.vt != VT_NULL)
	{
		hr	= value.ChangeType(VT_I4);
		if(FAILED(hr))			throw ErrorXML(IDS_XML_ERRREAD, hr, s_LinkNet, s_UM);
		m_NumUMi		= value.lVal;
	}

	value.Clear();
	hr	= spElem->getAttribute(CComBSTR(s_Slot),		&value);
	if(FAILED(hr))				throw ErrorXML(IDS_XML_ERRREAD, hr, s_LinkNet, s_Slot);
	if(value.vt != VT_NULL)
	{
		hr	= value.ChangeType(VT_I4);
		if(FAILED(hr))			throw ErrorXML(IDS_XML_ERRREAD, hr, s_LinkNet, s_Slot);
		m_NumSLOTi		= value.lVal;
	}
}

void	CLinkNet::XMLPostLoad()
{
	if(m_pParam) m_pParam->XMLLoad();
}

void	CLinkNet::XMLRelease()
{
	if(!(!m_XMLNode)) m_XMLNode.Release();
}


/////////////////////////////////////////////////////////////////////////////
// сравниватель <

struct LTpLinkNet : public binary_function<CLinkNet*, CLinkNet*, bool>
{ 
	bool operator()(const CLinkNet* _Left, const CLinkNet* _Right) const { return _Left->m_NumUMi < _Right->m_NumUMi; };
};

/////////////////////////////////////////////////////////////////////////////
// класс таблицы логических исходящих связей КСЛ
//

const	TCHAR*	CLinkNetTab::s_TabLinkNet = _T("TabLinkNet");

void	CLinkNetTab::ReOrder()	
{ 
	sort(LTpLinkNet());
}

bool	CLinkNetTab::Insert(int aNumUMi, int aNumSLOTi, CSlot*	aLink, CParam* aParam)
{
	for(iterator i = begin(); i != end(); ++i)
	{
		CLinkNet *It = *i;

		if( (It->m_NumUMi == aNumUMi && It->m_NumSLOTi == aNumSLOTi) || (It->m_pLink == aLink && aLink != NULL) ) return false;
	}

	CLinkNet *ItNew = new CLinkNet(this, aParam);

	ItNew->m_NumUMi		= aNumUMi;
	ItNew->m_NumSLOTi	= aNumSLOTi;
	ItNew->m_pLink		= aLink;

	push_back( ItNew );

	return(true);
}

CLinkNet*	CLinkNetTab::FindLink(int aNumUMi, int aNumSLOTi)
{
	for(iterator i = begin(); i != end(); ++i)
	{
		if( (*i)->m_NumUMi == aNumUMi && (*i)->m_NumSLOTi == aNumSLOTi ) return *i;
	}
	return 0;
}

CLinkNet*	CLinkNetTab::FindLink(CSlot*	aLink)
{
	for(list<CLinkNet*>::iterator i = begin(); i != end(); ++i)
	{
		CLinkNet *It = *i;

		if(It->m_pLink == aLink) return It;
	}

	return(0);
}

void	CLinkNetTab::XMLRemove()
{
	if(m_pSlot)
	{
		CComPtr<IXMLDOMNode>	spOldNode;

		m_pSlot->m_XMLNode->removeChild(m_XMLNode, &spOldNode);
	}
}

void	CLinkNetTab::XMLUpdate()
{
	CComQIPtr<IXMLDOMElement>	spElem	= m_XMLNode;
	if(!spElem)					throw ErrorXML(IDS_XML_ERRQI, 0, s_TabLinkNet);
}

void	CLinkNetTab::XMLAdd()
{
	if(m_pSlot && (!m_XMLNode))
	{
		HRESULT	hr;

		CComPtr<IXMLDOMNode>	spNewNode;

		//hr	= m_pSlot->m_pUM->m_pPult->m_XMLDOM->createNode(	CComVariant(NODE_ELEMENT),
		hr	= m_pSlot->m_pUM->m_pPult->m_pDisp->m_XMLDOM->createNode(	CComVariant(NODE_ELEMENT),
																		CComBSTR(s_TabLinkNet),
																		NULL,
																		&spNewNode);
		if(FAILED(hr))				throw ErrorXML(IDS_XML_ERRCREATE, hr, s_TabLinkNet);

		hr	= m_pSlot->m_XMLNode->appendChild(spNewNode, &m_XMLNode);
		if(FAILED(hr))				throw ErrorXML(IDS_XML_ERRAPPEND, hr, s_TabLinkNet);
		if(!m_XMLNode)				throw ErrorXML(IDS_XML_ERRAPPEND, hr, s_TabLinkNet);

		XMLUpdate();
	}
}

void	CLinkNetTab::XMLLoad(CComPtr<IXMLDOMNode> spNode)
{
	m_XMLNode	= spNode;	

	CComQIPtr<IXMLDOMElement>	spElem	= spNode;
	if(!spElem)				throw ErrorXML(IDS_XML_ERRQI, 0, s_TabLinkNet);

	HRESULT		hr;
	CComVariant	value;

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

			if( (static_cast<CString>(nodeName)).CompareNoCase(CLinkNet::s_LinkNet) == 0)
			{
				CString		buf;
				CLinkNet*	Link	= new CLinkNet(this, NULL);

				Link->XMLLoad(child);

				if(!FindLink(Link->m_NumUMi, Link->m_NumSLOTi))
				{
					push_back( Link );
				}
				else
				{
					buf.Format(_T("Link(%d:%d)->(%d:%d) already exist in structure")
						,m_pSlot->m_pUM->m_NumUM
						,m_pSlot->m_NumSLOT
						,Link->m_NumSLOTi
						,Link->m_NumUMi);

					delete Link;
					throw ErrorXML(buf, 0);
				}
			}
		}

		CComPtr<IXMLDOMNode>	next;
		child->get_nextSibling(&next);
		if(FAILED(hr))			throw ErrorXML(IDS_XML_ERRNEXTCHILD, hr);
		child	= next;
	}
}

void	CLinkNetTab::XMLRelease()
{
	for(list<CLinkNet*>::iterator i = begin(); i != end(); ++i)
	{
		(*i)->XMLRelease();
	}
	if(!(!m_XMLNode)) m_XMLNode.Release();
}

/////////////////////////////////////////////////////////////////////////////
// класс исходящей связи КСЛ

const	TCHAR*	CLinkCLL::s_LinkCLL		= _T("LinkCLL");
const	TCHAR*	CLinkCLL::s_Number		= _T("Number");
const	TCHAR*	CLinkCLL::s_UM			= _T("UM");
const	TCHAR*	CLinkCLL::s_Slot		= _T("Slot");
const	TCHAR*	CLinkCLL::s_TimeLife	= _T("TimeLife");
const	TCHAR*	CLinkCLL::s_TypModem	= _T("TypModem");
const	TCHAR*	CLinkCLL::s_TimeLink	= _T("TimeLink");
const	TCHAR*	CLinkCLL::s_Fone		= _T("Fone");
const	TCHAR*	CLinkCLL::s_IP			= _T("IP");
const	TCHAR*	CLinkCLL::s_IPDop1		= _T("IPDop1");
const	TCHAR*	CLinkCLL::s_IPDop2		= _T("IPDop2");
const	TCHAR*	CLinkCLL::s_Mask		= _T("Mask");
const	TCHAR*	CLinkCLL::s_GTW			= _T("GTW");
const	TCHAR*	CLinkCLL::s_MAC			= _T("MAC");
const	TCHAR*	CLinkCLL::s_DataPort	= _T("DataPort");
const	TCHAR*	CLinkCLL::s_VoicePort	= _T("VoicePort");
const	TCHAR*  CLinkCLL::s_Note		= _T("Note");

CLinkCLL::CLinkCLL(int aNumber, CLinkCLLTab* aTabLinkCLL)
{
	m_Number		= aNumber;
	m_TimeLife		= 0xFF;
	m_NumUM			= 0xFF;
	m_NumSLOT		= 0xFF;
	m_TypModem		= 0xFF;
	m_Rez1			= 0xFF;
	m_TimeLink		= 0xFF;
	m_Rez2			= 0xFF;
	m_Fone.Empty();
	m_IsKnov		= false;
	m_IsModify		= false;
	m_IsLink		= false;
	m_pParam		= NULL;
	m_pTabLinkCLL	= aTabLinkCLL;
}

CLinkCLL::~CLinkCLL()
{
	if(m_pParam) delete m_pParam;
}

void	CLinkCLL::XMLRemove()
{
	if(m_pTabLinkCLL)
	{
		CComPtr<IXMLDOMNode>	spOldNode;

		m_pTabLinkCLL->m_XMLNode->removeChild(m_XMLNode, &spOldNode);
	}
}

void	CLinkCLL::XMLUpdate()
{
	HRESULT	hr;
	CString	buf;

	CComQIPtr<IXMLDOMElement>	spElem	= m_XMLNode;
	if(!spElem)					throw ErrorXML(IDS_XML_ERRQI, 0, s_LinkCLL);

	hr	= spElem->setAttribute(CComBSTR(s_Number), CComVariant(m_Number));
	if(FAILED(hr))				throw ErrorXML(IDS_XML_ERRAPPEND, hr, s_LinkCLL, s_Number);

	hr	= spElem->setAttribute(CComBSTR(s_UM), CComVariant(m_NumUM));
	if(FAILED(hr))				throw ErrorXML(IDS_XML_ERRAPPEND, hr, s_LinkCLL, s_UM);

	hr	= spElem->setAttribute(CComBSTR(s_Slot), CComVariant(m_NumSLOT));
	if(FAILED(hr))				throw ErrorXML(IDS_XML_ERRAPPEND, hr, s_LinkCLL, s_Slot);

	hr	= spElem->setAttribute(CComBSTR(s_TimeLife), CComVariant(m_TimeLife));
	if(FAILED(hr))				throw ErrorXML(IDS_XML_ERRAPPEND, hr, s_LinkCLL, s_TimeLife);

	hr	= spElem->setAttribute(CComBSTR(s_TypModem), CComVariant(m_TypModem));
	if(FAILED(hr))				throw ErrorXML(IDS_XML_ERRAPPEND, hr, s_LinkCLL, s_TypModem);

	hr	= spElem->setAttribute(CComBSTR(s_TimeLink), CComVariant(m_TimeLink));
	if(FAILED(hr))				throw ErrorXML(IDS_XML_ERRAPPEND, hr, s_LinkCLL, s_TimeLink);

	hr	= spElem->setAttribute(CComBSTR(s_Fone), CComVariant(m_Fone));
	if(FAILED(hr))				throw ErrorXML(IDS_XML_ERRAPPEND, hr, s_LinkCLL, s_Fone);

	hr	= spElem->setAttribute(CComBSTR(s_IP), CComVariant(m_IP.GetString()));
	if(FAILED(hr))				throw ErrorXML(IDS_XML_ERRAPPEND, hr, s_LinkCLL, s_IP);

	hr	= spElem->setAttribute(CComBSTR(s_IPDop1), CComVariant(m_IPDop[0].GetString()));
	if(FAILED(hr))				throw ErrorXML(IDS_XML_ERRAPPEND, hr, s_LinkCLL, s_IPDop1);

	hr	= spElem->setAttribute(CComBSTR(s_IPDop2), CComVariant(m_IPDop[1].GetString()));
	if(FAILED(hr))				throw ErrorXML(IDS_XML_ERRAPPEND, hr, s_LinkCLL, s_IPDop2);

	hr	= spElem->setAttribute(CComBSTR(s_Mask), CComVariant(m_Mask.GetString()));
	if(FAILED(hr))				throw ErrorXML(IDS_XML_ERRAPPEND, hr, s_LinkCLL, s_Mask);

	hr	= spElem->setAttribute(CComBSTR(s_GTW), CComVariant(m_GTW.GetString()));
	if(FAILED(hr))				throw ErrorXML(IDS_XML_ERRAPPEND, hr, s_LinkCLL, s_GTW);

	hr	= spElem->setAttribute(CComBSTR(s_MAC), CComVariant(m_MAC.GetString()));
	if(FAILED(hr))				throw ErrorXML(IDS_XML_ERRAPPEND, hr, s_LinkCLL, s_MAC);

	hr	= spElem->setAttribute(CComBSTR(s_DataPort), CComVariant(m_DataPort.GetValue()));
	if(FAILED(hr))				throw ErrorXML(IDS_XML_ERRAPPEND, hr, s_LinkCLL, s_DataPort);

	hr	= spElem->setAttribute(CComBSTR(s_VoicePort), CComVariant(m_VoicePort.GetValue()));
	if(FAILED(hr))				throw ErrorXML(IDS_XML_ERRAPPEND, hr, s_LinkCLL, s_VoicePort);

	hr	= spElem->setAttribute(CComBSTR(s_Note), CComVariant(m_Note));
	if(FAILED(hr))				throw ErrorXML(IDS_XML_ERRAPPEND, hr, s_LinkCLL, s_Note);

	if(m_pParam) m_pParam->XMLUpdate();
}

void	CLinkCLL::XMLAdd()
{
	if(m_pTabLinkCLL)
	{
		HRESULT	hr;

		CComPtr<IXMLDOMNode>	spNewNode;

		hr	= m_pTabLinkCLL->m_pSlot->m_pUM->m_pPult->m_pDisp->m_XMLDOM->createNode(	CComVariant(NODE_ELEMENT),
																						CComBSTR(s_LinkCLL),
																						NULL,
																						&spNewNode);
		if(FAILED(hr))				throw ErrorXML(IDS_XML_ERRCREATE, hr, s_LinkCLL);

		hr	= m_pTabLinkCLL->m_XMLNode->appendChild(spNewNode, &m_XMLNode);
		if(FAILED(hr))				throw ErrorXML(IDS_XML_ERRAPPEND, hr, s_LinkCLL);
		if(!m_XMLNode)				throw ErrorXML(IDS_XML_ERRAPPEND, hr, s_LinkCLL);

		XMLUpdate();
	}
}

void	CLinkCLL::XMLLoad(CComPtr<IXMLDOMNode> spNode)
{
	CString	buf;
	m_XMLNode	= spNode;	

	CComQIPtr<IXMLDOMElement>	spElem	= spNode;
	if(!spElem)				throw ErrorXML(IDS_XML_ERRQI, 0, s_LinkCLL);

	HRESULT		hr;
	CComVariant	value;

	value.Clear();
	hr	= spElem->getAttribute(CComBSTR(s_Number),		&value);
	if(FAILED(hr))				throw ErrorXML(IDS_XML_ERRREAD, hr, s_LinkCLL, s_Number);
	if(value.vt != VT_NULL)
	{
		hr	= value.ChangeType(VT_I4);
		if(FAILED(hr))			throw ErrorXML(IDS_XML_ERRREAD, hr, s_LinkCLL, s_Number);
		m_Number		= value.lVal;
	}

	value.Clear();
	hr	= spElem->getAttribute(CComBSTR(s_UM),		&value);
	if(FAILED(hr))				throw ErrorXML(IDS_XML_ERRREAD, hr, s_LinkCLL, s_UM);
	if(value.vt != VT_NULL)
	{
		hr	= value.ChangeType(VT_I4);
		if(FAILED(hr))			throw ErrorXML(IDS_XML_ERRREAD, hr, s_LinkCLL, s_UM);
		m_NumUM		= value.lVal;
	}

	value.Clear();
	hr	= spElem->getAttribute(CComBSTR(s_Slot),		&value);
	if(FAILED(hr))				throw ErrorXML(IDS_XML_ERRREAD, hr, s_LinkCLL, s_Slot);
	if(value.vt != VT_NULL)
	{
		hr	= value.ChangeType(VT_I4);
		if(FAILED(hr))			throw ErrorXML(IDS_XML_ERRREAD, hr, s_LinkCLL, s_Slot);
		m_NumSLOT		= value.lVal;
	}

	value.Clear();
	hr	= spElem->getAttribute(CComBSTR(s_TimeLife),		&value);
	if(FAILED(hr))				throw ErrorXML(IDS_XML_ERRREAD, hr, s_LinkCLL, s_TimeLife);
	if(value.vt != VT_NULL)
	{
		hr	= value.ChangeType(VT_I4);
		if(FAILED(hr))			throw ErrorXML(IDS_XML_ERRREAD, hr, s_LinkCLL, s_TimeLife);
		m_TimeLife		= value.lVal;
	}

	value.Clear();
	hr	= spElem->getAttribute(CComBSTR(s_TypModem),		&value);
	if(FAILED(hr))				throw ErrorXML(IDS_XML_ERRREAD, hr, s_LinkCLL, s_TypModem);
	if(value.vt != VT_NULL)
	{
		hr	= value.ChangeType(VT_I4);
		if(FAILED(hr))			throw ErrorXML(IDS_XML_ERRREAD, hr, s_LinkCLL, s_TypModem);
		m_TypModem		= value.lVal;
	}

	value.Clear();
	hr	= spElem->getAttribute(CComBSTR(s_TimeLink),		&value);
	if(FAILED(hr))				throw ErrorXML(IDS_XML_ERRREAD, hr, s_LinkCLL, s_TimeLink);
	if(value.vt != VT_NULL)
	{
		hr	= value.ChangeType(VT_I4);
		if(FAILED(hr))			throw ErrorXML(IDS_XML_ERRREAD, hr, s_LinkCLL, s_TimeLink);
		m_TimeLink		= value.lVal;
	}

	value.Clear();
	hr	= spElem->getAttribute(CComBSTR(s_Fone),		&value);
	if(FAILED(hr))				throw ErrorXML(IDS_XML_ERRREAD, hr, s_LinkCLL, s_Fone);
	if(value.vt != VT_NULL)
	{
		m_Fone	= value;
		m_Fone.Trim();
	}

	value.Clear();
	hr	= spElem->getAttribute(CComBSTR(s_IP),		&value);
	if(FAILED(hr))				throw ErrorXML(IDS_XML_ERRREAD, hr, s_LinkCLL, s_IP);
	if(value.vt != VT_NULL)
	{
		buf	= value;
		buf.Trim();
		m_IP.SetString(buf);
	}

	value.Clear();
	hr	= spElem->getAttribute(CComBSTR(s_IPDop1),		&value);
	if(FAILED(hr))				throw ErrorXML(IDS_XML_ERRREAD, hr, s_LinkCLL, s_IPDop1);
	if(value.vt != VT_NULL)
	{
		buf	= value;
		buf.Trim();
		m_IPDop[0].SetString(buf);
	}

	value.Clear();
	hr	= spElem->getAttribute(CComBSTR(s_IPDop2),		&value);
	if(FAILED(hr))				throw ErrorXML(IDS_XML_ERRREAD, hr, s_LinkCLL, s_IPDop2);
	if(value.vt != VT_NULL)
	{
		buf	= value;
		buf.Trim();
		m_IPDop[1].SetString(buf);
	}

	value.Clear();
	hr	= spElem->getAttribute(CComBSTR(s_Mask),		&value);
	if(FAILED(hr))				throw ErrorXML(IDS_XML_ERRREAD, hr, s_LinkCLL, s_Mask);
	if(value.vt != VT_NULL)
	{
		buf	= value;
		buf.Trim();
		m_Mask.SetString(buf);
	}

	value.Clear();
	hr	= spElem->getAttribute(CComBSTR(s_GTW),		&value);
	if(FAILED(hr))				throw ErrorXML(IDS_XML_ERRREAD, hr, s_LinkCLL, s_GTW);
	if(value.vt != VT_NULL)
	{
		buf	= value;
		buf.Trim();
		m_GTW.SetString(buf);
	}

	value.Clear();
	hr	= spElem->getAttribute(CComBSTR(s_MAC),		&value);
	if(FAILED(hr))				throw ErrorXML(IDS_XML_ERRREAD, hr, s_LinkCLL, s_MAC);
	if(value.vt != VT_NULL)
	{
		buf	= value;
		buf.Trim();
		m_MAC.SetString(buf);
	}

	value.Clear();
	hr	= spElem->getAttribute(CComBSTR(s_DataPort),		&value);
	if(FAILED(hr))				throw ErrorXML(IDS_XML_ERRREAD, hr, s_LinkCLL, s_DataPort);
	if(value.vt != VT_NULL)
	{
		hr	= value.ChangeType(VT_I4);
		if(FAILED(hr))			throw ErrorXML(IDS_XML_ERRREAD, hr, s_LinkCLL, s_DataPort);
		m_DataPort.SetValue(value.lVal);
	}

	value.Clear();
	hr	= spElem->getAttribute(CComBSTR(s_VoicePort),		&value);
	if(FAILED(hr))				throw ErrorXML(IDS_XML_ERRREAD, hr, s_LinkCLL, s_VoicePort);
	if(value.vt != VT_NULL)
	{
		hr	= value.ChangeType(VT_I4);
		if(FAILED(hr))			throw ErrorXML(IDS_XML_ERRREAD, hr, s_LinkCLL, s_VoicePort);
		m_VoicePort.SetValue(value.lVal);
	}

	value.Clear();
	hr	= spElem->getAttribute(CComBSTR(s_Note),		&value);
	if(FAILED(hr))				throw ErrorXML(IDS_XML_ERRREAD, hr, s_LinkCLL, s_Note);
	if(value.vt != VT_NULL)
	{
		m_Note	= value;
		m_Note.Trim();
	}
}

void	CLinkCLL::XMLPostLoad()
{
	if(m_pParam) m_pParam->XMLLoad();
}

/////////////////////////////////////////////////////////////////////////////
// класс таблицы исходящих связей КСЛ
//

const	TCHAR*	CLinkCLLTab::s_TabLinkCLL	= _T("TabLinkCLL");
const	TCHAR*	CLinkCLLTab::s_VolRec		= _T("VolRec");
const	TCHAR*	CLinkCLLTab::s_VolPlay		= _T("VolPlay");

CLinkCLLTab::CLinkCLLTab(CSlot*	aSlot)
	: m_pSlot(aSlot)
	, m_FlagModify(false)
	, m_VolRec(0)
	, m_VolPlay(0)
{
}

CLinkCLL*	CLinkCLLTab::Find(int aNumber)
{
	for(const_iterator i = begin(); i != end(); ++i)
	{
		if((*i)->m_Number == aNumber) return *i;
	}
	return NULL;
}

bool	CLinkCLLTab::Delete(int aNumber)
{
	for(iterator i = begin(); i != end(); ++i)
	{
		if((*i)->m_Number == aNumber) 
		{
			CLinkCLL*	pLink = *i;
			
			erase(i);
			delete	pLink;

			return true;
		}
	}
	return false;
}

CLinkCLL*	CLinkCLLTab::Find(int aUM, int aSlot)
{
	for(const_iterator i = begin(); i != end(); ++i)
	{
		if((*i)->m_NumUM == aUM && (*i)->m_NumSLOT == aSlot) return *i;
	}
	return NULL;
}

CLinkCLL*	CLinkCLLTab::FindUM(int aUM)
{
	for(const_iterator i = begin(); i != end(); ++i)
	{
		if((*i)->m_NumUM == aUM) return *i;
	}
	return NULL;
}

CLinkCLL*	CLinkCLLTab::FindIP(DWORD aIP)
{
	if(aIP == 0 || aIP == 0xFFFFFFFF)	return 0;

	for(const_iterator i = begin(); i != end(); ++i)
	{
		if((*i)->m_IP.GetValue() == aIP) 
		{
			return *i;
		}
		if((*i)->m_IPDop[0].GetValue() == aIP)
		{
			return *i;
		}
		if((*i)->m_IPDop[1].GetValue() == aIP)
		{
			return *i;
		}
	}
	return 0;
}

void	CLinkCLLTab::ReNumber()
{
	int	CurNumber = 1;

	for(iterator i = begin(); i != end(); ++i)
	{
		CLinkCLL*	pLink	= *i;

		if(pLink->m_Number > 0)
		{
			if(pLink->m_Number != CurNumber)
			{
				pLink->m_Number	= CurNumber;
				pLink->XMLUpdate();
				pLink->m_FlagModify	= true;
			}
			CurNumber++;
		}
	}
}


CLinkCLL*	CLinkCLLTab::GetItem(int aNumber)
{
	CLinkCLL*	It	= NULL;

	if(aNumber >= 0 && aNumber <= 16)
	{
		It	= Find(aNumber);

		if(It == NULL)
		{
			It = new CLinkCLL(aNumber, this);
			push_back( It );
		}
	}
	return(It);
}


void	CLinkCLLTab::XMLRemove()
{
	if(m_pSlot)
	{
		CComPtr<IXMLDOMNode>	spOldNode;

		m_pSlot->m_XMLNode->removeChild(m_XMLNode, &spOldNode);
	}
}

void	CLinkCLLTab::XMLUpdate()
{
	HRESULT	hr;

	CComQIPtr<IXMLDOMElement>	spElem	= m_XMLNode;
	if(!spElem)					throw ErrorXML(IDS_XML_ERRQI, 0, s_TabLinkCLL);

	hr	= spElem->setAttribute(CComBSTR(s_VolRec), CComVariant(m_VolRec));
	if(FAILED(hr))				throw ErrorXML(IDS_XML_ERRAPPEND, hr, s_TabLinkCLL, s_VolRec);

	hr	= spElem->setAttribute(CComBSTR(s_VolPlay), CComVariant(m_VolPlay));
	if(FAILED(hr))				throw ErrorXML(IDS_XML_ERRAPPEND, hr, s_TabLinkCLL, s_VolPlay);
}

void	CLinkCLLTab::XMLAdd()
{
	if(m_pSlot)
	{
		HRESULT	hr;

		CComPtr<IXMLDOMNode>	spNewNode;

		hr	= m_pSlot->m_pUM->m_pPult->m_pDisp->m_XMLDOM->createNode(	CComVariant(NODE_ELEMENT),
																		CComBSTR(s_TabLinkCLL),
																		NULL,
																		&spNewNode);
		if(FAILED(hr))				throw ErrorXML(IDS_XML_ERRCREATE, hr, s_TabLinkCLL);

		hr	= m_pSlot->m_XMLNode->appendChild(spNewNode, &m_XMLNode);
		if(FAILED(hr))				throw ErrorXML(IDS_XML_ERRAPPEND, hr, s_TabLinkCLL);
		if(!m_XMLNode)				throw ErrorXML(IDS_XML_ERRAPPEND, hr, s_TabLinkCLL);

		XMLUpdate();
	}
}

void	CLinkCLLTab::XMLLoad(CComPtr<IXMLDOMNode> spNode)
{
	m_XMLNode	= spNode;	

	CComQIPtr<IXMLDOMElement>	spElem	= spNode;
	if(!spElem)				throw ErrorXML(IDS_XML_ERRQI, 0, s_TabLinkCLL);

	HRESULT		hr;
	CComVariant	value;

	value.Clear();
	hr	= spElem->getAttribute(CComBSTR(s_VolRec),		&value);
	if(FAILED(hr))				throw ErrorXML(IDS_XML_ERRREAD, hr, s_TabLinkCLL, s_VolRec);
	if(value.vt != VT_NULL)
	{
		hr	= value.ChangeType(VT_I4);
		if(FAILED(hr))			throw ErrorXML(IDS_XML_ERRREAD, hr, s_TabLinkCLL, s_VolRec);
		m_VolRec		= value.lVal;
	}

	value.Clear();
	hr	= spElem->getAttribute(CComBSTR(s_VolPlay),		&value);
	if(FAILED(hr))				throw ErrorXML(IDS_XML_ERRREAD, hr, s_TabLinkCLL, s_VolPlay);
	if(value.vt != VT_NULL)
	{
		hr	= value.ChangeType(VT_I4);
		if(FAILED(hr))			throw ErrorXML(IDS_XML_ERRREAD, hr, s_TabLinkCLL, s_VolPlay);
		m_VolPlay		= value.lVal;
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

			if( (static_cast<CString>(nodeName)).CompareNoCase(CLinkCLL::s_LinkCLL) == 0)
			{
				CString		buf;
				CLinkCLL*	Link	= new CLinkCLL(0, this);

				Link->XMLLoad(child);

				if(Find(Link->m_Number))
				{
					buf.Format(_T("Link (%d:%d)->(%d:%d) already exist in structure")
						,m_pSlot->m_pUM->m_NumUM
						,m_pSlot->m_NumSLOT
						,Link->m_NumSLOT
						,Link->m_NumUM);

					delete Link;
					throw ErrorXML(buf, 0);
				}
				else
				{
					push_back(Link);
				}
			}
		}

		CComPtr<IXMLDOMNode>	next;
		child->get_nextSibling(&next);
		if(FAILED(hr))			throw ErrorXML(IDS_XML_ERRNEXTCHILD, hr);
		child	= next;
	}
}

void	CLinkCLLTab::XMLPostLoad()
{
	for(CLinkCLLTab::iterator i = begin(); i != end(); ++i)
	{
		if((*i)->m_pParam) (*i)->m_pParam->XMLLoad();
	}
}

void	CLinkCLLTab::XMLRelease()
{
	for(list<CLinkCLL*>::iterator i = begin(); i != end(); ++i)
	{
		CLinkCLL *It = *i;

		if(!(!It->m_XMLNode)) It->m_XMLNode.Release();
	}
	if(!(!m_XMLNode)) m_XMLNode.Release();
}

/////////////////////////////////////////////////////////////////////////////
// класс КЛШ

BYTE	CSlot::s_KeyNet[16]			= {0xBD,0x69,0xB5,0x3D,0x83,0x93,0x4a,0xac,0xBC,0xD9,0xC3,0x1C,0x12,0xB5,0xAA,0x68};
const	TCHAR*	CSlot::s_Slot		= _T("Slot");
const	TCHAR*	CSlot::s_Number		= _T("Number");
const	TCHAR*	CSlot::s_TypSlot	= _T("TypSlot");
const	TCHAR*	CSlot::s_Program	= _T("Program");
const	TCHAR*	CSlot::s_NumUMi		= _T("LinkUM");
const	TCHAR*	CSlot::s_NumSLOTi	= _T("LinkSlot");

CSlot::CSlot(int aNumSLOT, CUM* apUM, typSlot aTypSlot, CParam* aParam) 
	: m_TabLinkNet(this)
	, m_TabLinkCLL(this)
	, m_TabDev(this)
	, m_NumSLOT(aNumSLOT)
	, m_CodRestart(0)
	, m_TypSlot(aTypSlot)
	, m_TypDev(tdNONE)
	, m_TypBus(tbNONE)
	, m_MaxAdr(0)
	, m_pLink(NULL)
	, m_pUM(apUM)
	, m_LvlKey(0)
	, m_PrefKey(0)
	, m_NumKey(0)
	, m_Ident(0)
	, m_pParam(aParam)
	, m_NoJob(false)
	, m_OnGGS(false)
	, m_NoLink(false)
	, m_OnKey(false)
	, m_LowVers(false)
	, m_TimeAns(0)
	, m_SuppIAP(false)
	, m_SuppREQ(false)
	, m_SuppLLBAT(false)
	, m_KnownSost(false)
	, m_WaitKey(false)
	, m_FlagModify(false)
	, m_LastGetConf(0)
	, m_ExistInp(false)
	, m_CountConf(0)
	, m_IntervConf(0)
	, m_IsProgram(false)
{
	m_LastMes		= COleDateTime::GetCurrentTime();
	m_TimSost		= COleDateTime::GetCurrentTime();
	m_TimLast		= COleDateTime::GetCurrentTime();
	m_TimGetConf	= COleDateTime::GetCurrentTime();
	m_TimRestart.SetStatus(COleDateTime::null);
	m_TimKey.SetStatus(COleDateTime::null);
	m_VersProg[0]	= 0;
	m_VersProg[1]	= 0;
	m_MsgApi[0]		= 0; 
	m_MsgApi[1]		= 0; 
}

CSlot::CSlot( CSlot &aSlot) 
	: m_pParam(NULL)
	, m_TabLinkNet(this)
	, m_TabLinkCLL(this)
	, m_TabDev(this)
{
	Clear();

	m_NumSLOT		= aSlot.m_NumSLOT;
	m_TypSlot		= aSlot.m_TypSlot;
	m_NumUMi		= aSlot.m_NumUMi;
	m_NumSLOTi		= aSlot.m_NumSLOTi;
	m_TypDev		= aSlot.m_TypDev;
	m_TypBus		= aSlot.m_TypBus;
	m_IsProgram		= aSlot.m_IsProgram;

	for(list<CDev*>::iterator k = aSlot.m_TabDev.begin(); k != aSlot.m_TabDev.end(); ++k)
	{
		CDev* ItO = *k;
		CDev* ItN = new CDev(*ItO);

		ItN->m_pSlot = this;

		m_TabDev.push_back( ItN );
	}

	for(list<CLinkNet*>::iterator l = aSlot.m_TabLinkNet.begin(); l != aSlot.m_TabLinkNet.end(); ++l)
	{
		CLinkNet* ItO = *l;
		CLinkNet* ItN = new CLinkNet(*ItO);

		m_TabLinkNet.push_back( ItN );
	}
}

CSlot::~CSlot()
{
	if(m_pParam != NULL) delete m_pParam;
}

void CSlot::Clear()
{
	m_TimLast		= COleDateTime::GetCurrentTime();
	m_TimGetConf	= COleDateTime::GetCurrentTime();

	m_NumSLOT		= 0;
	m_LastMes.SetStatus(COleDateTime::null);
	m_TimSost.SetStatus(COleDateTime::null);
	m_SostCLB.Clear();
	m_TimRestart.SetStatus(COleDateTime::null);
	m_CodRestart	= 0;
	m_TypSlot		= tsNone;
	m_pLink			= NULL;
	m_TabLinkNet.Clear();
	m_pUM			= NULL;
	m_Pack.Clear();
	m_TypDev		= tdNONE;
	m_TypBus		= tbNONE;
	m_MaxAdr		= 0;
	m_VersProg[0]	= 0;
	m_VersProg[1]	= 0;
	m_MsgApi[0]		= 0; 
	m_MsgApi[1]		= 0; 
	m_TimKey.SetStatus(COleDateTime::null);
	m_LvlKey		= 0;
	m_PrefKey		= 0;
	m_NumKey		= 0;
	m_Ident			= 0;
	m_TabDev.clear();
	m_TimeAns		= 0;
	m_ExistInp		= false;

	m_NumUMi		= -1;
	m_NumSLOTi		= -1;
	m_CountConf		= 0;
	m_IntervConf	= 0;
	m_NoJob			= false;
	m_OnGGS			= false;
	m_NoLink		= false;
	m_OnKey			= false;
	m_LowVers		= false;
	m_SuppIAP		= false;
	m_SuppREQ		= false;
	m_KnownSost		= false;
	m_WaitKey       = false;
	m_FlagModify	= false;
	m_IsProgram		= false;
}

void	CSlot::SetIdent(int aIdent)
{
	if(m_pUM->m_pPult->m_RootUM == m_pUM->m_NumUM || !m_pUM->m_pPult->m_bCross)
	{
		m_Pack.SetIdent(aIdent);
	}
	else
	{
		CSlot*	ItRoot = m_pUM->m_pPult->m_pDisp->RootSlot(m_pUM->m_pPult, m_pUM->m_NumUM);

		if(ItRoot) ItRoot->SetIdent(aIdent);
	}
}

void	CSlot::SetPrior(int aPrior)
{
	if(m_pUM->m_pPult->m_RootUM == m_pUM->m_NumUM || !m_pUM->m_pPult->m_bCross)
	{
		m_Pack.SetPrior(aPrior);
	}
	else
	{
		CSlot*	ItRoot = m_pUM->m_pPult->m_pDisp->RootSlot(m_pUM->m_pPult, m_pUM->m_NumUM);

		if(ItRoot) ItRoot->SetPrior(aPrior);
	}
}

void CSlot::Put()
{
	if(m_pUM->m_pPult->m_RootUM == m_pUM->m_NumUM || !m_pUM->m_pPult->m_bCross)
	{
		if(!m_Pack.IsEmpty()) 
		{
			m_pUM->m_pPult->m_pDisp->Put(m_pUM->m_pPult, m_Pack);
			m_Pack.Clear(); 
		}
	}
	else
	{
		CSlot*	ItRoot = m_pUM->m_pPult->m_pDisp->RootSlot(m_pUM->m_pPult, m_pUM->m_NumUM);

		if(ItRoot != NULL) ItRoot->Put();
	}
}

void CSlot::PutNoIdent()
{
	if(m_pUM->m_pPult->m_RootUM == m_pUM->m_NumUM || !m_pUM->m_pPult->m_bCross)
	{
		if(!m_Pack.IsEmpty()) 
		{
			m_pUM->m_pPult->m_pDisp->PutNoIdent(m_pUM->m_pPult, m_Pack);
			m_Pack.Clear(); 
		}
	}
	else
	{
		CSlot*	ItRoot = m_pUM->m_pPult->m_pDisp->RootSlot(m_pUM->m_pPult, m_pUM->m_NumUM);

		if(ItRoot != NULL) ItRoot->PutNoIdent();
	}
}

void CSlot::AddMesNoIdent(int aPrior, const CMes& aMes)
{
	SetPrior(aPrior);
	if(m_pUM->m_pPult->m_RootUM == m_pUM->m_NumUM || !m_pUM->m_pPult->m_bCross)
	{
		if(!m_Pack.AddMes(aMes))
		{
			PutNoIdent();
			m_Pack.AddMes(aMes);
		}
	}
	else
	{
		CSlot*	ItRoot = m_pUM->m_pPult->m_pDisp->RootSlot(m_pUM->m_pPult, m_pUM->m_NumUM);

		if(ItRoot) ItRoot->AddMes(aPrior, aMes);
	}
}

void CSlot::AddMes(int aPrior, const CMes& aMes)
{
	SetPrior(aPrior);
	if(m_pUM->m_pPult->m_RootUM == m_pUM->m_NumUM || !m_pUM->m_pPult->m_bCross)
	{
		if(!m_Pack.AddMes(aMes))
		{
			Put();
			m_Pack.AddMes(aMes);
		}
	}
	else
	{
		CSlot*	ItRoot = m_pUM->m_pPult->m_pDisp->RootSlot(m_pUM->m_pPult, m_pUM->m_NumUM);

		if(ItRoot != NULL) ItRoot->AddMes(aPrior, aMes);
	}
}

void CSlot::AddMes(int aPrior, CMes::typMes aTypMes, BYTE aMask)
{
	CMes	mes(m_pUM->m_NumUM, m_NumSLOT, m_pUM->m_pPult->m_RootUM, m_pUM->m_pPult->m_RootSLOT, aTypMes, aMask);

	AddMes(aPrior, mes);
}

void CSlot::AddMes(int aPrior, CMes::typMes aTypMes, CMes::askFromMain aAsk, int dev, const BYTE* aMask, int aLen, int aDuration)
{
	CMes mes(m_pUM->m_NumUM, m_NumSLOT, m_pUM->m_pPult->m_RootUM, m_pUM->m_pPult->m_RootSLOT, dev, aTypMes, aAsk, aMask, aLen, aDuration);

	AddMes(aPrior, mes);
}

void CSlot::AddMes(int aPrior, CMes::typService aService, int aDuration, int dev, CMes::askFromMain aAsk, const BYTE* aData, int aLen)
{
	int		Len	= 0;
	BYTE	body[8+3*4];

	memset(body, 0, sizeof(body));

	body[0]	= aService;
	CMes::SetAdr(&body[1], m_pUM->m_pPult->m_RootUM, m_pUM->m_pPult->m_RootSLOT);
	body[3]	= aDuration==0? (0): ((aDuration-1)/20 + 1);
	body[3]	= body[3]&0xF;
	body[4]	= aLen==0? (0): ((aLen-1)/4 + 1);
	body[4]	= body[4]&0x3;
	CMes::SetAdr(&body[5], dev, aAsk);
	if(aAsk > 0xF) body[5] |= 0x20;
	memcpy(&body[7], aData, body[4]*4);

	Len = 7 + body[4]*4;

	CMes mes(m_pUM->m_NumUM, m_NumSLOT, m_pUM->m_pPult->m_RootUM, m_pUM->m_pPult->m_RootSLOT, CMes::tmAskServ, body, Len);

	AddMes(aPrior, mes);
}

void CSlot::AddMes(int aPrior, CMes::typMes aTypMes, const BYTE* aMask, int aLen)
{
	CMes mes(m_pUM->m_NumUM, m_NumSLOT, m_pUM->m_pPult->m_RootUM, m_pUM->m_pPult->m_RootSLOT, aTypMes, aMask, aLen);

	AddMes(aPrior, mes);
}

void CSlot::AddMes(int aPrior, CMes::typService aTypSrv, const BYTE* aBody, int aLen)
{
	CMes mes(m_pUM->m_NumUM, m_NumSLOT, m_pUM->m_pPult->m_RootUM, m_pUM->m_pPult->m_RootSLOT, aTypSrv, aBody, aLen);

	AddMes(aPrior, mes);
}

void	CSlot::AddMes(int aPrior, CMes::typPrefix aTypPrefix, BYTE aAddr)
{
	CMes mes(m_pUM->m_NumUM, m_NumSLOT, m_pUM->m_pPult->m_RootUM, m_pUM->m_pPult->m_RootSLOT, aTypPrefix, aAddr);

	AddMes(aPrior, mes);
}

void	CSlot::AddMes(int aPrior, CMes::typPrefix aTypPrefix, BYTE aAddr, CMes::askFromMain aAsk, int aLen, const BYTE* aBody)
{
	CMes mes(m_pUM->m_NumUM, m_NumSLOT, m_pUM->m_pPult->m_RootUM, m_pUM->m_pPult->m_RootSLOT, aTypPrefix, aAddr, aAsk, aLen, aBody);

	AddMes(aPrior, mes);
}

void	CSlot::AddMes(int aPrior, int aDriv, CMes::subServ18 aSubServ18, const BYTE* aBody, int aLen)
{
	CMes	mes(m_pUM->m_NumUM, m_NumSLOT, m_pUM->m_pPult->m_RootUM, m_pUM->m_pPult->m_RootSLOT, aDriv, aSubServ18, aBody, aLen);

	AddMes(aPrior, mes);
}

void CSlot::AddMesSetNVRAM(int aPrior, int aAddr, int aValue)
{
	int		ValueLow = aValue&0xFF;
	int		ValueHig = (aValue>>8)&0xFF;
	BYTE	val[4] = {0,0,0,0};

	val[0]	= aAddr;
	val[1]	= ValueHig & 0x3F;
	val[2]	= ((ValueHig & 0xC0) >> 6) | ((ValueLow & 0x0F) << 2);
	val[3]	= ((ValueLow & 0xF0) >> 2) | 0x03;

	AddMes(aPrior, CMes::tmGetNVR, val, 4);
}

void CSlot::GetSost()
{
	CMes	mes(m_pUM->m_NumUM, m_NumSLOT, m_pUM->m_pPult->m_RootUM, m_pUM->m_pPult->m_RootSLOT, m_pUM->m_pPult->m_RootUM, CMes::tmGetSetDev, (CMes::askFromMain)m_pUM->m_pPult->m_RootSLOT);

	AddMes(prGetSost, mes);
	AddMes(prGetSost, CMes::tmGetBlock, 0);
	if(Vers() > 106 || m_TypDev == tdCDC || m_TypDev == tdCUS || m_TypDev == tdCLL || m_TypDev == tdCRC || m_TypDev == tdGSM) AddMes(prGetSost, CMes::tmGetNVR,   m_NvRam.NextIndex());

	Put();
}

bool CSlot::IsProgram()
{
	return m_IsProgram || m_VersProg[0] == 10;
}

bool CSlot::Ggs(CMes::askFromMain aAsk, int dev, DWORD aIP, DWORD aPort, int aNumWavIn,  int aNumWavOut)
{
	if(m_pUM->m_pPult->m_RootUM == m_pUM->m_NumUM || !m_pUM->m_pPult->m_bCross)
	{
		BYTE	Adr[2+1+8+2]	= {0x3F,0x3F,0,0,0,0,0,0,0,0,0,aNumWavIn&0x3F,aNumWavOut&0x3F};

		CMes::SetAdr(Adr, dev, aAsk);

		int	LenMes = 2;

		if(	IsProgram() )	// 26.10.2010
		{
			Adr[LenMes++]	= 1;

			BYTE	tmp[6];
			memcpy(&tmp[0], &aIP,	4);
			memcpy(&tmp[4], &aPort, 2);

			CMes::Conv8To6(6, tmp, &Adr[LenMes], 8);

			LenMes	+= 10;
		}
		else
		{
			Adr[LenMes++]	= 0;
		}
		AddMes(prDial, CMes::tmSetSost, Adr, LenMes);
		Put();

		return(true);
	}
	else
	{
		CSlot*	ItRoot = m_pUM->m_pPult->m_TabUM.RootSlot(m_pUM->m_NumUM);

		if(ItRoot)
		{
			CUM* ItUM = m_pUM->m_pPult->m_TabUM.FindUM(m_pUM->m_NumUM);
			if(ItUM)	
			{
				BYTE	Type	= IsProgram()? 2:0; // 2 - КЛШ, с которым включается ГГС, программный; 0 КЛШ, с которым включается ГГС, аппаратный

				CMes mes(m_pUM->m_NumUM, m_NumSLOT, m_pUM->m_pPult->m_RootUM, m_pUM->m_pPult->m_RootSLOT, dev, CMes::tmSetSost, aAsk, &Type, IsProgram()? 1:0);
				ItRoot->AddMes(prDial, mes);
				ItRoot->Put();

				int	NumUM   = m_pUM->m_NumUM;
				int	NumSLOT = m_NumSLOT;

				while(ItUM->m_NumUM != m_pUM->m_pPult->m_RootUM)
				{
					BYTE	Adr[2+1+8+2]= {0x3F,0x3F,0,0,0,0,0,0,0,0,0,aNumWavIn&0x3F,aNumWavOut&0x3F};
					CSlot*	ItS			= ItUM->FindInpLink();

					if(ItS == NULL)				break;
					if(ItS->m_pLink == NULL)	break;

					if(aAsk == CMes::amOffGGS)
					{
						if(ItS->IsNewGGS())	CMes::SetAdr(Adr, 0xFF, NumSLOT);
					}
					else
					{
						if(ItS->IsNewGGS())	CMes::SetAdr(Adr, m_pUM->m_NumUM, NumSLOT);
						else				CMes::SetAdr(Adr, NumUM, NumSLOT);
					}

					CMes	MesInp(ItUM->m_NumUM, ItS->m_NumSLOT, m_pUM->m_pPult->m_RootUM, m_pUM->m_pPult->m_RootSLOT, CMes::tmSetSost, Adr, 3);

					NumUM   = ItS->m_pLink->m_pUM->m_NumUM;
					NumSLOT = ItS->m_pLink->m_NumSLOT;

					if(aAsk == CMes::amOffGGS)
					{
						if(ItS->IsNewGGS())	CMes::SetAdr(Adr, 0xFF, NumSLOT);
					}
					else
					{
						if(ItS->IsNewGGS())	CMes::SetAdr(Adr, ItUM->m_NumUM, NumSLOT);
						else				CMes::SetAdr(Adr, NumUM, NumSLOT);
					}

					int	LenMes = 2;

					if(NumUM == m_pUM->m_pPult->m_RootUM)
					{							// КСЛ в корневом УМ
						if(	ItS->m_pLink->IsProgram() )	// 26.10.2010
						{
							Adr[LenMes++]	= 1;

							BYTE	tmp[6];
							memcpy(&tmp[0], &aIP,	4);
							memcpy(&tmp[4], &aPort, 2);

							CMes::Conv8To6(6, tmp, &Adr[LenMes], 8);

							LenMes	+= 10;
						}
						else
						{
							Adr[LenMes++]	= 0;
						}
					}
					else
					{								// КСЛ в удаленном УМ	07.07.2011
						if(	ItS->m_pLink->IsProgram() )	// 26.10.2010
						{
							Adr[LenMes++]	= 4;	// удаленный КСЛ - программный
						}
					}

					CMes	MesOut(NumUM, NumSLOT, m_pUM->m_pPult->m_RootUM, m_pUM->m_pPult->m_RootSLOT, CMes::tmSetSost, Adr, LenMes);

					if(!ItS->IsNewGGS()) ItRoot->AddMes(prDial, MesInp);
					ItRoot->AddMes(prDial, MesOut);

					ItUM	= ItS->m_pLink->m_pUM;
				}
				CMes mesAdd(m_pUM->m_NumUM, m_NumSLOT, m_pUM->m_pPult->m_RootUM, m_pUM->m_pPult->m_RootSLOT, dev, CMes::tmSetSost, aAsk);
				ItRoot->AddMes(prDial, mesAdd);
				ItRoot->Put(); // 03.05.2011 - еще одна попытка включения

				if(ItUM->m_NumUM == m_pUM->m_pPult->m_RootUM) return true;
			}
		}
	} 
	return false;
}

void CSlot::SetAdmPult()
{
	CMes	mes(m_pUM->m_NumUM, m_NumSLOT, m_pUM->m_pPult->m_RootUM, m_pUM->m_pPult->m_RootSLOT, m_pUM->m_pPult->m_RootUM, CMes::tmGetSetDev, (CMes::askFromMain)m_pUM->m_pPult->m_RootSLOT);

	AddMes(prGetSost, mes);
	Put();
}

BYTE CSlot::NextIdent()
{
	m_Ident++;
	m_Ident &= 0x3F;

	if(m_Ident == 0) m_Ident = 1;

	return(m_Ident);
}

void CSlot::SetAddr()
{
	m_LastMes	= COleDateTime::GetCurrentTime();
	m_TimSost	= COleDateTime::GetCurrentTime();

	m_Pack.SetAddr(m_pUM->m_pPult->m_RootSLOT, m_NumSLOT);
}

void	CSlot::GetNvRamAll()
{
	m_NvRam.Clear();

	for(int i = 0; i < 64; i++) AddMes(prGetSost, CMes::tmGetNVR, i);
	Put();
}

bool CSlot::IsDevice()
{
	return(m_TypSlot != tsMon);
}

bool	CSlot::IsLocal()
{
	return(m_pUM->m_pPult->m_RootUM == m_pUM->m_NumUM);
}

bool	CSlot::IsCLB()
{
	return	m_TypDev == tdCLB		|| 
			m_TypDev == tdCDC		|| 
			m_TypDev == tdCLB_		|| 
			m_TypDev == tdCLBP		|| 
			m_TypDev == tdCLB5m		|| 
			m_TypDev == tdCLB7m		||
			m_TypDev == tdCLBCM3	||
			m_TypDev == tdCLBCM3m;
}

bool	CSlot::IsEther()
{
	return	m_TypSlot == tsCRCo  || 
			m_TypSlot == tsGSMo  || 
			m_TypSlot == tsNETo  || 
			m_TypSlot == tsCLLEo || 
			m_TypSlot == tsCLLMo || 
			m_TypSlot == tsCLLRo;
}

bool CSlot::IsLine()
{
	return(m_TypSlot == tsCLLo  || 
		   m_TypSlot == tsBRDGo);
}

bool CSlot::IsInpLink()
{
	return(	m_TypSlot == tsCLLi || 
			m_TypSlot == tsBRDGi|| 
			m_TypSlot == tsCRCi || 
			m_TypSlot == tsGSMi || 
			m_TypSlot == tsCLLEi||
			m_TypSlot == tsCLLMi||
			m_TypSlot == tsCLLRi||
			m_TypSlot == tsNETi);
}

bool CSlot::IsNewGGS()
{
	return(m_TypSlot == tsNETi  || 
		   m_TypSlot == tsGSMi  || 
		   m_TypSlot == tsCLLEi || 
		   m_TypSlot == tsCLLMi || 
		   m_TypSlot == tsCLLRi);
}


void CSlot::ReLinkOne(typSlot aTypInp, typSlot aTypOut)
{
	if(m_pLink != NULL)
	{
		if(!m_pLink->m_pUM->m_bLink)
		{
			m_TypSlot			= aTypOut;
			m_pLink->m_TypSlot	= aTypInp;

			m_pLink->m_pUM->ReLink();
		}
	}
}

void CSlot::ReLinkEth(typSlot aTypInp, typSlot aTypOut)
{
	if(m_pLink != NULL)
	{
		if(!m_pLink->m_pUM->m_bLink)
		{
			m_TypSlot			= aTypOut;	// делаем текущий слот исходящим
			m_pLink->m_TypSlot	= aTypInp;	// делаем связанный слот входящим

			for(list<CLinkNet*>::iterator m = m_pLink->m_TabLinkNet.begin(); m != m_pLink->m_TabLinkNet.end(); ++m)
			{						
				CLinkNet* ItLink = *m;

				if(ItLink->m_pLink == this) ItLink->m_pLink = m_pLink;	// если связь указывает на текущий слот,
																		// то переназначаем ее на связанный

				ItLink->m_pLink->m_pLink	= this;		// переставляем указатель связанного по таблице связи слота на текущий

				if(!m_TabLinkNet.FindLink(ItLink->m_pLink))	
				{
					ItLink->m_pTabLinkNet	= &m_TabLinkNet;
					m_TabLinkNet.push_back(ItLink);
				}
				else
				{
					delete ItLink;
				}
			}
			m_pLink->m_TabLinkNet.clear();		// очищаем таблицу связей связанного слота

			m_pLink->m_pUM->ReLink();
		}
	}
}

void CSlot::SetTimeConf()
{
#if defined(WIN32_PLATFORM_PSPC) || defined(WIN32_PLATFORM_WFSP)
	m_LastGetConf	= 0;
#else
	m_LastGetConf	= time(0);
#endif
}

void CSlot::ClrStat()
{
	m_LastGetConf	= 0;
	m_CountConf		= 0;
	m_IntervConf	= 0;
}

void CSlot::AskConf()
{
	if(!m_pUM->m_bNoAdmin)
	{
		CMes	mes(m_pUM->m_NumUM, m_NumSLOT, m_pUM->m_pPult->m_RootUM, m_pUM->m_pPult->m_RootSLOT, m_pUM->m_pPult->m_RootUM, CMes::tmGetSetDev, static_cast<CMes::askFromMain>(m_pUM->m_pPult->m_RootSLOT));

		AddMes(prGetSost, mes);

		Put();

		SetTimeConf();
	}
}

void CSlot::GetSostL()
{
	double	Sec = (COleDateTime::GetCurrentTime() - m_TimGetConf).GetTotalSeconds();

	if( Sec >= 90.0	&& Sec <= 450.0 )
	{
		AskConf();
	}
}

void CSlot::GetSostR()
{
	if(!m_NoJob && !m_ExistInp)
	{
		AskConf();
	}
}

void	CSlot::SetNoInp()
{
	if(!m_NoJob) m_ExistInp = false;
}

void CSlot::TestOffL()
{
	if((COleDateTime::GetCurrentTime() - m_TimLast).GetTotalSeconds() > 430.0 && !m_NoJob)
	{
		m_NoJob = true;
		m_pUM->m_pPult->m_pDisp->SendAlert(m_pUM->m_pPult, m_pUM->m_NumUM, m_NumSLOT, CMes::taOffBlk);
	}
}

void CSlot::TestOffR()
{
	if(!m_NoJob && !m_ExistInp)
	{
		m_NoJob = true;
		m_pUM->m_pPult->m_pDisp->SendAlert(m_pUM->m_pPult, m_pUM->m_NumUM, m_NumSLOT, CMes::taOffBlk);
	}
}

bool CSlot::IsLowVers()
{
	bool	rc = false;
	int		V  = 0;

	if(MinVers(V))
	{
		if(V > m_VersProg[1]) rc = true;
	}

	return(rc);
}

bool CSlot::MinVers(int& aV)
{
	bool	rc = false;

	aV	= 0;

	if(IsDevice())
	{
		switch(m_TypDev)
		{
		case tdCLB:			// КЛШ
			if(m_VersProg[0] == 1)
			{
				aV	= 11;
//				aV	= 12;
				rc	= true;
			}
			else if(m_VersProg[0] == 2)
			{
				aV	= 2;
				rc	= true;
			}
			break;
		case tdCDC:			// КЛШ-КДК
			if(m_VersProg[0] == 0)
			{
				aV	= 8;
				rc	= true;
			}
			else if(m_VersProg[0] == 2)
			{
				aV	= 2;
				rc	= true;
			}
			break;
		case tdCUS:			// КУС
			if(m_VersProg[0] == 0)
			{
				aV	= 4;
				rc	= true;
			}
			break;
		case tdCLL:			// КСЛ
			break;
		case tdCRC:			// КРК
			break;
		case tdGSM:			// GSM
			break;
		}
	}

	return(rc);
}

void	CSlot::LoadKeyNet()
{
	CMes	getPar0(m_pUM->m_NumUM, m_NumSLOT, m_pUM->m_pPult->m_RootUM, m_pUM->m_pPult->m_RootSLOT, CMes::jnGet, addrKeyNet, lenKeyNet);
	AddMes(32, getPar0);
}

void	CSlot::GenKeyNet(const TCHAR* aKeyNet, BYTE* aKeyNetBin)
{
	memset(aKeyNetBin, 0xFF, lenKeyNet);

	size_t Len = _tcsclen(aKeyNet);

	if(Len > 0)
	{
		CIdeaCtx	Idea;
		Idea.Key(s_KeyNet);

		BYTE		bufF[8];
		memset(bufF, 0, sizeof(bufF));
		CStringA	KeyNet(aKeyNet);

		memcpy(bufF, static_cast<const char*>(KeyNet), min(Len, sizeof(bufF)));

		Idea.Encrypt(bufF, bufF, 1);

		memcpy(aKeyNetBin, bufF, sizeof(lenKeyNet));

		if(aKeyNetBin[0] == 0 || aKeyNetBin[0] == 0xFF) aKeyNetBin[0] = 0x01;
	}
}

void	CSlot::SaveKeyNet(const TCHAR* aKeyNet)
{
	BYTE	bufK[4];
	memset(bufK, 0xFF, sizeof(bufK));

	size_t Len = _tcsclen(aKeyNet);

	if(Len > 0)
	{
		CIdeaCtx	Idea;
		Idea.Key(s_KeyNet);

		BYTE		bufF[8];
		memset(bufF, 0, sizeof(bufF));
		CStringA	KeyNet(aKeyNet);

		memcpy(bufF, static_cast<const char*>(KeyNet), min(Len, sizeof(bufF)));

		Idea.Encrypt(bufF, bufF, 1);

		memcpy(bufK, bufF, sizeof(bufK));

		if(bufK[0] == 0 || bufK[0] == 0xFF) bufK[0] = 0x01;

	}
	CMes	setKeyNet(m_pUM->m_NumUM, m_NumSLOT, m_pUM->m_pPult->m_RootUM, m_pUM->m_pPult->m_RootSLOT, CMes::jnSet, addrKeyNet, lenKeyNet, bufK);
	AddMes(32, setKeyNet);
}

CAtlString CSlot::nameTypDev(typDev aTypDev)
{
	switch(aTypDev)
	{
	case tdNONE:		return(_S_(IDS_NOT_DEFINED));
	case tdCLB:			return(_S_(IDS_TYPDEV_LBC));
	case tdCDC:			return(_S_(IDS_TYPDEV_LBC_CDC));
	case tdCUS:			return(_S_(IDS_TYPDEV_IUC));
	case tdCLB_:		return(_S_(IDS_TYPDEV_LBC_P));
	case tdCLBP:		return(_S_(IDS_TYPDEV_LBC_PRO));
	case tdCLB5m:		return(_S_(IDS_TYPDEV_LBC_L5));
	case tdCLB7m:		return(_S_(IDS_TYPDEV_LBC_L7));
	case tdCLBCM3:		return(_S_(IDS_TYPDEV_LBC_CM3));
	case tdCLBCM3m:		return(_S_(IDS_TYPDEV_LBC_CM3_m));
	case tdNET:			return(_S_(IDS_TYPDEV_NET));
	case tdCLL:			return(_S_(IDS_TYPDEV_JLC));
	case tdCRC:			return(_S_(IDS_TYPDEV_RCC));
	case tdGSM:			return(_S_(IDS_TYPDEV_GSM));
	case tdDPL:			return(_S_(IDS_TYPDEV_LDP));
	case tdDPG:			return(_S_(IDS_TYPDEV_GDP));
	case tdGGS:			return(_S_(IDS_TYPDEV_CPEC));
	case tdCLLM:		return(_S_(IDS_TYPDEV_JLC_GSM));
	case tdCLLR:		return(_S_(IDS_TYPDEV_JLC_433));
	case tdCLLE:		return(_S_(IDS_TYPDEV_JLC_ETH));
	case tdBRDG:		return(_S_(IDS_TYPDEV_CONCENTR));
	case tdCLLG:		return(_S_(IDS_TYPDEV_JLC_24G));
	case td723:			return(_S_(IDS_TYPDEV_CODEC723));
	case tdCLLEm:		return(_S_(IDS_TYPDEV_JLCL_ETH));
	case tdCLLRm:		return(_S_(IDS_TYPDEV_JLCL_433));
	case tdCLLGm:		return(_S_(IDS_TYPDEV_JLCL_24G));
	case tdCLLMm:		return(_S_(IDS_TYPDEV_JLCL_GSM));
	case tdCLLMCM3:		return(_S_(IDS_TYPDEV_JLC_GSM_CM3));
	case tdCLLRCM3:		return(_S_(IDS_TYPDEV_JLC_433_CM3));
	case tdCLLECM3:		return(_S_(IDS_TYPDEV_JLC_ETH_CM3));
	case tdCLLMCM3m:	return(_S_(IDS_TYPDEV_JLC_GSM_CM3_m));
	case tdCLLRCM3m:	return(_S_(IDS_TYPDEV_JLC_433_CM3_m));
	case tdCLLECM3m:	return(_S_(IDS_TYPDEV_JLC_ETH_CM3_m));
	default:
		{
			CAtlString	buf;

			buf.Format(_T("%d"), aTypDev);
			return(buf);
		}
	}
}

CAtlString CSlot::nameTypBus(typBus aTypBus)
{
	switch(aTypBus)
	{
	case tbNONE:	return(_S_(IDS_NOT_DEFINED));
	case tbARM:		return(_S_(IDS_TYPBUS_ARM));
	case tbOB4x:	return(_S_(IDS_TYPBUS_OB_4X));
	case tbOB31:	return(_S_(IDS_TYPBUS_OB_31));
	case tbOB30:	return(_S_(IDS_TYPBUS_OB_30));
	case tbCDC:		return(_S_(IDS_TYPBUS_CDC));
	case tbOB5X:	return(_S_(IDS_TYPBUS_OB_5X));
	default:
		{
			CAtlString	buf;

			buf.Format(_T("%d"), aTypBus);
			return(buf);
		}
	}
}

bool	CSlot::CanProgramm(typSlot aTypSlot)
{
	switch(aTypSlot)
	{
	case tsCLB:
	case tsCLLEo:
	case tsCLLEi:
		{
			return true;
		}
	}
	return false;
}

void	CSlot::XMLRemove()
{
	if(m_pUM)
	{
		CComPtr<IXMLDOMNode>	spOldNode;

		m_pUM->m_XMLNode->removeChild(m_XMLNode, &spOldNode);
	}
}

void	CSlot::XMLUpdate()
{
	HRESULT	hr;
	CString	buf;

	CComQIPtr<IXMLDOMElement>	spElem	= m_XMLNode;
	if(!spElem)					throw ErrorXML(IDS_XML_ERRQI, 0, s_Slot);

	hr	= spElem->setAttribute(CComBSTR(s_Number), CComVariant(m_NumSLOT));
	if(FAILED(hr))				throw ErrorXML(IDS_XML_ERRAPPEND, hr, s_Slot, s_Number);

	hr	= spElem->setAttribute(CComBSTR(s_TypSlot), CComVariant(m_TypSlot));
	if(FAILED(hr))				throw ErrorXML(IDS_XML_ERRAPPEND, hr, s_Slot, s_TypSlot);

	hr	= spElem->setAttribute(CComBSTR(s_Program), CComVariant(m_IsProgram? 1: 0));
	if(FAILED(hr))				throw ErrorXML(IDS_XML_ERRAPPEND, hr, s_Slot, s_Program);

	if(IsLine())
	{
		hr	= spElem->setAttribute(CComBSTR(s_NumUMi), CComVariant(m_NumUMi));
		if(FAILED(hr))				throw ErrorXML(IDS_XML_ERRAPPEND, hr, s_Slot, s_NumUMi);

		hr	= spElem->setAttribute(CComBSTR(s_NumSLOTi), CComVariant(m_NumSLOTi));
		if(FAILED(hr))				throw ErrorXML(IDS_XML_ERRAPPEND, hr, s_Slot, s_NumSLOTi);
	}

	if(m_pParam) m_pParam->XMLUpdate();
}

void	CSlot::XMLAdd()
{
	if(m_pUM)
	{
		HRESULT	hr;

		CComPtr<IXMLDOMNode>	spNewNode;

		hr	= m_pUM->m_pPult->m_pDisp->m_XMLDOM->createNode(	CComVariant(NODE_ELEMENT),
																CComBSTR(s_Slot),
																NULL,
																&spNewNode);
		if(FAILED(hr))				throw ErrorXML(IDS_XML_ERRCREATE, hr, s_Slot);

		hr	= m_pUM->m_XMLNode->appendChild(spNewNode, &m_XMLNode);
		if(FAILED(hr))				throw ErrorXML(IDS_XML_ERRAPPEND, hr, s_Slot);
		if(!m_XMLNode)				throw ErrorXML(IDS_XML_ERRAPPEND, hr, s_Slot);


		if(IsEther())
		{
			m_TabLinkNet.XMLAdd();
			m_TabLinkCLL.XMLAdd();
		}
		XMLUpdate();
	}
}

void	CSlot::XMLLoad(CComPtr<IXMLDOMNode> spNode)
{
	m_XMLNode	= spNode;	

	CComQIPtr<IXMLDOMElement>	spElem	= spNode;
	if(!spElem)				throw ErrorXML(IDS_XML_ERRQI, 0, s_Slot);

	HRESULT		hr;
	CComVariant	value;

	value.Clear();
	hr	= spElem->getAttribute(CComBSTR(s_Number),		&value);
	if(FAILED(hr))				throw ErrorXML(IDS_XML_ERRREAD, hr, s_Slot, s_Number);
	if(value.vt != VT_NULL)
	{
		hr	= value.ChangeType(VT_I4);
		if(FAILED(hr))			throw ErrorXML(IDS_XML_ERRREAD, hr, s_Slot, s_Number);
		m_NumSLOT		= value.lVal;
	}

	value.Clear();
	hr	= spElem->getAttribute(CComBSTR(s_TypSlot),		&value);
	if(FAILED(hr))				throw ErrorXML(IDS_XML_ERRREAD, hr, s_Slot, s_TypSlot);
	if(value.vt != VT_NULL)
	{
		hr	= value.ChangeType(VT_I4);
		if(FAILED(hr))			throw ErrorXML(IDS_XML_ERRREAD, hr, s_Slot, s_TypSlot);
		m_TypSlot		= static_cast<typSlot>(value.lVal);
	}

	value.Clear();
	hr	= spElem->getAttribute(CComBSTR(s_Program),		&value);
	if(FAILED(hr))				throw ErrorXML(IDS_XML_ERRREAD, hr, s_Slot, s_Program);
	if(value.vt != VT_NULL)
	{
		hr	= value.ChangeType(VT_I4);
		if(FAILED(hr))			throw ErrorXML(IDS_XML_ERRREAD, hr, s_Slot, s_Program);
		m_IsProgram		= value.lVal != 0;
	}

	value.Clear();
	hr	= spElem->getAttribute(CComBSTR(s_NumUMi),		&value);
	if(FAILED(hr))				throw ErrorXML(IDS_XML_ERRREAD, hr, s_Slot, s_NumUMi);
	if(value.vt != VT_NULL)
	{
		hr	= value.ChangeType(VT_I4);
		if(FAILED(hr))			throw ErrorXML(IDS_XML_ERRREAD, hr, s_Slot, s_NumUMi);
		m_NumUMi		= value.lVal;
	}

	value.Clear();
	hr	= spElem->getAttribute(CComBSTR(s_NumSLOTi),		&value);
	if(FAILED(hr))				throw ErrorXML(IDS_XML_ERRREAD, hr, s_Slot, s_NumSLOTi);
	if(value.vt != VT_NULL)
	{
		hr	= value.ChangeType(VT_I4);
		if(FAILED(hr))			throw ErrorXML(IDS_XML_ERRREAD, hr, s_Slot, s_NumSLOTi);
		m_NumSLOTi		= value.lVal;
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

			if( (static_cast<CString>(nodeName)).CompareNoCase(CLinkNetTab::s_TabLinkNet) == 0)
			{
				m_TabLinkNet.XMLLoad(child);
			}
			else if( (static_cast<CString>(nodeName)).CompareNoCase(CLinkCLLTab::s_TabLinkCLL) == 0)
			{
				m_TabLinkCLL.XMLLoad(child);
			}
			else if( (static_cast<CString>(nodeName)).CompareNoCase(CDevTab::s_TabDev) == 0)
			{
				m_TabDev.XMLLoad(child);
			}
		}

		CComPtr<IXMLDOMNode>	next;
		child->get_nextSibling(&next);
		if(FAILED(hr))			throw ErrorXML(IDS_XML_ERRNEXTCHILD, hr);
		child	= next;
	}
}

void	CSlot::XMLPostLoad()
{
	if(m_pParam) m_pParam->XMLLoad();

	m_TabLinkCLL.XMLPostLoad();

	for(list<CDev*>::iterator i = m_TabDev.begin(); i != m_TabDev.end(); ++i)
	{
		(*i)->XMLPostLoad();
	}
}

/////////////////////////////////////////////////////////////////////////////
// КЛШ, подключенные к УМ
//

CSlot*	CSlotTab::FindBlok(int aNumSLOT)
{
	for(list<CSlot*>::iterator i = begin(); i != end(); ++i)
	{
		if((*i)->m_NumSLOT == aNumSLOT) return *i;
	}
	return NULL;
}

void CSlotTab::GetSost()
{
	for(list<CSlot*>::iterator i = begin(); i != end(); ++i)
	{
		CSlot*	ItSlot = *i;

		if(ItSlot->m_TypSlot != CSlot::tsMon && ItSlot->m_TypSlot != CSlot::tsNETo && ItSlot->m_TypSlot != CSlot::tsNETi) ItSlot->GetSost();
	}
}

bool	CSlotTab::Insert(CSlot* aSlot)
{
	if(FindBlok(aSlot->m_NumSLOT) == NULL)
	{
		push_back(aSlot);
		return true;
	}
	else
	{
		return false;
	}
}

void CSlotTab::GetSostL()
{
	for(list<CSlot*>::iterator i = begin(); i != end(); ++i)
	{
		if((*i)->IsDevice()) (*i)->GetSostL();
	}
}

void CSlotTab::GetSostR()
{
	for(list<CSlot*>::iterator i = begin(); i != end(); ++i)
	{
		if((*i)->IsDevice()) (*i)->GetSostR();
	}
}

void CSlotTab::SetNoInp()
{
	for(list<CSlot*>::iterator i = begin(); i != end(); ++i)
	{
		if((*i)->IsDevice()) (*i)->SetNoInp();
	}
}


void CSlotTab::TestOffL()
{
	for(list<CSlot*>::iterator i = begin(); i != end(); ++i)
	{
		if((*i)->IsDevice()) (*i)->TestOffL();
	}
}

void CSlotTab::TestOffR()
{
	for(list<CSlot*>::iterator i = begin(); i != end(); ++i)
	{
		if((*i)->IsDevice()) (*i)->TestOffR();
	}
}

/////////////////////////////////////////////////////////////////////////////
// сравниватель <

struct LTpSlot : public binary_function<CSlot*, CSlot*, bool>
{ 
	bool operator()(const CSlot* _Left, const CSlot* _Right) const { return (_Left->m_NumSLOT < _Right->m_NumSLOT); }
};

void CSlotTab::ReOrder()	
{ 
	sort(LTpSlot());

	for(iterator i = begin(); i != end(); ++i)	(*i)->ReOrder();
}
