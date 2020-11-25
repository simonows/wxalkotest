#include "StdAfx.h"
#include "Xml.h"
#include "resource.h"

extern HINSTANCE s_hInstRes;

////////////////////////////////////////////////////////////////////////////
// ErrorXML - описание исключительной ситуации при обработке XML структуры

ErrorXML::ErrorXML(const TCHAR* aMessage, HRESULT	hr, const TCHAR* aElem) 
	: m_Hr(hr)
{
	TCHAR	format[256];

	if(!::LoadString(s_hInstRes, IDS_XML_ERR1, format, _countof(format)))	_tcscpy_s(format, _countof(format), _T("%s elements %s"));

	m_Message.Format(format, aMessage, aElem);
}

ErrorXML::ErrorXML(const TCHAR* aMessage, HRESULT	hr, const TCHAR* aElem, const TCHAR* aParam) 
	: m_Hr(hr)
{
	TCHAR	format[256];

	if(!::LoadString(s_hInstRes, IDS_XML_ERR2, format, _countof(format)))	_tcscpy_s(format, _countof(format), _T("%s parameters %s elements %s"));

	m_Message.Format(format, aMessage, aParam, aElem);
}

ErrorXML::ErrorXML(UINT aMessage, HRESULT	hr)
	: m_Hr(hr)
{
	TCHAR	message[256];

#if defined(WIN32_PLATFORM_PSPC) || defined(WIN32_PLATFORM_WFSP)
	if(!::LoadString(s_hInstRes, aMessage, message, _countof(message)))		_stprintf(message, _T("S%d"), aMessage);
#else
	if(!::LoadString(s_hInstRes, aMessage, message, _countof(message)))		_stprintf_s(message, _countof(message), _T("S%d"), aMessage);
#endif

	m_Message	= message;
}

ErrorXML::ErrorXML(UINT aMessage, HRESULT	hr, const TCHAR* aElem)
	: m_Hr(hr)
{
	TCHAR	message[256];
	TCHAR	format[256];

#if defined(WIN32_PLATFORM_PSPC) || defined(WIN32_PLATFORM_WFSP)
	if(!::LoadString(s_hInstRes, aMessage, message, _countof(message)))		_stprintf(message, _T("S%d"), aMessage);
#else
	if(!::LoadString(s_hInstRes, aMessage, message, _countof(message)))		_stprintf_s(message, _countof(message), _T("S%d"), aMessage);
#endif

	if(!::LoadString(s_hInstRes, IDS_XML_ERR1, format, _countof(format)))	_tcscpy_s(format, _countof(format), _T("%s elements %s"));

	m_Message.Format(format, message, aElem);
}
	
ErrorXML::ErrorXML(UINT aMessage, HRESULT	hr, const TCHAR* aElem, const TCHAR* aParam)
	: m_Hr(hr)
{
	TCHAR	message[256];
	TCHAR	format[256];

#if defined(WIN32_PLATFORM_PSPC) || defined(WIN32_PLATFORM_WFSP)
	if(!::LoadString(s_hInstRes, aMessage, message, _countof(message)))		_stprintf(message, _T("S%d"), aMessage);
#else
	if(!::LoadString(s_hInstRes, aMessage, message, _countof(message)))		_stprintf_s(message, _countof(message), _T("S%d"), aMessage);
#endif

	if(!::LoadString(s_hInstRes, IDS_XML_ERR2, format, _countof(format)))	_tcscpy_s(format, _countof(format), _T("%s parameters %s elements %s"));

	m_Message.Format(format, message, aParam, aElem);
}

