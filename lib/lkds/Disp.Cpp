#include "stdafx.h"

#include <Disp.h>
#include <Xml.h>

const	TCHAR* CDisp::s_Disp	= _T("Disp");
const	TCHAR* CDisp::s_Number	= _T("Number");
const	TCHAR* CDisp::s_Name	= _T("Name");

/////////////////////////////////////////////////////////////////////////////
// класс диспетчерского пульта, вместе с подключенными к нему УМ
//

CDisp::CDisp()
{
	m_Number	= 0;
	m_sNaim.Empty();
	m_pParam	= NULL;
}

CDisp::~CDisp()
{
	if(m_pParam) delete m_pParam;
}

void	CDisp::Clear()
{
	m_sNaim.Empty();
	m_Number	= 0;
	if(m_pParam)
	{
		delete m_pParam;
		m_pParam	= 0;
	}
	m_TabPult.Clear();
}

void	CDisp::XMLUpdate()
{
	try
	{
		HRESULT	hr;
		CString	buf;

		CComQIPtr<IXMLDOMElement>	spElem	= m_XMLNode;
		if(!spElem)					throw ErrorXML(_T("Error query interfaces"), 0, s_Disp);

		hr	= spElem->setAttribute(CComBSTR(s_Number), CComVariant(m_Number));
		if(FAILED(hr))				throw ErrorXML(_T("Error addition"), hr, s_Disp, s_Number);

		hr	= spElem->setAttribute(CComBSTR(s_Name), CComVariant(m_sNaim));
		if(FAILED(hr))				throw ErrorXML(_T("Error addition"), hr, s_Disp, s_Name);

		if(m_pParam) m_pParam->XMLUpdate();
	}
	catch(ErrorXML aErr)
	{
		throw aErr;
	}
}

void	CDisp::XMLAdd()
{
	try
	{
		HRESULT	hr;

		CComQIPtr<IXMLDOMNode>	spOwnNode	= m_XMLDOM;
		if(!spOwnNode)				throw ErrorXML(_T("Error query interface IXMLDOMNode from IXMLDOMDocument"), 0);

		CComPtr<IXMLDOMNode>	spNewNode;
		hr	= m_XMLDOM->createNode(	CComVariant(NODE_ELEMENT),
									CComBSTR(s_Disp),
									NULL,
									&spNewNode);
		if(FAILED(hr))				throw ErrorXML(_T("Error create"), hr, s_Disp);

		hr	= spOwnNode->appendChild(spNewNode, &m_XMLNode);
		if(FAILED(hr))				throw ErrorXML(_T("Error addition"), hr, s_Disp);
		if(!m_XMLNode)				throw ErrorXML(_T("Error addition"), hr, s_Disp);

		XMLUpdate();
	}
	catch(ErrorXML aErr)
	{
		throw aErr;
	}
}

void	CDisp::XMLLoad(CComPtr<IXMLDOMNode> spNode)
{
	try
	{
		m_XMLNode	= spNode;	

		CComQIPtr<IXMLDOMElement>	spElem	= spNode;
		if(!spElem)				throw ErrorXML(_T("Error query interfaces"), 0, s_Disp);

		HRESULT		hr;
		CComVariant	value;

		value.Clear();
		hr	= spElem->getAttribute(CComBSTR(s_Number),		&value);
		if(FAILED(hr))				throw ErrorXML(_T("Error read"), hr, s_Disp, s_Number);
		if(value.vt != VT_NULL)
		{
			hr	= value.ChangeType(VT_I4);
			if(FAILED(hr))			throw ErrorXML(_T("Error read"), hr, s_Disp, s_Number);
			m_Number	= value.lVal;
		}

		value.Clear();
		hr = spElem->getAttribute(CComBSTR(s_Name), &value);
		if(FAILED(hr))			throw ErrorXML(_T("Error read"), hr, s_Disp, s_Name);
		if(value.vt != VT_NULL)
		{
			m_sNaim = value; 
			m_sNaim.Trim();
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

				if( (static_cast<CString>(nodeName)).CompareNoCase(CPult::s_Pult) == 0)
				{
					CString	buf;
					CPult*	Pult	= new CPult(0);
					Pult->m_pDisp	= this;

					Pult->XMLLoad(child);

					CPult*	Exist	= m_TabPult.FindPult(Pult->m_Number);

					if(Exist)
					{
						buf.Format(_T("Subnet %d already exist in structure")
							,Exist->m_Number);

						delete Pult;
						throw ErrorXML(buf, 0);
					}
					else
					{
						m_TabPult.Insert(Pult);
					}
				}
			}

			CComPtr<IXMLDOMNode>	next;
			child->get_nextSibling(&next);
			if(FAILED(hr))			throw ErrorXML(_T("CDisp::XMLLoad get_nextSibling"), hr);
			child	= next;
		}
	}
	catch(ErrorXML aErr)
	{
		throw aErr;
	}
}

