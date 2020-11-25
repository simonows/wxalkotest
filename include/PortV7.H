#pragma once

#include "PackV7.h"

/////////////////////////////////////////////////////////////////////////////
// обмен с ЛБ7 по COM порту

class CPortV7
{
	void			Wait(long aInervMcs);
	int				ReadCommBlock( BYTE* lpbyBlock, int nMaxLength );

	bool			m_bOpen;

	HANDLE			m_hPort;
	OVERLAPPED		m_OverlappedR;
	OVERLAPPED		m_OverlappedW;
	long			m_McsPerByte;
	CString			m_CommN;
	int				m_Parity;
	int				m_Speed;
	LARGE_INTEGER	m_Frequency;
	HANDLE			m_Lock;				// блокировочное событие

	HANDLE			volatile m_hCommInpThread;
	DWORD			m_dwThreadInpID;

	static	DWORD	WINAPI	CommInpProc( LPVOID lpData );
	static	DWORD	WINAPI	ReOpen( PVOID pvContext );

protected:
	bool			PutPort(CPackV7& aPack);

public:
	CPortV7(int aSpeed = CBR_115200) 
		: m_hPort(INVALID_HANDLE_VALUE)
		, m_bOpen(false)
		, m_McsPerByte(0)
		, m_hCommInpThread(0)
		, m_dwThreadInpID(0)
		, m_CommN(_T(""))
		, m_Parity(NOPARITY)
		, m_Speed(aSpeed)
	{m_Frequency.QuadPart=0; m_Lock = ::CreateEvent(NULL, TRUE, TRUE, NULL); };

	~CPortV7() {Close(); ::CloseHandle(m_Lock); };

	void			Open(const TCHAR* aPort);
	void			Close();
	bool			IsOpen()	{ return m_bOpen; };	// запусщен процесс передачи ГГС

	virtual	void	PutErr(const TCHAR* aMes = NULL, int aErr = -1) = 0;
	virtual	void	PutLogInp(const BYTE* aMes, int aLen)	{};
	virtual	void	PutLogOut(const BYTE* aMes, int aLen)	{};
	virtual	void	MngPack(CPackV7& aPack)	= 0;
	virtual	void	WaitEndThread(volatile HANDLE& hThread, const TCHAR* aMesErr);
};
