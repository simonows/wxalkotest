#include "../stdafx.h"
#include <mtest/mtestpro.h>
#include <mtest/mtestpro_dlg.h>
#include <mtest/panel.h>


IMPLEMENT_DYNAMIC(CPanel, CButton)

CPanel::CPanel()
{
    m_pDlg = NULL;
}

CPanel::~CPanel()
{
}

BEGIN_MESSAGE_MAP(CPanel, CButton)
END_MESSAGE_MAP()


void CPanel::Init(CMTestProDlg* pDlg)
{
    m_pDlg = pDlg;
}

void CPanel::DrawCount()
{
    CDC* pDC = GetDC();
    CRect r;
    CFont f;

    if(pDC == NULL)
    {
        goto exit;
    }

    int xLogPixPerInch = pDC->GetDeviceCaps(LOGPIXELSX);
    const int WightP = MulDiv(300, xLogPixPerInch, 2540);

    GetClientRect(&r);

    r.top += 10;
    r.bottom -= 40;
    r.left += 35;
    r.right -= 10;
    const int high = r.bottom-r.top;
    const int highOne = high/CHistSostArray::maxLenHist;


    f.CreateFont(WightP, 0, 0, 0, FW_BOLD, 0, 0, 0, RUSSIAN_CHARSET, OUT_TT_PRECIS,
                CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, _T("Arial"));

    CFont* pOldFont = pDC->SelectObject(&f);

    pDC->SetBkColor(RGB(255, 255, 255));

    CPoint pnt(r.left, r.bottom -highOne);
    int l = m_pDlg->m_Hist.GetSize() - 1;

    if (l >= 0)
    {
        CString buf;

        buf.Format(_T("%d"), m_pDlg->m_Hist[l].m_Count);
        pDC->SetTextAlign(TA_RIGHT|TA_TOP);
        pDC->TextOut(pnt.x, pnt.y, buf);
    }

exit:
    pDC->SelectObject(pOldFont);
    f.DeleteObject();
    ReleaseDC(pDC);
}

