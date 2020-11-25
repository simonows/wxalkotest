#include "../../src/MTestPro/res/resource.h"

class CAboutDlg : public CDialog
{
public:
    CAboutDlg();
    CString m_Version;

    enum { IDD = IDD_ABOUTBOX };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);

    virtual BOOL OnInitDialog();
    DECLARE_MESSAGE_MAP()
};

