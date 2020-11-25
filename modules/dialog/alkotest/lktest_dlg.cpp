
#include "../stdafx.h"
#include <mtest/mtestpro.h>
#include <mtest/lktest_dlg.h>
#include "afxdialogex.h"


// диалоговое окно CLKTestDlg2
IMPLEMENT_DYNAMIC(CLKTestDlg, CDialog)

CLKTestDlg::CLKTestDlg()
: CDialog(CLKTestDlg::IDD)
{
    //return TRUE;
    //AddPage(&mop2);
}

CLKTestDlg::~CLKTestDlg()
{
}

void CLKTestDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
}

BOOL CLKTestDlg::OnInitDialog()
{
    SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME), FALSE);
    m_wndMenu.LoadMenu(IDR_MENU1);
    SetMenu(&m_wndMenu);
    //((CTabCtrl*)GetDlgItem(IDC_TAB2))->AddPage(&m_op1);
    TC_ITEM tci;

    tci.mask = TCIF_TEXT;
    tci.iImage = -1;
    tci.pszText = "ЛБ v7.2";

    ((CTabCtrl*)GetDlgItem(IDC_TAB2))->InsertItem(0, &tci);
    tci.pszText = "АТС 4x4";
    ((CTabCtrl*)GetDlgItem(IDC_TAB2))->InsertItem(1, &tci);

    tci.mask = TCIF_PARAM;
    tci.lParam = (LPARAM)&m_op1;
    ((CTabCtrl*)GetDlgItem(IDC_TAB2))->SetItem(0, &tci);

    m_op1.Create(CMTestProDlg::IDD, ((CTabCtrl*)GetDlgItem(IDC_TAB2)));
    m_op1.SetWindowPos(NULL, 0, 21, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
    m_op1.ShowWindow(SW_SHOW);

    tci.mask = TCIF_PARAM;
    tci.lParam = (LPARAM)&m_op2;
    ((CTabCtrl*)GetDlgItem(IDC_TAB2))->SetItem(1, &tci);

    m_op2.Create(CATC4x4Dlg::IDD, ((CTabCtrl*)GetDlgItem(IDC_TAB2)));
    m_op2.SetWindowPos(NULL, 0, 21, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
    m_op2.ShowWindow(SW_HIDE);

    return TRUE;
}

void CLKTestDlg::OnSelchangeTab(NMHDR* pNMHDR, LRESULT* pResult)
{
    int current = ((CTabCtrl*)GetDlgItem(IDC_TAB2))->GetCurSel();
    m_op1.ShowWindow(current == 0 ? SW_SHOW : SW_HIDE);
    m_op2.ShowWindow(current == 1 ? SW_SHOW : SW_HIDE);

    *pResult = 0;
}

BEGIN_MESSAGE_MAP(CLKTestDlg, CDialog)
    ON_NOTIFY(TCN_SELCHANGE, IDC_TAB2, OnSelchangeTab)
    ON_COMMAND(ID_MENU_FILE_EXIT, &CLKTestDlg::OnExit)
    ON_COMMAND(ID_MENU_SUPPORT_ABOUT, &CLKTestDlg::OnAbout)
    ON_COMMAND(ID_MENU_SETTINGS_PARAM, &CLKTestDlg::OnParam)
    ON_WM_SIZE()
END_MESSAGE_MAP()


void CLKTestDlg::OnSize(UINT nType, int cx, int cy)
{
    CDialog::OnSize(nType, cx, cy);

    if (GetDlgItem(IDC_TAB2) != NULL)
    {
        ((CTabCtrl*)GetDlgItem(IDC_TAB2))->MoveWindow(0, 0, cx, cy);
        m_op1.SetWindowPos(NULL, 0, 21, cx, cy, SWP_NOZORDER);
        m_op2.SetWindowPos(NULL, 0, 21, cx, cy, SWP_NOZORDER);
    }
}

afx_msg void CLKTestDlg::OnExit()
{
    exit(1);
}

afx_msg void CLKTestDlg::OnAbout()
{
    CAboutDlg dlgAbout;
    dlgAbout.DoModal();
}

//extern CSettingsDlg* dlgSettings;

afx_msg void CLKTestDlg::OnParam()
{
    m_op1.on_settings();
}
