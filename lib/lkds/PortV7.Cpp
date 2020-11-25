#include "stdafx.h"
#include "PortV7.h"
#include "XML.h"
#include "LoadRes.h"
#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// передача ГГС к ЛБ7

void CPortV7::Open(const TCHAR* aPort)
{
	try
	{
		m_CommN	= aPort;
		m_bOpen	= true;

		if(!QueryPerformanceFrequency(&m_Frequency))	m_Frequency.QuadPart= 0;

		m_OverlappedR.Internal		= 0;
		m_OverlappedR.InternalHigh	= 0;
		m_OverlappedR.Offset		= 0; 
		m_OverlappedR.OffsetHigh	= 0; 
		m_OverlappedR.hEvent		= CreateEvent(NULL,TRUE,FALSE,NULL);
		m_OverlappedW.Internal		= 0;
		m_OverlappedW.InternalHigh	= 0;
		m_OverlappedW.Offset		= 0; 
		m_OverlappedW.OffsetHigh	= 0; 
		m_OverlappedW.hEvent		= CreateEvent(NULL,TRUE,FALSE,NULL);

		m_hPort = ::CreateFile( _T("\\\\.\\")+m_CommN, 
							  GENERIC_READ|GENERIC_WRITE,
							  0,
							  NULL,
							  OPEN_EXISTING,
							  FILE_ATTRIBUTE_NORMAL |
							  FILE_FLAG_OVERLAPPED,
							  NULL); 

		if(m_hPort == INVALID_HANDLE_VALUE)			throw	ErrorXML(m_CommN+_S_(IDS_V7_ERRCONNCOM), GetLastError());

		BOOL		rc = 0;

		COMMTIMEOUTS  CommTimeOuts ;
		CommTimeOuts.ReadIntervalTimeout		= 0xFFFFFFFF ;
		CommTimeOuts.ReadTotalTimeoutMultiplier = 0 ;
		CommTimeOuts.ReadTotalTimeoutConstant	= 1000 ;
		CommTimeOuts.WriteTotalTimeoutMultiplier= 2 ;
		CommTimeOuts.WriteTotalTimeoutConstant	= 0 ;

		rc = SetCommMask( m_hPort, EV_RXCHAR ) ;
		rc = SetupComm( m_hPort, 4096, 4096 ) ;
		rc = PurgeComm( m_hPort, PURGE_TXABORT | PURGE_RXABORT |
								 PURGE_TXCLEAR | PURGE_RXCLEAR ) ;
		SetCommTimeouts( m_hPort, &CommTimeOuts ) ;

		DWORD	ModemStat = 0;

		GetCommModemStatus( m_hPort, &ModemStat);

		DCB dcb;
		dcb.DCBlength = sizeof(dcb);

		if(!GetCommState(m_hPort, &dcb))			throw	ErrorXML(_S_(IDS_V7_ERRGETPARCOM), GetLastError());

		dcb.BaudRate			= m_Speed;
		dcb.ByteSize			= 8;
		dcb.Parity				= m_Parity;

		dcb.StopBits			= ONESTOPBIT;
		dcb.fBinary				= TRUE;
		dcb.fParity				= TRUE;

		m_McsPerByte			= (1100*CBR_9600)/dcb.BaudRate;

		if(!SetCommState(m_hPort, &dcb))			throw	ErrorXML(_S_(IDS_V7_ERRPARCOM), GetLastError());

		m_hCommInpThread = CreateThread(	NULL, 0, 
											(LPTHREAD_START_ROUTINE) CommInpProc,
											this, 0, &m_dwThreadInpID);

		if(!m_hCommInpThread)						throw	ErrorXML(_S_(IDS_V7_ERRSTARTCOM), GetLastError());
	}
	catch(ErrorXML aErr)
	{
		Close();
		throw ErrorXML(_T("CPortV7::Open - ")+aErr.m_Message, aErr.m_Hr);
	}
}

