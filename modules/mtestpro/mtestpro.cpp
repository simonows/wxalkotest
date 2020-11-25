#include "../stdafx.h"
#include <mtest/mtestpro.h>
#include <mtest/lktest_dlg.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


/* LogFile: - протокол обращений */
bool LogFile::Open()
{
    open(theApp.m_sPathStart+_T("MTestPro.Log"), ios::out | ios::app);
    return(good());
}

bool LogFile::Put(const TCHAR* aMes)
{
    if (good())
    {
        *this << CStringA(COleDateTime::GetCurrentTime().Format(_T("%d.%m.%Y %H:%M:%S ")))
              << CStringA(aMes)
              << endl;
    }
    return true;
}

bool LogFile::PutMes(const TCHAR* aCnt, const BYTE* aMes, int aLen, bool aMessageBox)
{
    CString buf(aCnt);
    bool rc = false;

    for (int i = 0; i < aLen; i++)
    {
        buf.AppendFormat(_T(" %02X"), ((int)aMes[i])&0xFF);
    }

    rc = Put(buf);

    if (aMessageBox)
    {
        AfxMessageBox(aCnt, MB_OK|MB_ICONSTOP);
    }

    return rc;
}

bool LogFile::PutErr(const TCHAR* aMes, int aErr, bool aMessageBox)
{
    LPVOID lpMsgBuf;
    CString buf = aMes == NULL ? _T("") : aMes;
    int err = aErr == -1 ? GetLastError() : aErr;

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, err,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL );

    buf.AppendFormat(_T(": %d (0x%x) - %s"), err, err, lpMsgBuf);
    LocalFree(lpMsgBuf);

    buf = buf.Left(buf.GetLength() - 1);

    bool rc = Put(buf);

    if (aMessageBox) AfxMessageBox(buf, MB_OK | MB_ICONSTOP);

    return rc;
}

/* обработка командной строки */
class CTestProLineInfo : public CCommandLineInfo
{
public:
    CTestProLineInfo();

    int m_IsLog; // протоколирование     [-l<0_или_1>]   = 0

    virtual void ParseParam( LPCTSTR lpszParam, BOOL bFlag, BOOL bLast );
};

CTestProLineInfo::CTestProLineInfo()
    : m_IsLog(0)
{
}

void CTestProLineInfo::ParseParam( LPCTSTR lpszParam, BOOL bFlag, BOOL bLast )
{
    if (bFlag)
    {
        if((lpszParam[0] == 'l') || (lpszParam[0] == 'L'))
        {
            m_IsLog = _ttoi(&lpszParam[1]);
        }
        else
        {
            CCommandLineInfo::ParseParam(lpszParam, bFlag, bLast);
        }
    }
    else
    {
        CCommandLineInfo::ParseParam(lpszParam, bFlag, bLast);
    }
}


BEGIN_MESSAGE_MAP(CMTestProApp, CWinApp)
END_MESSAGE_MAP()


CMTestProApp::CMTestProApp()
    : m_pDlg(0)
    , m_IsLog()
{
    // TODO: add construction code here,
    // Place all significant initialization in InitInstance
}


CMTestProApp    theApp;
HINSTANCE       s_hInstRes;


BOOL CMTestProApp::InitInstance()
{
    s_hInstRes = ::GetModuleHandle(0);

    // InitCommonControlsEx() is required on Windows XP if an application
    // manifest specifies use of ComCtl32.dll version 6 or later to enable
    // visual styles.  Otherwise, any window creation will fail.
    INITCOMMONCONTROLSEX InitCtrls;
    InitCtrls.dwSize = sizeof(InitCtrls);
    // Set this to include all the common control classes you want to use
    // in your application.
    InitCtrls.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&InitCtrls);

    CWinApp::InitInstance();

    AfxEnableControlContainer();

    SetRegistryKey(_T("LKDS"));
    {
        TCHAR szAppName[_MAX_PATH];
        TCHAR drive[_MAX_DRIVE];
        TCHAR dir[_MAX_DIR];

        ::GetModuleFileName(NULL, szAppName, _MAX_PATH);

        _tsplitpath_s(
             szAppName
            ,drive
            ,sizeof(drive) / sizeof(TCHAR)
            ,dir
            ,sizeof(dir) / sizeof(TCHAR)
            ,NULL
            ,0
            ,NULL
            ,0
        );
        _tmakepath_s(
             szAppName
            ,sizeof(szAppName)/sizeof(TCHAR)
            ,drive
            ,dir
            ,NULL
            ,NULL
        );

        m_sPathStart = szAppName;
    }
    m_Log.Open();
    m_Log.Put(CString("Start ") + GetCommandLine());

    CTestProLineInfo cmdInfo;
    ParseCommandLine(cmdInfo);

    m_IsLog = cmdInfo.m_IsLog;

    WSADATA wsaData;
    int rc = 0;

    if((rc = WSAStartup(0x202, &wsaData)) != 0) m_Log.PutErr(_T("WSAStartup"), rc);

    CLKTestDlg dlg;
    m_pDlg = &dlg.m_op1;
    m_pMainWnd = &dlg.m_op1;
    INT_PTR nResponse = dlg.DoModal();

    if (WSACleanup())
    {
        m_Log.PutErr(_T("WSACleanup"), WSAGetLastError());
    }

    m_Log.Put(CString("Stop ") + GetCommandLine());
    return FALSE;
}

BOOL CMTestProApp::ProcessMessageFilter(int code, LPMSG lpMsg)
{
    if(m_hwndDialog != NULL)
    {
        if(lpMsg->hwnd == m_hwndDialog || ::IsChild(m_hwndDialog, lpMsg->hwnd))
        {
            if(m_gpToolTip != NULL) m_gpToolTip->RelayEvent(lpMsg);
        }
    }
    return CWinApp::ProcessMessageFilter(code, lpMsg);
}

void CMTestProApp::Mes(LPCTSTR aMes, UINT nType)
{
    m_Log.Put(aMes);
    AfxMessageBox(aMes, MB_OK | nType);
}

