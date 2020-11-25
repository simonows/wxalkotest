#include "../stdafx.h"
#include <mtest/about_dlg.h>

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);

    DDX_Text(pDX, IDC_VERSION, m_Version);
}

BOOL CAboutDlg::OnInitDialog()
{
    TCHAR szAppName[_MAX_PATH];
    DWORD bufDW = 0;
    DWORD dwHandle = 0;

    ::GetModuleFileName(NULL, szAppName, _MAX_PATH);

    DWORD dwLen = GetFileVersionInfoSize(szAppName, &bufDW);
    LPVOID lpData = new BYTE[dwLen];

    if (dwLen <= 0)
    {
        goto exit;
    }

    if (GetFileVersionInfo(szAppName, dwHandle, dwLen, lpData))
    {
        VS_FIXEDFILEINFO* pInfo;
        UINT Len;

        if (VerQueryValue(lpData, _T("\\"), (LPVOID*)&pInfo, &Len))
        {
            WORD wMajorVersion = (WORD)((pInfo->dwProductVersionMS >> 16) & 0xFFFF);
            WORD wMinorVersion = (WORD)((pInfo->dwProductVersionMS) & 0xFFFF);
            WORD wBuildNumber = (WORD)((pInfo->dwProductVersionLS >> 16) & 0xFFFF);
            WORD wRevisionNumber = (WORD)((pInfo->dwProductVersionLS) & 0xFFFF);

            m_Version.Format(
                 _T("%d.%d.%d.%d")
                ,wMajorVersion
                ,wMinorVersion
                ,wBuildNumber
                ,wRevisionNumber
            );
        }
    }
    delete lpData;

exit:
    CDialog::OnInitDialog();

    return TRUE;
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

