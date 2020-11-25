#pragma once

#include "../../src/MTestPro/stdafx.h"
#include "../../src/MTestPro/res/resource.h"

// диалоговое окно CATC4x4Dlg

class CATC4x4Dlg : public CDialog
{
    DECLARE_DYNAMIC(CATC4x4Dlg)

public:
    CATC4x4Dlg(CWnd* pParent = NULL);
    virtual ~CATC4x4Dlg();

    enum { IDD = IDD_ATC_4X4 };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);

    DECLARE_MESSAGE_MAP()
};

