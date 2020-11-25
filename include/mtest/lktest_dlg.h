#pragma once

#include "../../src/MTestPro/res/resource.h"
#include <mtest/about_dlg.h>
#include <mtest/mtestpro_dlg.h>
#include <mtest/atc4x4dlg.h>

// диалоговое окно CLKTestDlg2
class CLKTestDlg : public CDialog
{
    DECLARE_DYNAMIC(CLKTestDlg)

public:
    CLKTestDlg();
    virtual ~CLKTestDlg();
    CMTestProDlg m_op1;
    CATC4x4Dlg m_op2;

    enum { IDD = IDD_LKTEST_DLG };

protected:
    CMenu m_wndMenu;
    void OnSelchangeTab(NMHDR* pNMHDR, LRESULT* pResult);
    virtual BOOL OnInitDialog();
    virtual void DoDataExchange(CDataExchange* pDX);

    DECLARE_MESSAGE_MAP()

public:
    afx_msg void OnExit();
    afx_msg void OnAbout();
    afx_msg void OnParam();
    afx_msg void OnSize(UINT nType, int cx, int cy);
};

