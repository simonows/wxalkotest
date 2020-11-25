#pragma once

#include <PortV7.h>
#include <KeySys.h>

/////////////////////////////////////////////////////////////////////////////
// передача ГГС по для блока ЛБ6Pro

class CGGSV7: public CPortV7
{
	bool			PutSock(CPackV7& aPack);

	SOCKET			m_Socket;			// сокет для подключения (посылка звука)
	CIdentV7		m_IdentOWN;			// собственный идентификатор
	CIdentV7		m_IdentLB;			// идентификатор лифтового блока
	DWORD			m_AddrIPInp;		// прослушиваемый IP адрес
	DWORD			m_AddrIPOut;		// IP адрес ЛБ
	unsigned short	m_PortInp;			// прослушиваемый IP порт
	unsigned short	m_PortOut;			// IP порт ЛБ
	CKeySys			m_Key;				// ключ системы

	HANDLE			volatile m_hWinSockThread;
	DWORD			m_dwThreadWinSockID;

	static	DWORD	WINAPI	WinSockProc( CGGSV7* lpSrv );

public:
	CGGSV7() 
		: m_Socket(0)
		, m_hWinSockThread(0)
		, m_dwThreadWinSockID(0)
		, m_AddrIPInp(0)
		, m_AddrIPOut(0)
		, m_PortInp(0)
		, m_PortOut(0)
		{};

	~CGGSV7() {Stop();};

	virtual	void	MngPack(CPackV7& aPack);
	virtual	void	GetVolume(BYTE& aVolumIn, BYTE& aVolumOut);

	void			Start( const TCHAR* aPort, const CIdentV7& aIdentOWN, const CIdentV7& aIdentLB, DWORD aAddrIPInp, DWORD aAddrIPOut, unsigned short aPortInp, unsigned short aPortOut, const CKeySys& aKey);
	void			Stop();
	bool			IsStart()	{ return IsOpen(); };	// запусщен процесс передачи ГГС
	bool			PutCnv(CPackV7& aPack);				// вывод в конвертер
};
