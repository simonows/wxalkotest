#include <mtest/atc4x4dlg.h>


IMPLEMENT_DYNAMIC(CATC4x4Dlg, CDialog)

CATC4x4Dlg::CATC4x4Dlg(CWnd* pParent) : CDialog(CATC4x4Dlg::IDD)
{
}

CATC4x4Dlg::~CATC4x4Dlg()
{
}

void CATC4x4Dlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CATC4x4Dlg, CDialog)
END_MESSAGE_MAP()
