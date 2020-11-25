/* MTestPro.h : main header file for the PROJECT_NAME application */

#pragma once

#include "../../src/MTestPro/res/resource.h"
#include <mtest/sendnet.h>
#include <mtest/receivnet.h>
#include <mtest/ggsnet.h>
#include <mtest/portlb.h>
#include <mtest/lktest_dlg.h>


/*
 * LogFile: - протокол обращений
*/
class LogFile : public ofstream
{
public:
    bool Open();
    bool Put(const TCHAR* aMes);
    bool PutErr(const TCHAR* aMes = NULL, int aErr = -1, bool aMessageBox = true);
    bool PutMes(const TCHAR* aCnt, const BYTE* aMes, int aLen, bool aMessageBox = false);
};

/*
 * CMTestProApp:
 * See MTestPro.cpp for the implementation of this class
*/
class CMTestProDlg;

class CMTestProApp : public CWinApp
{
public:
    CMTestProApp();

    CToolTipCtrl *m_gpToolTip;
    HWND m_hwndDialog;
    CString m_sPathStart;
    int m_IsLog;
    LogFile m_Log;
    CSendNet m_Send;
    CReceivNet m_Receiv;
    CGGSNet m_GGS;
    CPortLB m_PortLB;
    CMTestProDlg *m_pDlg;

    public:
    virtual BOOL InitInstance();
    virtual BOOL ProcessMessageFilter(int code, LPMSG lpMsg);

    void Mes(const TCHAR* aMes, UINT nType = MB_ICONSTOP);

    DECLARE_MESSAGE_MAP()
};

extern CMTestProApp theApp;

