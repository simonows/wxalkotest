#include "stdafx.h"
#include "LoadRes.h"

CAtlString	_S_(UINT aID)
{
	extern HINSTANCE s_hInstRes;

	TCHAR	buf[1024];

	if(!::LoadString(s_hInstRes, aID, buf, _countof(buf)))
	{
		DWORD rc = ::GetLastError();
		_stprintf_s(buf, _countof(buf), _T("S%d"), aID);
	}
	return buf;
}
