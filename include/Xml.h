#pragma once

////////////////////////////////////////////////////////////////////////////
// ErrorXML - описание исключительной ситуации при обработке XML структуры

class	ErrorXML
{
public:
	CString	m_Message;
	HRESULT	m_Hr;

	ErrorXML(const TCHAR* aMessage, HRESULT	hr)
		: m_Message(aMessage)
		, m_Hr(hr)
	{};
	ErrorXML(const TCHAR* aMessage, HRESULT	hr, const TCHAR* aElem);
	ErrorXML(const TCHAR* aMessage, HRESULT	hr, const TCHAR* aElem, const TCHAR* aParam);
	ErrorXML(UINT aMessage, HRESULT	hr);
	ErrorXML(UINT aMessage, HRESULT	hr, const TCHAR* aElem);
	ErrorXML(UINT aMessage, HRESULT	hr, const TCHAR* aElem, const TCHAR* aParam);
};