void CPortV7::Close()
{
	if(m_bOpen)
	{
		m_bOpen	= false;

		if(m_hPort != INVALID_HANDLE_VALUE) SetCommMask( m_hPort, 0 ) ;

		WaitEndThread(m_hCommInpThread, _T("CPortV7::CommInpProc"));

		if(m_hPort != INVALID_HANDLE_VALUE)
		{
			EscapeCommFunction( m_hPort, CLRDTR ) ;
			PurgeComm( m_hPort, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR ) ;
			CloseHandle( m_hPort ) ;

		}
		CloseHandle( m_OverlappedR.hEvent);
		CloseHandle( m_OverlappedW.hEvent);
	}
	m_hPort	= INVALID_HANDLE_VALUE;
}

void CPortV7::WaitEndThread(volatile HANDLE& hThread, const TCHAR* aMesErr)
{
	while(hThread)	Sleep(1);
}

void CPortV7::Wait(long aInervMcs)
{
	if(m_Frequency.QuadPart != 0)
	{
		LARGE_INTEGER CountEnd;
		LARGE_INTEGER CountCur;

		QueryPerformanceCounter(&CountCur);

		CountEnd.QuadPart = CountCur.QuadPart + ((aInervMcs*m_Frequency.QuadPart)/1000000);

		do
		{
			QueryPerformanceCounter(&CountCur);
		} while(CountCur.QuadPart < CountEnd.QuadPart);
	}
	else
	{
		Sleep((aInervMcs+500)/1000);
	}
}

bool CPortV7::PutPort(CPackV7& aPack)
{
	if(m_bOpen)
	{
		BYTE		outBuf[1032];
		DWORD		NumberOfBytesWritten = 0;

		outBuf[0]	= 0xFF;
		outBuf[1]	= (aPack.LenFul()>>8)&0xFF;
		outBuf[2]	=  aPack.LenFul()&0xFF;

		aPack.AddRepl(0);
		aPack.SetIdent(0);
		DWORD	NumberOfBytesToWrite = aPack.ChanCod(&outBuf[3], sizeof(outBuf)-3, CPackV7::DefaultKey)+3;

		::WaitForSingleObject(m_Lock, 32);

		PutLogOut(aPack,  aPack.LenCRC());
		//PutLogOut(outBuf, NumberOfBytesToWrite);

		if(!WriteFile( m_hPort, outBuf, NumberOfBytesToWrite, &NumberOfBytesWritten, &m_OverlappedW))
		{
			int	err = GetLastError();
			if(err != ERROR_IO_PENDING)
			{
				PutErr(_S_(IDS_V7_ERRPUTCOM));
				return false;
			}
		}
		Wait( (m_McsPerByte*NumberOfBytesToWrite) + 4000);

		if(	aPack.m_ClassPack	== CPackV7::cpData		&&
			aPack.m_TypPack		== CPackV7::tpDirect	&&
			aPack.m_TypAsk		== CPackV7::taRestart	&&
			aPack.m_Direct		== CPackV7::tdQuery)
		{
			if(!::QueueUserWorkItem((LPTHREAD_START_ROUTINE)ReOpen,  this, WT_EXECUTELONGFUNCTION))
			{
				PutErr(_T("::QueueUserWorkItem(ReOpen)"), GetLastError());
			}
		}
	}
	return true;
}

int CPortV7::ReadCommBlock( BYTE* lpbyBlock, int nMaxLength )
{
	COMSTAT    ComStat ;
	DWORD      dwErrorFlags	= 0;
	DWORD      dwLength		= 0;
	DWORD      dwError		= 0;

	ClearCommError( m_hPort, &dwErrorFlags, &ComStat ) ;
	dwLength = min( (DWORD) nMaxLength, ComStat.cbInQue ) ;

	if (dwLength > 0)
	{
		if(!ReadFile( m_hPort, lpbyBlock, dwLength, &dwLength, &m_OverlappedR ))
		{
			if ( (dwError = GetLastError()) == ERROR_IO_PENDING)
			{
#if !(defined(WIN32_PLATFORM_PSPC) || defined(WIN32_PLATFORM_WFSP))
				while(!GetOverlappedResult( m_hPort, &m_OverlappedR, &dwLength, TRUE ))
				{
					dwError = GetLastError();

					if(dwError != ERROR_IO_INCOMPLETE)
					{
						ClearCommError( m_hPort, &dwErrorFlags, &ComStat ) ;
						break;
					}
				}
#endif
			}
			else
			{
			    dwLength = 0 ;
				ClearCommError( m_hPort, &dwErrorFlags, &ComStat ) ;
			}
		}
	}
   	return ( dwLength ) ;
}