void	CDisp::XMLPostLoad()
{
	try
	{

		if(m_pParam) m_pParam->XMLLoad();

		for(list<CPult*>::iterator i = m_TabPult.begin(); i != m_TabPult.end(); ++i)
		{
			(*i)->XMLPostLoad();
		}
	}
	catch(ErrorXML aErr)
	{
		throw aErr;
	}
}


void	CDisp::XMLInit()
{
	try
	{
		HRESULT	hr = m_XMLDOM.CoCreateInstance(__uuidof(DOMDocument));

		if(FAILED(hr) || !m_XMLDOM)	throw ErrorXML(_T("Error create XML parsers"), hr);

		CComPtr<IXMLDOMProcessingInstruction>	spProcInstr;
		hr	= m_XMLDOM->createProcessingInstruction(CComBSTR(_T("xml")),
													CComBSTR(_T("version=\"1.0\" encoding=\"windows-1251\"")),
													&spProcInstr);
		if(FAILED(hr))				throw ErrorXML(_T("createProcessingInstruction"), hr);

		CComPtr<IXMLDOMNode>	spInsProcInst;
		hr	= m_XMLDOM->appendChild(spProcInstr, &spInsProcInst);
		if(FAILED(hr))				throw ErrorXML(_T("appendChild"), hr);
		if(!spInsProcInst)			throw ErrorXML(_T("appendChild"), hr);
	}
	catch(ErrorXML aErr)
	{
		throw aErr;
	}
}

void	CDisp::XMLRelease()
{
	for(list<CPult*>::iterator i = m_TabPult.begin(); i != m_TabPult.end(); ++i)
	{
		(*i)->XMLRelease();
	}
	if(!(!m_XMLNode)) m_XMLNode.Release();
	if(!(!m_XMLDOM))  m_XMLDOM.Release();
}

void	CDisp::XMLSaveFile(const TCHAR* aFileName, bool aIsUnicode)
{
	/*try
	{
		CString						Encoding	= aIsUnicode?_T("Unicode"):_T("windows-1251");
		//CComBSTR					bstrXML( "" );
		CComPtr<IXMLDOMDocument>	XMLDOMSave;


		HRESULT	hr = XMLDOMSave.CoCreateInstance(__uuidof(DOMDocument));

		if(FAILED(hr) || !XMLDOMSave)	throw ErrorXML(_T("Error create XML parsers from save"), hr);

		hr	= m_XMLDOM->get_xml(&bstrXML );
		if(FAILED(hr))				throw ErrorXML(_T("Error read XML string"), hr);

		CString sXml = bstrXML;
		int		Last = 0;
		CString	StCont = _T("");
		int		Pos = sXml.Find(_T("><"), 0);

		while(Pos != -1) 
		{
			StCont += sXml.Mid(Last, Pos - Last+1)+_T("\n");
			Last	= Pos+1;
			Pos		= sXml.Find(_T("><"), Pos+1);
		}
		StCont += sXml.Mid(Last, sXml.GetLength()-Last);

		bstrXML = StCont;
		
		VARIANT_BOOL	bSuccess;
		hr = XMLDOMSave->loadXML(bstrXML, &bSuccess);
		if(FAILED(hr))				throw ErrorXML(_T("loadXML"), hr);
		if(!bSuccess)				throw ErrorXML(_T("loadXML"), hr);

		CComPtr<IXMLDOMNode>	instr;
		hr = XMLDOMSave->get_firstChild(&instr);
		if(FAILED(hr))				throw ErrorXML(_T("get_firstChild"), hr);
		if(!instr)					throw ErrorXML(_T("get_firstChild"), hr);

		CComPtr<IXMLDOMProcessingInstruction>	spProcInstr;
		hr	= XMLDOMSave->createProcessingInstruction(CComBSTR(_T("xml")),
													CComBSTR(_T("version=\"1.0\" encoding=\"")+Encoding+_T("\"")),
													&spProcInstr);
		if(FAILED(hr))				throw ErrorXML(_T("createProcessingInstruction"), hr);

		CComPtr<IXMLDOMNode>	spUpdProcInst;
		hr	= XMLDOMSave->replaceChild(spProcInstr, instr, &spUpdProcInst);
		if(FAILED(hr))				throw ErrorXML(_T("replaceChild"), hr);
		if(!spUpdProcInst)			throw ErrorXML(_T("replaceChild"), hr);

		hr	= XMLDOMSave->save(CComVariant(aFileName));
		if(FAILED(hr))				throw ErrorXML(_T("Error save XML configuration"), hr);

		ClearModify();
	}
	catch(ErrorXML aErr)
	{
		throw aErr;
	}*/
}

