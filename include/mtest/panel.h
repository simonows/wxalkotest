#pragma once

/*
 * CPanel
*/
class CMTestProDlg;

class CPanel: public CButton
{
    DECLARE_DYNAMIC(CPanel)

public:
    CPanel();
    virtual ~CPanel();

    void Init(CMTestProDlg*  pDlg);
    void DrawCount();
    virtual void DrawItem(LPDRAWITEMSTRUCT);


    CMTestProDlg *m_pDlg;

protected:
    DECLARE_MESSAGE_MAP()
};