DWORD WINAPI CPortV7::CommInpProc( LPVOID lpData )
{
	int			nLength		= 0;
	BYTE		abIn[ 1024 + 1] ;
	int			len			= 0;
	int			lenPack		= 0;
	BYTE		buf[ 1024 + 1] ;
	CPortV7*	pPort		= (CPortV7*)lpData;
	time_t		lastINP		= 0;

	while(pPort->m_bOpen)
	{
		DWORD	dwEvtMask	= 0;

		if(WaitCommEvent( pPort->m_hPort, &dwEvtMask, NULL ))
		{
			if(dwEvtMask & EV_RXCHAR)
			{
				while((nLength = pPort->ReadCommBlock( abIn, sizeof(abIn))) > 0)
				{
					if( (time(0) - lastINP) > 1)	len	= 0;

					lastINP	= time(0);

					for( int i = 0; i < nLength; i++)
					{
						if(len < sizeof(buf))
						{
							if(len == 0)
							{
								if(abIn[i] != 0xFF) continue;

								::ResetEvent(pPort->m_Lock);	// запрет вывода в COM порт на время приема пакета

								buf[len++]	= abIn[i];
							}
							else if(len == 1)
							{
								buf[len++]	= abIn[i];
							}
							else if(len == 2)
							{
								buf[len++]	= abIn[i];
								lenPack	= (((buf[len-2]<<8)&0xFF00)|(buf[len-1]&0xFF)) + 3;

								if(lenPack > 0x400 + 3)
								{
									len		= 0;
									lenPack	= 0;
									::SetEvent(pPort->m_Lock);	// разрешение вывода в COM порт
									continue;

								}
							}
							else
							{
								if(len < lenPack)
								{
									buf[len++]	= abIn[i];

									if(len == lenPack)
									{
										::SetEvent(pPort->m_Lock);	// разрешение вывода в COM порт

										CPackV7	Pack;

										//pPort->PutLogInp(buf, len);

										if(Pack.ChanDecod(&buf[3], len-3, CPackV7::DefaultKey))
										{
											pPort->PutLogInp(Pack,  Pack.LenCRC());
											pPort->MngPack(Pack);
										}
										len	= 0;
									}
								}
							}
						}
						else
						{
							len		= 0;
							lenPack	= 0;
							::SetEvent(pPort->m_Lock);	// разрешение вывода в COM порт
						}
					}
				}
			}
			else
			{
				len		= 0;
				lenPack	= 0;
				::SetEvent(pPort->m_Lock);	// разрешение вывода в COM порт
			}
		}
		else
		{
			pPort->PutErr(_S_(IDS_V7_ERRGETCOM));
			break;
		}
	}
	CloseHandle(pPort->m_hCommInpThread);
	pPort->m_hCommInpThread	= NULL;
	pPort->m_dwThreadInpID	= 0;

	return( TRUE ) ;
}

DWORD	WINAPI	CPortV7::ReOpen( PVOID pvContext )
{
	CPortV7*	pPort = static_cast<CPortV7*>(pvContext);

	pPort->Close();

	int		Err	= 0;
	CString	Message;

	//Sleep(5*1000);
	Sleep(7*1000);

	try
	{
		pPort->Open(pPort->m_CommN);
	}
	catch(ErrorXML aErr)
	{
		pPort->PutErr(aErr.m_Message, aErr.m_Hr);
	}
	return TRUE;
}
