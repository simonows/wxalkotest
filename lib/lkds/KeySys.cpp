#include "StdAfx.h"
#include <KeySys.h>
#include <Idea.h>
#include <PackV7.h>

CKeySys::CKeySys(void)
{
	Clear();
}

CKeySys::~CKeySys(void)
{
}

void	CKeySys::Clear()
{
	memset(m_Buf, 0, sizeof(m_Buf));
}

void	CKeySys::Create(const TCHAR* aKey)
{
	CIdeaCtx	Idea;
	Idea.Key(CPackV7::ForKeySYS);

	memset(m_Buf, 0, sizeof(m_Buf));
	memcpy(m_Buf, static_cast<const char*>(CStringA(aKey)), min(_tcslen(aKey), sizeof(m_Buf)));

	Idea.Encrypt(m_Buf, m_Buf, 2);

	for(int i = 0; i < 8; i++)
	{
		m_Buf[i+8] ^= m_Buf[i];
	}
}

void	CKeySys::Set(const TCHAR* aKey)
{

	CString	Key(aKey);

	if(Key == _T("00000000000000000000000000000000"))
	{
		SetDefault();
	}
	else
	{
		for(int i = 0; i < len; i++)
		{
			int	Byte	= 0;

#if defined(WIN32_PLATFORM_PSPC) || defined(WIN32_PLATFORM_WFSP)
			_stscanf(Key.Mid(i*2,2), _T("%X"), &Byte);
#else
			_stscanf_s(Key.Mid(i*2,2), _T("%X"), &Byte);
#endif
			m_Buf[i] = Byte;
		}
	}
}

void	CKeySys::Set(const BYTE* aKey)
{
	memcpy(m_Buf, aKey, sizeof(m_Buf));
}

void	CKeySys::SetDefault()
{
	Set(_T("0102030405060708090A0B0C0D0E0F10"));
}

CAtlString	CKeySys::GetString()
{
	CString	buf;

	for(int i = 0; i < sizeof(m_Buf); i++)
	{
		buf.AppendFormat(_T("%02X"), m_Buf[i]);
	}
	return buf;
}

bool	CKeySys::IsEmpty()
{
	for(int i = 0; i < sizeof(m_Buf); i++)
	{
		if(m_Buf[i] != 0) return false;
	}
	return true;
}

bool CKeySys::operator == (const CKeySys& key) const
{
	return memcmp(m_Buf, key, len) == 0;
}

bool CKeySys::operator != (const CKeySys& key) const
{
	return memcmp(m_Buf, key, len) != 0;
}