void	CDisp::XMLSaveStr(CComBSTR& aXmlStr)
{
	try
	{
		HRESULT	hr	= m_XMLDOM->get_xml(&aXmlStr);
		if(FAILED(hr))				throw ErrorXML(_T("Error read XML string"), hr);
	}
	catch(ErrorXML aErr)
	{
		throw aErr;
	}
}

void	CDisp::XMLLoad()
{
	try
	{
		HRESULT	hr;

		m_TabPult.Clear();

		CComPtr<IXMLDOMNode>	child;
		hr = m_XMLDOM->get_firstChild(&child);
		if(FAILED(hr))				throw ErrorXML(_T("get_firstChild"), hr);

		while(!(!child))
		{
			DOMNodeType	tp;

			child->get_nodeType(&tp);

			if (tp == NODE_ELEMENT)
			{
				CComBSTR	nodeName;

				child->get_nodeName(&nodeName);

				if( (static_cast<CString>(nodeName)).CompareNoCase(s_Disp) == 0)
				{
					XMLLoad(child);
					break;
				}
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

void	CDisp::XMLLoadFile(const TCHAR* aFileName)
{
	try
	{
		HRESULT			hr;
		VARIANT_BOOL	bSuccess;
		hr = m_XMLDOM->load(CComVariant(aFileName), &bSuccess);
		if(FAILED(hr))				throw ErrorXML(_T("m_XMLDOM->load"), hr);
		if(!bSuccess)				throw ErrorXML(_T("m_XMLDOM->load"), hr);

		//if(FAILED(hr) || !bSuccess) 01.07.2015
		//{
		//	CComBSTR	bstrXML(_T("<?xml version=\"1.0\"?><Disp Name=\"")+m_sNaim+_T("\" Number=\"0\"></Disp>"));
		//	hr = m_XMLDOM->loadXML(bstrXML, &bSuccess);

		//	if(FAILED(hr))			throw ErrorXML(_T("m_XMLDOM->loadXML"), hr);
		//	if(!bSuccess)			throw ErrorXML(_T("m_XMLDOM->loadXML"), hr);
		//}

		XMLLoad();
	}
	catch(ErrorXML aErr)
	{
		throw aErr;
	}
}

void	CDisp::XMLLoadStr(const TCHAR* aXmlStr)
{
	try
	{
		CComBSTR		bstrXML	= aXmlStr;
		HRESULT			hr;
		VARIANT_BOOL	bSuccess;
		hr = m_XMLDOM->loadXML(bstrXML, &bSuccess);
		if(FAILED(hr))				throw ErrorXML(_T("m_XMLDOM->loadXML"), hr);
		if(!bSuccess)				throw ErrorXML(_T("m_XMLDOM->loadXML"), hr);

		XMLLoad();
	}
	catch(ErrorXML aErr)
	{
		throw aErr;
	}
}

bool	CDisp::IsModify()
{
	if(m_FlagModify) return true;

	for(list<CPult*>::iterator i = m_TabPult.begin(); i != m_TabPult.end(); ++i)
	{
		CPult*	Pult	= *i;
		if(Pult->IsModify()) return true;
	}
	return false;
}

void	CDisp::ClearModify()
{
	m_FlagModify = false;

	for(list<CPult*>::iterator i = m_TabPult.begin(); i != m_TabPult.end(); ++i)
	{
		CPult*	Pult	= *i;
		Pult->ClearModify();
	}
}

CSlot*	CDisp::RootSlot(CPult* aPult, int aNumUM)
{
	return(aPult->m_TabUM.RootSlot(aNumUM));
}

int		CDisp::SetLink()
{
	for(list<CPult*>::iterator i = m_TabPult.begin(); i != m_TabPult.end(); ++i)
	{
		CPult*	Pult	= *i;
		Pult->m_TabUM.SetLink();
	}
	return 1;
}

CPult*	CDisp::FindPult(int aNumber)
{
	for(list<CPult*>::iterator p = m_TabPult.begin(); p != m_TabPult.end(); ++p)
	{
		if((*p)->m_Number == aNumber) return *p;
	}
	return 0;
}

CUM*	CDisp::FindUM(int aSubNet, int aNumUM)
{
	CPult*	pPult	= FindPult(aSubNet);

	return pPult? pPult->m_TabUM.FindUM(aNumUM): 0;
}

CSlot*	CDisp::FindBlok(int aSubNet, int aNumUM, int aNumSLOT)
{
	CPult*	pPult	= FindPult(aSubNet);

	return pPult? pPult->m_TabUM.FindBlok(aNumUM, aNumSLOT): 0;
}

CDev*	CDisp::FindDev(int aSubNet, int aNumUM, int aNumSLOT, int aAdrBus)
{
	CPult*	pPult	= FindPult(aSubNet);

	return pPult? pPult->m_TabUM.FindDev(aNumUM, aNumSLOT, aAdrBus): 0;
}