void CPanel::DrawItem(LPDRAWITEMSTRUCT lpDIS)
{
    CDC* pDC = CDC::FromHandle(lpDIS->hDC);
    int xLogPixPerInch = pDC->GetDeviceCaps(LOGPIXELSX);
    const int WightP = MulDiv(300, xLogPixPerInch, 2540);
    CRect r = lpDIS->rcItem;
    r.top += 10;
    r.bottom -= 40;
    r.left += 35;
    r.right -= 10;
    const int high = r.bottom - r.top;
    const int wight = r.right -r.left;
    const int highOne = high/CHistSostArray::maxLenHist;
    const int wightOne = wight / (m_pDlg->m_bModeUB ? 24: 21);
    //CSize sizeOne(wightOne/2, highOne-1);
    CSize sizeOne((wightOne*3)/4, highOne-1);
    CString buf;

    CPen p1(PS_SOLID, 1, RGB(0,0,0));
    //CPen p2(PS_DASH, 1, RGB(128,128,128));
    CPen p2(PS_DOT, 1, RGB(128,128,128));
    CBrush w(RGB(255,255,255));
    CBrush b(RGB(0,0,0));
    CFont f1;
    CFont f2;

    f1.CreateFont(
         WightP
        ,0
        ,0
        ,0
        ,FW_BOLD
        ,0
        ,0
        ,0
        ,RUSSIAN_CHARSET
        ,OUT_TT_PRECIS
        ,CLIP_DEFAULT_PRECIS
        ,DEFAULT_QUALITY
        ,DEFAULT_PITCH
        ,_T("Arial")
    );

    f2.CreateFont(
         WightP
        ,0
        ,900
        ,900
        ,FW_BOLD
        ,0
        ,0
        ,0
        ,RUSSIAN_CHARSET
        ,OUT_TT_PRECIS
        ,CLIP_DEFAULT_PRECIS
        ,DEFAULT_QUALITY
        ,DEFAULT_PITCH
        ,_T("Arial")
    );

    CPen *pOldPen = pDC->SelectObject(&p1);
    CBrush *pOldBrush = pDC->SelectObject(&w);
    CFont *pOldFont = pDC->SelectObject(&f1);

    pDC->Rectangle(&(lpDIS->rcItem));
    pDC->Rectangle(&(r));

    pDC->SelectObject(&b);

    pDC->SelectObject(&p2);
    for (int j = 1; j < CHistSostArray::maxLenHist; j++)
    {
        pDC->MoveTo(r.left,  r.bottom -(highOne*j)-1);
        pDC->LineTo(r.right, r.bottom -(highOne*j)-1);
    }
    for (int l = 1; l <= 3; l++)
    {
        pDC->MoveTo(r.left+(wightOne*6*l)-2,  r.bottom);
        pDC->LineTo(r.left+(wightOne*6*l)-2,  r.top);
    }

    pDC->SetBkColor(RGB(255,255,255));
    pDC->SelectObject(&p1);

    int totSost = min(m_pDlg->m_Hist.GetSize(), CHistSostArray::maxLenHist);

    for (int i = 0; i < totSost; i++)
    {
        CPoint pnt(r.left, r.bottom -(highOne * (i + 1)));
        int l = m_pDlg->m_Hist.GetSize() - i - 1;

        if (l >= 0)
        {
            buf.Format(_T("%d"), m_pDlg->m_Hist[l].m_Count);
            pDC->SetTextAlign(TA_RIGHT|TA_TOP);
            pDC->TextOut(pnt.x, pnt.y, buf);
        }

        if (m_pDlg->m_Hist[l].m_bValid)
        {
            if (m_pDlg->m_bModeUB)
            {
                for (int i = 0; i < 24; i++)
                {
                    if (m_pDlg->m_Hist[l].m_SostUB.tstBit(i))
                    {
                        pDC->Rectangle(&(CRect(pnt,sizeOne)));
                    }
                    pnt.x += wightOne;
                }
            }
            else
            {
                if (m_pDlg->m_Hist[l].m_Sost.tstBit(CSostSens::bSnBP)) pDC->Rectangle(&(CRect(pnt,sizeOne)));
                pnt.x += wightOne;
                if (m_pDlg->m_Hist[l].m_Sost.tstBit(CSostSens::bSnSTOP1)) pDC->Rectangle(&(CRect(pnt,sizeOne)));
                pnt.x += wightOne;
                if (m_pDlg->m_Hist[l].m_Sost.tstBit(CSostSens::bSnSTOP2)) pDC->Rectangle(&(CRect(pnt,sizeOne)));
                pnt.x += wightOne;
                if (m_pDlg->m_Hist[l].m_Sost.tstBit(CSostSens::bSnDK1)) pDC->Rectangle(&(CRect(pnt,sizeOne)));
                pnt.x += wightOne;
                if (m_pDlg->m_Hist[l].m_Sost.tstBit(CSostSens::bSnDK2)) pDC->Rectangle(&(CRect(pnt,sizeOne)));
                pnt.x += wightOne;
                if (m_pDlg->m_Hist[l].m_Sost.tstBit(CSostSens::bSnRKD)) pDC->Rectangle(&(CRect(pnt,sizeOne)));
                pnt.x += wightOne;
                if (m_pDlg->m_Hist[l].m_Sost.tstBit(CSostSens::bSnROD)) pDC->Rectangle(&(CRect(pnt,sizeOne)));
                pnt.x += wightOne;
                if (m_pDlg->m_Hist[l].m_Sost.tstBit(CSostSens::bSnRZD)) pDC->Rectangle(&(CRect(pnt,sizeOne)));
                pnt.x += wightOne;
                if (m_pDlg->m_Hist[l].m_Sost.tstBit(CSostSens::bSnRD)) pDC->Rectangle(&(CRect(pnt,sizeOne)));
                pnt.x += wightOne;
                if (m_pDlg->m_Hist[l].m_Sost.tstBit(CSostSens::bSnABL)) pDC->Rectangle(&(CRect(pnt,sizeOne)));
                pnt.x += wightOne;
                if (m_pDlg->m_Hist[l].m_Sost.tstBit(CSostSens::bSnMP)) pDC->Rectangle(&(CRect(pnt,sizeOne)));
                pnt.x += wightOne;
                if (!m_pDlg->m_Hist[l].m_Sost.tstBit(CSostSens::bSnCall)) pDC->Rectangle(&(CRect(pnt,sizeOne)));
                pnt.x += wightOne;
                if (m_pDlg->m_Hist[l].m_Sost.tstBit(CSostSens::bSnUKSL) ||
                    m_pDlg->m_Hist[l].m_Sost.tstBit(CSostSens::bSnDevUKSL)) pDC->Rectangle(&(CRect(pnt,sizeOne)));
                pnt.x += wightOne;
                if (m_pDlg->m_Hist[l].m_Sost.tstBit(CSostSens::bSbOnLift)) pDC->Rectangle(&(CRect(pnt,sizeOne)));
                pnt.x += wightOne;
                if (m_pDlg->m_Hist[l].m_Sost.tstBit(CSostSens::bSbAkMP)) pDC->Rectangle(&(CRect(pnt,sizeOne)));
                pnt.x += wightOne;
                if (m_pDlg->m_Hist[l].m_Sost.tstBit(CSostSens::bUSER1)) pDC->Rectangle(&(CRect(pnt,sizeOne)));
                pnt.x += wightOne;
                if (m_pDlg->m_Hist[l].m_Sost.tstBit(CSostSens::bUSER2)) pDC->Rectangle(&(CRect(pnt,sizeOne)));
                pnt.x += wightOne;
                if (m_pDlg->m_Hist[l].m_Sost.tstBit(CSostSens::bUSER3)) pDC->Rectangle(&(CRect(pnt,sizeOne)));
                pnt.x += wightOne;
                if (m_pDlg->m_Hist[l].m_Sost.tstBit(CSostSens::bUSER4)) pDC->Rectangle(&(CRect(pnt,sizeOne)));
                pnt.x += wightOne;
                if (m_pDlg->m_Hist[l].m_Sost.tstBit(CSostSens::bOUT1)) pDC->Rectangle(&(CRect(pnt,sizeOne)));
                pnt.x += wightOne;
                if (m_pDlg->m_Hist[l].m_Sost.tstBit(CSostSens::b220V)) pDC->Rectangle(&(CRect(pnt,sizeOne)));
                pnt.x += wightOne;
            }
        }
        else
        {
            buf.Format(_T("Нет связи с лифтовым блоком %d"), m_pDlg->m_Hist[l].m_NumLift);
            pDC->SetTextAlign(TA_LEFT | TA_TOP);
            pDC->TextOut(pnt.x+15, pnt.y - 1, buf);
        }
    }

    pDC->SelectObject(&f2);
    pDC->SetTextAlign(TA_RIGHT | TA_TOP);
    if (m_pDlg->m_bModeUB)
    {
        for (int i = 0; i < 24; i++)
        {
            buf.Format(_T("УБ %d"), i+1);
            pDC->TextOut(r.left+wightOne * i, r.bottom, buf);
        }
    }
    else
    {
        pDC->TextOut(r.left + wightOne * 0, r.bottom, _T("101"));
        pDC->TextOut(r.left + wightOne * 1, r.bottom, _T("СТОП1"));
        pDC->TextOut(r.left + wightOne * 2, r.bottom, _T("СТОП2"));
        pDC->TextOut(r.left + wightOne * 3, r.bottom, _T("ДК1"));
        pDC->TextOut(r.left + wightOne * 4, r.bottom, _T("ДК2"));
        pDC->TextOut(r.left + wightOne * 5, r.bottom, _T("РКД"));
        pDC->TextOut(r.left + wightOne * 6, r.bottom, _T("РОД"));
        pDC->TextOut(r.left + wightOne * 7, r.bottom, _T("РЗД"));
        pDC->TextOut(r.left + wightOne * 8, r.bottom, _T("РД"));
        pDC->TextOut(r.left + wightOne * 9, r.bottom, _T("АБЛ"));
        pDC->TextOut(r.left + wightOne * 10, r.bottom, _T("МП"));
        pDC->TextOut(r.left + wightOne * 11, r.bottom, _T("ВЫЗОВ"));
        pDC->TextOut(r.left + wightOne * 12, r.bottom, _T("УКСЛ"));
        pDC->TextOut(r.left + wightOne * 13, r.bottom, _T("ВКЛ"));
        pDC->TextOut(r.left + wightOne * 14, r.bottom, _T("ИЗ МП"));
        pDC->TextOut(r.left + wightOne * 15, r.bottom, _T("USER1"));
        pDC->TextOut(r.left + wightOne * 16, r.bottom, _T("USER2"));
        pDC->TextOut(r.left + wightOne * 17, r.bottom, _T("USER3"));
        pDC->TextOut(r.left + wightOne * 18, r.bottom, _T("USER4"));
        pDC->TextOut(r.left + wightOne * 19, r.bottom, _T("OUT1"));
        pDC->TextOut(r.left + wightOne * 20, r.bottom, _T("220B"));
    }
    pDC->SelectObject(pOldPen);
    pDC->SelectObject(pOldBrush);
    pDC->SelectObject(pOldFont);

    p1.DeleteObject();
    p2.DeleteObject();
    w.DeleteObject();
    b.DeleteObject();
    f1.DeleteObject();
    f2.DeleteObject();
}

