#pragma once

#include "../stdafx.h"
#include <mtest/mtestpro.h>
#include <mtest/mtestpro_dlg.h>
#include <mtest/settings_dlg.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CSettingsDlg* dlgSettings;

IMPLEMENT_DYNCREATE(CCursButton, CButton)

BEGIN_MESSAGE_MAP(CCursButton, CButton)
    ON_WM_SETCURSOR()
END_MESSAGE_MAP()

afx_msg BOOL CCursButton::OnSetCursor( CWnd* pWnd, UINT nHitTest, UINT message )
{
    if(IsWindowEnabled())
    {
        ::SetCursor(AfxGetApp()->LoadCursor(IDC_SEL_CURS));
        return TRUE;
    }
    return(CButton::OnSetCursor( pWnd, nHitTest, message ));
}


int CHistSost::operator == (CHistSost& sl)
{
    if (!m_bValid && !sl.m_bValid)
    {
        return TRUE;
    }
    if (m_bValid ^ sl.m_bValid)
    {
        return FALSE;
    }

    return (
           m_Sost.tstBit(CSostSens::bSnBP) == sl.m_Sost.tstBit(CSostSens::bSnBP)
        && m_Sost.tstBit(CSostSens::bSnSTOP1) == sl.m_Sost.tstBit(CSostSens::bSnSTOP1)
        && m_Sost.tstBit(CSostSens::bSnSTOP2) == sl.m_Sost.tstBit(CSostSens::bSnSTOP2)
        && m_Sost.tstBit(CSostSens::bSnDK1) == sl.m_Sost.tstBit(CSostSens::bSnDK1)
        && m_Sost.tstBit(CSostSens::bSnDK2) == sl.m_Sost.tstBit(CSostSens::bSnDK2)
        && m_Sost.tstBit(CSostSens::bSnRKD) == sl.m_Sost.tstBit(CSostSens::bSnRKD)
        && m_Sost.tstBit(CSostSens::bSnROD) == sl.m_Sost.tstBit(CSostSens::bSnROD)
        && m_Sost.tstBit(CSostSens::bSnRZD) == sl.m_Sost.tstBit(CSostSens::bSnRZD)
        && m_Sost.tstBit(CSostSens::bSnRD) == sl.m_Sost.tstBit(CSostSens::bSnRD)
        && m_Sost.tstBit(CSostSens::bSnABL) == sl.m_Sost.tstBit(CSostSens::bSnABL)
        && m_Sost.tstBit(CSostSens::bSnMP) == sl.m_Sost.tstBit(CSostSens::bSnMP)
        && m_Sost.tstBit(CSostSens::bSnCall) == sl.m_Sost.tstBit(CSostSens::bSnCall)
        && m_Sost.tstBit(CSostSens::bSnUKSL) == sl.m_Sost.tstBit(CSostSens::bSnUKSL)
        && m_Sost.tstBit(CSostSens::bSnDevUKSL) == sl.m_Sost.tstBit(CSostSens::bSnDevUKSL)
        && m_Sost.tstBit(CSostSens::bSbOnLift) == sl.m_Sost.tstBit(CSostSens::bSbOnLift)
        && m_Sost.tstBit(CSostSens::bSbAkMP) == sl.m_Sost.tstBit(CSostSens::bSbAkMP)
        && m_Sost.tstBit(CSostSens::bUSER1) == sl.m_Sost.tstBit(CSostSens::bUSER1)
        && m_Sost.tstBit(CSostSens::bUSER2) == sl.m_Sost.tstBit(CSostSens::bUSER2)
        && m_Sost.tstBit(CSostSens::bUSER3) == sl.m_Sost.tstBit(CSostSens::bUSER3)
        && m_Sost.tstBit(CSostSens::bUSER4) == sl.m_Sost.tstBit(CSostSens::bUSER4)
        && m_Sost.tstBit(CSostSens::bOUT1) == sl.m_Sost.tstBit(CSostSens::bOUT1)
        && m_Sost.tstBit(CSostSens::b220V) == sl.m_Sost.tstBit(CSostSens::b220V)
        && m_SostUB == sl.m_SostUB
    );
}

int CHistSost::operator != (CHistSost& sl)
{
    return (!(*this == sl));
}

/* массив историй изменения состояний */
bool CHistSostArray::Put(CHistSost& aSost)
{
    if(GetSize() > 0)
    {
        if(GetData()[GetSize()-1] != aSost)
        {
            Add(aSost);
            while(GetSize() > maxLenHist+1)
            {
                RemoveAt(0);
            }
            return true;
        }
        else
        {
            GetData()[GetSize()-1].m_Count++;
            return false;
        }
    }
    else
    {
        Add(aSost);
        return true;
    }
}

/* неизменяемый признак */
IMPLEMENT_DYNCREATE(CFlagOut, CButton)

BEGIN_MESSAGE_MAP(CFlagOut, CButton)
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

/* CTestSulDisp */
void CMTestProDisp::Put(CPult* aPult, CPackMes& aPack)
{
    if (m_pDlg->m_bOpen)
    {
        CMes Mes;

        aPack.BegGet();

        while (aPack.GetMes(Mes))
        {
            theApp.m_Send.Put(CPackV7(Mes));
        }
    }
}

/* массив допустимых значений прошивок */
const TCHAR* CListValidVers::s_File = _T("MTestPro.Cfg");

bool CListValidVers::Load()
{
    clear();

    char buf[200];
    ifstream inp(theApp.m_sPathStart+s_File);

    memset(buf, 0, sizeof(buf));

    if (inp.good())
    {
        while (inp.getline(buf,199).good())
        {
            int TypDev  = 0;
            int TypExt  = 0;
            char Vers[200];

            memset(Vers, 0, sizeof(Vers));
            sscanf_s(buf, "%d;%d;%[^;]", &TypDev, &TypExt, Vers, sizeof(Vers));

            push_back(CValidVers(static_cast<CDev::typDev>(TypDev), static_cast<CDev::typExt>(TypExt), Vers));
        }
        return true;
    }
    return false;
}

CString CListValidVers::Find(CDev::typDev aTypDev, CDev::typExt aTypExt)
{
    for (iterator i = begin(); i != end(); ++i)
    {
        if (i->m_TypDev == aTypDev && i->m_TypExt == aTypExt)
        {
            return i->m_Vers;
        }
    }
    return _T("");
}

CMTestProDlg::CMTestProDlg(CWnd* pParent /*=NULL*/)
    :CDialog(CMTestProDlg::IDD)
    ,m_bModeUB(false)
    ,m_hEventAsk(0)
    ,m_bOpen(false)
    ,m_hAskThread(0)
    ,m_dwThreadAskID(0)
    ,m_pSlot(0)
    ,m_pLift(0)
    ,m_iSend(0)
    ,m_iReceive(0)
    ,m_pToolTip(0)
    ,m_LastGoodMes(COleDateTime::GetCurrentTime())
    ,m_bNewSost(false)
    ,m_bKnovSost(false)
    ,m_Red(RGB(255,0,0))
    ,m_Yello(RGB(255,255,0))
    ,m_Green(RGB(0, 255, 0))
    ,m_White(RGB(255, 255, 255))
    ,m_Ident(0)
    ,m_MAC(_T(""))
    ,m_Key(_T(""))
    ,m_AutoParam(FALSE)
    ,m_SaveAutoParam(FALSE)
    ,m_IsExistLoader(false)
    ,m_IsActLoader(false)
    ,m_IsExistPage1(false)
    ,m_IsActPage1(false)
    ,m_CountWaitAct(0)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    m_hTypAnsw[0] = 0;
    m_hTypAnsw[1] = 0;
    m_hTypAnsw[2] = 0;
    macfile = NULL;
    m_ComLB = theApp.GetProfileString(_T("MTestParam"), _T("ComLB"), _T("COM1"));
    m_ComGGS = theApp.GetProfileString(_T("MTestParam"), _T("ComGGS"), _T("COM1"));
    m_IP = theApp.GetProfileInt(_T("MTestParam"), _T("IP"), 0);
    m_Port = theApp.GetProfileInt(_T("MTestParam"), _T("Port"), 46000);
    m_PortGGS = theApp.GetProfileInt(_T("MTestParam"), _T("PortGGS"), 46001);
    m_Ident = theApp.GetProfileInt(_T("MTestParam"), _T("Ident"), 2);
    m_Key = theApp.GetProfileString(_T("MTestParam"), _T("Key"), _T(""));
    m_KeyInLb = theApp.GetProfileString(_T("MTestParam"), _T("m_KeyInLb"),_T(""));
    m_AutoParam = theApp.GetProfileInt(_T("MTestParam"), _T("AutoParam"),FALSE);
    macfile = new MacChecker();
    dlgSettings = new CSettingsDlg(&macfile);
    m_IdentOWN.SetUnique();
}


void CMTestProDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);

    if(!pDX->m_bSaveAndValidate && m_bKnovSost)
    {
        m_SostLb0 = FALSE;
        m_SostLb1 = FALSE;
        m_SostLb2 = FALSE;
        m_SostLb3 = FALSE;
        m_SostLb4 = FALSE;
        m_SostLb5 = FALSE;
        m_SostLb6 = FALSE;
        m_SostLb7 = FALSE;
        m_SostLb8 = FALSE;
        m_SostLb9 = FALSE;
        m_SostLb10 = FALSE;
        m_SostLb11 = FALSE;
        m_SostLb12 = FALSE;
        m_SostUSER1 = FALSE;
        m_SostUSER2 = FALSE;
        m_SostUSER3 = FALSE;
        m_SostUSER4 = FALSE;
        m_SostOUT1 = FALSE;
        m_Sost220V = FALSE;
        m_SostAsk = FALSE;
        m_SostMP = FALSE;
        m_SostInnr = FALSE;
        m_SostSecur = FALSE;
        m_SostNoMCS = FALSE;
        m_Door1 = TRUE;
        m_Door2 = TRUE;

        if (m_pLift->m_SostSens.tstBit(CSostSens::bSnBP))  m_SostLb0  = TRUE;
        if (m_pLift->m_SostSens.tstBit(CSostSens::bSnSTOP1))  m_SostLb1  = TRUE;
        if (m_pLift->m_SostSens.tstBit(CSostSens::bSnSTOP2))  m_SostLb2  = TRUE;
        if (m_pLift->m_SostSens.tstBit(CSostSens::bSnDK1))  m_SostLb3  = TRUE;
        if (m_pLift->m_SostSens.tstBit(CSostSens::bSnDK2))  m_SostLb4  = TRUE;
        if (m_pLift->m_SostSens.tstBit(CSostSens::bSnRKD))  m_SostLb5  = TRUE;
        if (m_pLift->m_SostSens.tstBit(CSostSens::bSnROD))  m_SostLb6  = TRUE;
        if (m_pLift->m_SostSens.tstBit(CSostSens::bSnRZD))  m_SostLb7  = TRUE;
        if (m_pLift->m_SostSens.tstBit(CSostSens::bSnRD))  m_SostLb8  = TRUE;
        if (m_pLift->m_SostSens.tstBit(CSostSens::bSnABL))  m_SostLb9  = TRUE;
        if (m_pLift->m_SostSens.tstBit(CSostSens::bSnMP))  m_SostLb10 = TRUE;
        if (!m_pLift->m_SostSens.tstBit(CSostSens::bSnCall))  m_SostLb11 = TRUE;
        if (m_pLift->m_SostSens.tstBit(CSostSens::bSnUKSL) ||
            m_pLift->m_SostSens.tstBit(CSostSens::bSnDevUKSL)) m_SostLb12 = TRUE;
        if (m_pLift->m_SostSens.tstBit(CSostSens::bUSER1))  m_SostUSER1 = TRUE;
        if (m_pLift->m_SostSens.tstBit(CSostSens::bUSER2))  m_SostUSER2 = TRUE;
        if (m_pLift->m_SostSens.tstBit(CSostSens::bUSER3))  m_SostUSER3 = TRUE;
        if (m_pLift->m_SostSens.tstBit(CSostSens::bUSER4))  m_SostUSER4 = TRUE;
        if (m_pLift->m_SostSens.tstBit(CSostSens::bOUT1))  m_SostOUT1 = TRUE;
        if (m_pLift->m_SostSens.tstBit(CSostSens::b220V))  m_Sost220V = TRUE;

        if (m_pLift->m_Sost.tstBit(CSostDev::sbAsk))  m_SostAsk = TRUE;
        //      if(m_pLift->m_Sost.tstBit(CSostDev::sbInpMP         ))  m_SostMP = TRUE;
        if (m_pLift->m_Sost.tstBit(CSostDev::sbOpenMP)) m_SostMP = TRUE;
        if (m_pLift->m_Sost.tstBit(CSostDev::sbInpInnr)) m_SostInnr = TRUE;
        if (m_pLift->m_Sost.tstBit(CSostDev::sbDestrSec)) m_SostSecur = TRUE;
        if (m_pLift->m_Sost.tstBit(CSostDev::sbOpenInnr)) m_SostNoMCS = TRUE;

        if (m_pLift->m_SostDZ.tstBit(0)) m_Door1 = FALSE;
        if (m_pLift->m_SostDZ.tstBit(1)) m_Door2 = FALSE;
        //DDX_OCFloat(pDX, IDC_AKKUM, NULL, battery_value);

        DDX_Check(pDX, IDC_ASK, m_SostAsk);
        DDX_Check(pDX, IDC_MP, m_SostMP);
        DDX_Check(pDX, IDC_INNR, m_SostInnr);
        DDX_Check(pDX, IDC_SECUR, m_SostSecur);
        DDX_Check(pDX, IDC_NO_LINK_MCS, m_SostNoMCS);
        DDX_Check(pDX, IDC_DOOR_1, m_Door1);
        DDX_Check(pDX, IDC_DOOR_2, m_Door2);

        if (m_bNewSost)
        {
            m_Panel.Invalidate();
            m_bNewSost = false;
            m_bOnOffOUT1.SetWindowText(m_SostOUT1 ? _T("Отключить OUT1") : _T("Включить OUT1"));
            m_bOnOffOUT2.SetWindowText(m_SostUSER4 ? _T("Отключить OUT2") : _T("Включить OUT2"));
        }
        else
        {
            m_Panel.DrawCount();
        }
    }
    DDX_IPAddress(pDX, IDC_IP_ADDR, m_IP);
    DDX_Text(pDX, IDC_IP_PORT_DANN, m_Port);
    DDX_Text(pDX, IDC_IP_PORT_VOICE, m_PortGGS);
    DDX_Text(pDX, IDC_ID_LB, m_Ident);
    DDX_Text(pDX, IDC_MAC, m_MAC);
    DDX_Text(pDX, IDC_KEY, m_Key);
    DDX_Check(pDX, IDC_AUTO_PARAM, m_AutoParam);
    DDX_Text(pDX, IDC_MAC_COPY, mac_last_char);
    DDX_Text(pDX, IDC_AKKUM, battery_value_str);
}


BEGIN_MESSAGE_MAP(CMTestProDlg, CDialog)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_WM_DESTROY()
    ON_WM_TIMER()
    ON_MESSAGE(ID_EXISTS_PACK, OnExistsPack)
    ON_MESSAGE(ID_GET_IDENT, OnGetIdent)
    ON_MESSAGE(ID_CLR_PORT_LB, OnClrPortLb)
    ON_MESSAGE(ID_CLR_PORT_GGS, OnClrPortGGS)
    ON_MESSAGE(NO_LINK, OnNoLink)
    ON_MESSAGE(ERR_SOCKET, OnErrSocket)
    ON_BN_CLICKED(IDC_ON_GGS_MP, OnBnClickedOnGgsMp)
    ON_BN_CLICKED(IDC_ON_GGS_LIFT, OnBnClickedOnGgsLift)
    ON_BN_CLICKED(IDC_OFF_GGS, OnBnClickedOffGgs)
    ON_BN_CLICKED(IDC_ON, OnBnClickedOn)
    ON_BN_CLICKED(IDC_OFF, OnBnClickedOff)
    ON_BN_CLICKED(IDC_RESET, OnBnClickedReset)
    ON_BN_CLICKED(IDC_GET_SK, OnBnClickedGetSk)
    ON_BN_CLICKED(IDC_OF_OFF_OUT1, OnBnClickedOnOffOut1)
    ON_BN_CLICKED(IDC_ON_OFF_OUT2, OnBnClickedOnOffOut2)
    ON_BN_CLICKED(IDC_EMULAT, OnBnClickedEmulat)
    ON_BN_CLICKED(IDC_GET_VERS, OnBnClickedGetVers)
    ON_WM_CTLCOLOR()
    ON_BN_CLICKED(IDC_CONNECT, &CMTestProDlg::OnBnClickedConnect)
    ON_BN_CLICKED(IDC_AUTO_PARAM, &CMTestProDlg::OnBnClickedAutoParam)
    ON_BN_CLICKED(IDC_SWITCH_PAGE, &CMTestProDlg::OnBnClickedSwitchPage)
    ON_CBN_DROPDOWN(IDC_COM_PORT, &CMTestProDlg::OnCbnDropdownComPort)
    ON_CBN_DROPDOWN(IDC_COM_USB, &CMTestProDlg::OnCbnDropdownComUsb)
    ON_WM_KEYDOWN()
    ON_WM_SYSKEYDOWN()
    ON_WM_WINDOWPOSCHANGED()
//  ON_WM_CREATE()
//  ON_WM_SHOWWINDOW()
ON_WM_SIZE()
END_MESSAGE_MAP()


// CMTestProDlg message handlers
void CMTestProDlg::Disconnect()
{
    m_AutoParam = TRUE;
    m_cAutoParam.SetCheck(BST_CHECKED);
    EnableParam();
    Stop();
}

BOOL CMTestProDlg::OnInitDialog()
{
    m_Disp.Set(this);
    InitStruct();
    CDialog::OnInitDialog();

    no_com_port_flag = false;
    connect_btn_flag = true;
    no_no_flag = false;
    //((CEdit*)GetDlgItem(IDC_INDICATOR_BOX))->GetWindowRect(&Rect);
    Rect.bottom -= 25;
    Rect.left -= 1;
    //Rect.right += 10;
    Rect.top -= 50;
    // Add "About..." menu item to system menu.

    // IDM_ABOUTBOX must be in the system command range.
    ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
    ASSERT(IDM_ABOUTBOX < 0xF000);

    CMenu* pSysMenu = GetSystemMenu(FALSE);
    if (pSysMenu != NULL)
    {
        CString strAboutMenu;
        strAboutMenu.LoadString(IDS_ABOUTBOX);
        if (!strAboutMenu.IsEmpty())
        {
            pSysMenu->AppendMenu(MF_SEPARATOR);
            pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
        }
    }

    // Set the icon for this dialog.  The framework does this automatically
    //  when the application's main window is not a dialog
    SetIcon(m_hIcon, TRUE);         // Set big icon
    SetIcon(m_hIcon, FALSE);        // Set small icon

    m_iSend     = theApp.LoadIcon(IDI_SEND);
    m_iReceive  = theApp.LoadIcon(IDI_RECEIVE);

    m_Panel.SubclassDlgItem(IDC_PANEL, this);
    m_bOnGgsLift.SubclassDlgItem(IDC_ON_GGS_LIFT, this);
    m_bOnGgsMp.SubclassDlgItem(IDC_ON_GGS_MP, this);
    m_bOffGgs.SubclassDlgItem(IDC_OFF_GGS, this);
    m_bOn.SubclassDlgItem(IDC_ON, this);
    m_bOff.SubclassDlgItem(IDC_OFF, this);
    m_bReset.SubclassDlgItem(IDC_RESET, this);
    m_bGetSK.SubclassDlgItem(IDC_GET_SK, this);
    m_eNumSK.SubclassDlgItem(IDC_NUM_SK, this);
    m_iIndicator.SubclassDlgItem(IDC_INDICATOR, this);
    m_bSostAsk.SubclassDlgItem(IDC_ASK, this);
    m_bSostMP.SubclassDlgItem(IDC_MP, this);
    m_bSostInnr.SubclassDlgItem(IDC_INNR, this);
    m_bSostSecur.SubclassDlgItem(IDC_SECUR, this);
    m_bSostNoMCS.SubclassDlgItem(IDC_NO_LINK_MCS, this);
    m_bOnOffOUT1.SubclassDlgItem(IDC_OF_OFF_OUT1, this);
    m_bOnOffOUT2.SubclassDlgItem(IDC_ON_OFF_OUT2, this);
    m_bEmulat.SubclassDlgItem(IDC_EMULAT, this);
    m_bDoor1.SubclassDlgItem(IDC_DOOR_1, this);
    m_bDoor2.SubclassDlgItem(IDC_DOOR_2, this);
    m_bGetVers.SubclassDlgItem(IDC_GET_VERS, this);
    m_eVers.SubclassDlgItem(IDC_VERS, this);
    m_cComLB.SubclassDlgItem(IDC_COM_PORT, this);
    m_cComGGS.SubclassDlgItem(IDC_COM_USB, this);
    m_cIP.SubclassDlgItem(IDC_IP_ADDR, this);
    m_cPort.SubclassDlgItem(IDC_IP_PORT_DANN, this);
    m_cPortGGS.SubclassDlgItem(IDC_IP_PORT_VOICE, this);
    m_cIdent.SubclassDlgItem(IDC_ID_LB, this);
    m_cMAC.SubclassDlgItem(IDC_MAC, this);
    m_cKey.SubclassDlgItem(IDC_KEY, this);
    m_cAutoParam.SubclassDlgItem(IDC_AUTO_PARAM, this);
    m_bConnect.SubclassDlgItem(IDC_CONNECT, this);
    m_cAkkum.SubclassDlgItem(IDC_AKKUM, this);
    m_eCodRestarts.SubclassDlgItem(IDC_COD_RESTARTS, this);
    m_bSwitchPage.SubclassDlgItem(IDC_SWITCH_PAGE, this);

    m_bSwitchPage.EnableWindow(FALSE);
    m_bSwitchPage.SetWindowText("Переключение страниц микропрограмм");

    m_cComLB.SetWindowText(m_ComLB);
    m_cComGGS.SetWindowText(m_ComGGS);

    m_Panel.Init(this);

    if(!m_pToolTip)
    {
        m_pToolTip = new CToolTipCtrl;
        m_pToolTip->Create(this);

        theApp.m_gpToolTip = m_pToolTip;

        m_pToolTip->AddTool(&m_bSostLb0, _T("ВП - напряжение в цепи управления"));
        m_pToolTip->AddTool(&m_bSostLb1, _T("СТОП1 - до кнопки СТОП"));
        m_pToolTip->AddTool(&m_bSostLb2, _T("СТОП2 - после кнопки СТОП"));
        m_pToolTip->AddTool(&m_bSostLb3, _T("ДК1 - до контакта дверей кабины"));
        m_pToolTip->AddTool(&m_bSostLb4, _T("ДК2 - после контакта дверей кабины"));
        m_pToolTip->AddTool(&m_bSostLb5, _T("РКД - реле контроля дверей (конец цепи безопасности)"));
        m_pToolTip->AddTool(&m_bSostLb6, _T("РОД - реле привода дверей на открытие"));
        m_pToolTip->AddTool(&m_bSostLb7, _T("РЗД - реле привода дверей на закрытие"));
        m_pToolTip->AddTool(&m_bSostLb8, _T("РД - реле движения"));
        m_pToolTip->AddTool(&m_bSostLb9, _T("АБЛ - автоматическая блокировка УПЛ/УЛЖ (бит пользователя)"));
        m_pToolTip->AddTool(&m_bSostLb10, _T("МП - датчик проникновения в машинное помещение"));
        m_pToolTip->AddTool(&m_bSostLb11, _T("ВЫЗОВ - кнопка ВЫЗОВ в лифте и МП"));
        m_pToolTip->AddTool(&m_bSostLb12, _T("УКСЛ - наличие импульсов от УКСЛ"));
        m_pToolTip->AddTool(&m_bSostUSER1, _T("USER1 - вход пользователя 1"));
        m_pToolTip->AddTool(&m_bSostUSER2, _T("USER2 - вход пользователя 2"));
        m_pToolTip->AddTool(&m_bSostUSER3, _T("USER3 - вход пользователя 3"));
        m_pToolTip->AddTool(&m_bSostUSER4, _T("USER4 - вход пользователя 4"));
        m_pToolTip->AddTool(&m_bSostOUT1, _T("OUT1 - состояние токового выхода 1"));
        m_pToolTip->AddTool(&m_bSost220V, _T("220В - наличие сетевого питания"));
        m_pToolTip->AddTool(&m_Panel, _T("История состояний"));
        m_pToolTip->AddTool(&m_bSostAsk, _T("Вызов диспетчера"));
        m_pToolTip->AddTool(&m_bSostMP, _T("Открыто машинное помещение"));
        m_pToolTip->AddTool(&m_bSostInnr, _T("Проникновение в шахту"));
        m_pToolTip->AddTool(&m_bSostSecur, _T("Разрыв цепи безопасности"));
        m_pToolTip->AddTool(&m_bSostNoMCS, _T("Отсутствует связь с MCS"));
        m_pToolTip->AddTool(&m_bOnGgsLift, _T("Включение разговорной связи с кабиной лифта"));
        m_pToolTip->AddTool(&m_bOnGgsMp, _T("Включение разговорной связи с машинным помещением"));
        m_pToolTip->AddTool(&m_bOffGgs, _T("Выключение разговорной связи"));
        m_pToolTip->AddTool(&m_bOn, _T("Включение лифта"));
        m_pToolTip->AddTool(&m_bOff, _T("Выключение лифта"));
        m_pToolTip->AddTool(&m_bReset, _T("Очистка состояний лифтового блока"));
        m_pToolTip->AddTool(&m_bGetSK, _T("Чтение номера вставленного сервисного ключа"));
        m_pToolTip->AddTool(&m_eNumSK, _T("Номер вставленного сервисного ключа"));
        m_pToolTip->AddTool(&m_iIndicator, _T("Индикация приема данных от лифтового блока"));
        m_pToolTip->AddTool(&m_bOnOffOUT1, _T("Включение/выключение выхода OUT1"));
        m_pToolTip->AddTool(&m_bOnOffOUT2, _T("Включение/выключение выхода OUT2(USER4)"));
        m_pToolTip->AddTool(&m_bEmulat, _T("Запуск эмулятора станции управления лифта"));
        m_pToolTip->AddTool(&m_bDoor1, _T("Состояние одной двери шахты"));
        m_pToolTip->AddTool(&m_bDoor2, _T("Состояние двух дверей шахты"));
        m_pToolTip->AddTool(&m_bGetVers, _T("Чтение версии микропрограммы"));
        m_pToolTip->AddTool(&m_eVers, _T("Версия микропрограммы"));
        m_pToolTip->AddTool(&m_cComLB, _T("COM порт конфигуратора блока"));
        m_pToolTip->AddTool(&m_cComGGS, _T("COM порт голосового конвертора"));
        m_pToolTip->AddTool(&m_cIP, _T("IP адрес блока"));
        m_pToolTip->AddTool(&m_cPort, _T("IP порт данных блока"));
        m_pToolTip->AddTool(&m_cPortGGS, _T("IP порт голосовых данных блока"));
        m_pToolTip->AddTool(&m_cIdent, _T("Идентификатор блока"));
        m_pToolTip->AddTool(&m_cMAC, _T("MAC адрес блока"));
        m_pToolTip->AddTool(&m_cKey, _T("Ключ системы"));
        m_pToolTip->AddTool(&m_cAutoParam, _T("Признак автоматического вычитывания параметров блока"));
        m_pToolTip->AddTool(&m_bConnect, _T("Подключиться/отключится по IP"));

        m_pToolTip->Activate(TRUE);
    }

    theApp.m_hwndDialog = m_hWnd;

    m_bOnOffOUT1.EnableWindow(FALSE);
    m_bOnOffOUT2.EnableWindow(FALSE);
    m_bEmulat.EnableWindow(FALSE);
    m_bGetVers.EnableWindow(FALSE);

    m_ValidVers.Load();

    EnableParam();
    EnableButton();

    /*if(m_AutoParam)
    {
        try
        {
            theApp.m_PortLB.Open(m_ComLB);
            theApp.m_PortLB.SendAsk();
        }
        catch(ErrorXML aErr)
        {
            Disconnect();
            theApp.m_Log.PutErr(aErr.m_Message, aErr.m_Hr, true);
        }
    }*/
    SetTimer(tmGetIdent, 500, 0);

    CFont font;
    font.CreateFont(
         16 // nHeight
        ,0 // nWidth
        ,0 // nEscapement
        ,0 // nOrientation
        ,FW_NORMAL // nWeight
        ,FALSE // bItalic
        ,TRUE // bUnderline
        ,0 // cStrikeOut
        ,ANSI_CHARSET // nCharSet
        ,OUT_DEFAULT_PRECIS // nOutPrecision
        ,CLIP_DEFAULT_PRECIS // nClipPrecision
        ,DEFAULT_QUALITY // nQuality
        ,DEFAULT_PITCH | FF_SWISS // nPitchAndFamily
        ,"Consolas" // lpszFacename
    );
    ((CEdit*)GetDlgItem(IDC_MAC_COPY))->SetFont(&font);
    ((CEdit*)GetDlgItem(IDC_MAC))->SetFont(&font);
    ((CEdit*)GetDlgItem(IDC_ID_LB))->SetFont(&font);
    ((CEdit*)GetDlgItem(IDC_AKKUM))->SetFont(&font);

    if (!dlgSettings->FileExist()){
        dlgSettings->DoModal();
    }
    else{
        SetWindowPos(
             NULL
            ,theApp.GetProfileInt(_T("MTestParam"), _T("Left"), CW_USEDEFAULT)
            ,theApp.GetProfileInt(_T("MTestParam"), _T("Top"), CW_USEDEFAULT)
            ,theApp.GetProfileInt(_T("MTestParam"), _T("Width"), CW_USEDEFAULT)
            ,theApp.GetProfileInt(_T("MTestParam"), _T("Height"), CW_USEDEFAULT)
            ,SWP_NOZORDER
        );
    }

    SetTimer(batLevel, 1000, 0);
    return TRUE; // return TRUE unless you set the focus to a control
}

void CMTestProDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
/*  if ((nID & 0xFFF0) == IDM_ABOUTBOX)
    {
        CAboutDlg dlgAbout;
        dlgAbout.DoModal();
    }
    else
    {
        CDialog::OnSysCommand(nID, lParam);
    }*/
}


afx_msg void CMTestProDlg::OnOK()
{
    return;
}

afx_msg void CMTestProDlg::OnCancel()
{
    return;
}


void CMTestProDlg::OnDestroy()
{
    CDialog::OnDestroy();

    theApp.m_gpToolTip = NULL;
    theApp.m_hwndDialog = NULL;

    theApp.WriteProfileString(_T("MTestParam"), _T("ComLB"), m_ComLB);
    theApp.WriteProfileString(_T("MTestParam"), _T("ComGGS"), m_ComGGS);
    theApp.WriteProfileInt(_T("MTestParam"), _T("IP"), m_IP);
    theApp.WriteProfileInt(_T("MTestParam"), _T("Port"), m_Port);
    theApp.WriteProfileInt(_T("MTestParam"), _T("PortGGS"), m_PortGGS);
    theApp.WriteProfileInt(_T("MTestParam"), _T("Ident"), m_Ident);
    theApp.WriteProfileString(_T("MTestParam"), _T("Key"), m_Key);
    theApp.WriteProfileString(_T("MTestParam"), _T("m_KeyInLb"), m_KeyInLb);
    theApp.WriteProfileInt(_T("MTestParam"), _T("AutoParam"), m_AutoParam);

    Stop();
}

/*
 * If you add a minimize button to your dialog, you will need the code below
 * to draw the icon.  For MFC applications using the document/view model,
 * this is automatically done for you by the framework
*/
void CMTestProDlg::OnPaint()
{
    if (IsIconic())
    {
        CPaintDC dc(this); // device context for painting

        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

        // Center icon in client rectangle
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        // Draw the icon
        dc.DrawIcon(x, y, m_hIcon);
    }
    else
    {
        CDialog::OnPaint();
    }
}

/*
 * The system calls this function to obtain the cursor to display while
 * the user drags the minimized window.
*/
HCURSOR CMTestProDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

void CMTestProDlg::InitStruct()
{
    m_Disp.m_pPult = new CPult(0);
    m_Disp.m_pPult->m_pDisp = &m_Disp;
    m_Disp.m_pPult->m_bCross = false;

    m_Disp.m_TabPult.Insert(m_Disp.m_pPult);

    CUM* ItUMD = new CUM(0, NULL);

    ItUMD->m_pPult = m_Disp.m_pPult;
    m_Disp.m_pPult->m_TabUM.push_back(ItUMD);

    CSlot *ItSlotD = new CSlot(8, ItUMD, CSlot::tsMon, NULL);
    ItUMD->m_TabSlot.push_back(ItSlotD);

    m_pSlot = new CSlot(7, ItUMD, CSlot::tsCLB, NULL);

    m_pLift = new CDev(numDev, m_pSlot, CDev::tdNone, NULL);
    m_pSlot->m_TabDev.push_back(m_pLift);

    ItUMD->m_TabSlot.push_back(m_pSlot);

    m_Disp.m_pPult->m_TabUM.m_pPult = m_Disp.m_pPult;
    m_Disp.m_pPult->m_TabUM.SetLink();
}



bool CMTestProDlg::Start()
{
    m_pLift->m_TypDev = CDev::tdNone;
    m_cComLB.GetWindowText(m_ComLB);
    m_cComGGS.GetWindowText(m_ComGGS);

    CheckAnswer status;
    CString str;
    duplicate = false;
    Invalidate();
    /*if (m_MAC.GetLength() != 0 && !no_no_flag){
        if (!macfile) macfile = new MacChecker();
        macfile->open();
        status = macfile->check(m_MAC);
        if (status.result == 1){
            duplicate = true;
        }
        if (status.result == 2){
            if (status.count == 2 || status.count == 3 || status.count == 4) str.Format(_T(" %i раза пользователем: %s"), status.count, status.user);
            else str.Format(_T(" %i раз пользователем: %s"), status.count, status.user);
            MessageBox(_T("Устройство с данным mac-адресом уже использовалось " + status.last_date + str), _T("Ошибка"), MB_ICONERROR | MB_OK);
            //Stop();
        }
        Invalidate();
        macfile->writem(m_MAC);
        macfile->close();
    }*/
    try
    {
        m_bOpen = true;

        theApp.m_Receiv.m_Ident = m_IdentOWN;
        theApp.m_Receiv.m_Key = m_KeySys;

        theApp.m_Send.m_AddrRemote = htonl(m_IP);
        theApp.m_Send.m_PortRemote = m_Port;
        theApp.m_Send.m_Ident = m_IdentLB;

        theApp.m_Receiv.Start();
        theApp.m_Send.Start();

        m_hEventAsk = ::CreateEvent(NULL,TRUE, FALSE, NULL);

        m_hAskThread = ::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) AskLift, this, 0, &m_dwThreadAskID);

        if (!m_hAskThread)
        {
            throw ErrorXML(_T("Ошибка при запуске AskLift"), ::GetLastError());
        }

        m_hTypAnsw[0] = CreateEvent(NULL,TRUE, FALSE, NULL);
        m_hTypAnsw[1] = CreateEvent(NULL,TRUE, FALSE, NULL);
        m_hTypAnsw[2] = CreateEvent(NULL,TRUE, FALSE, NULL);

        EnableButton();
        m_bConnect.SetWindowText(_T("Отключиться"));
    }
    catch (ErrorXML aErr)
    {
        m_bOpen = false;
        no_com_port_flag = true;
        no_no_flag = true;
        theApp.m_Log.PutErr(aErr.m_Message, aErr.m_Hr, true);
    }

    return m_bOpen;
}

void CMTestProDlg::Stop()
{
    if(m_bOpen)
    {
        m_bSwitchPage.EnableWindow(FALSE);
        m_bSwitchPage.SetWindowText("Переключение страниц микропрограмм");

        m_IsExistLoader = false;
        m_IsActLoader = false;
        m_IsExistPage1 = false;
        m_IsActPage1 = false;
        m_bOpen = false;

        EnableButton();
        m_bConnect.SetWindowText(_T("Подключиться"));

        SetEvent(m_hEventAsk);

        while (m_hAskThread != NULL)
        {
            Sleep(1);
        }

        theApp.m_Send.Stop();
        theApp.m_Receiv.Stop();
    }
}

DWORD WINAPI CMTestProDlg::AskLift( CMTestProDlg* lpDlg )
{
    BYTE Dann[4] = {1,0,0,0};
    BYTE BodyTestAkkum[] = {0,0};

    while (lpDlg->m_bOpen)
    {
        //SCOPE_LOCK_MUTEX(lpDlg->single_conn.get());
        if(lpDlg->m_pLift->m_TypDev == CDev::tdNone)
        {
            BYTE BodyIAP[4] = {CMes::saIAPTotal, 0, 0, 0};

            // запрос состояния загрузчика
            lpDlg->m_pLift->AddMes(
                 prAskDriv
                ,CMes::tmGetSostD
                ,CMes::amIAP
                ,BodyIAP
                ,sizeof(BodyIAP)
            );
            BodyIAP[0] = CMes::saIAPPage;
            BodyIAP[1] = 1;

            // запос состояния первой страницы
            lpDlg->m_pLift->AddMes(
                 prAskDriv
                ,CMes::tmGetSostD
                ,CMes::amIAP
                ,BodyIAP
                ,sizeof(BodyIAP)
            );

            lpDlg->m_pLift->AddMes(
                 prAskDriv
                ,CMes::tmGetSostD
                ,CMes::amNone
            );
        }

        lpDlg->m_pLift->AddMes(prAskDriv, CMes::tmGetSostD, CMes::amOffGGS);
        lpDlg->m_pLift->AddMes(prAskDriv, CMes::tmGetSostD, CMes::amValLift, Dann, 4);
        lpDlg->m_pLift->AddMes(prAskDriv, CMes::tmGetSostD, CMes::amValUB);
        lpDlg->m_pLift->AddMes(prAskDriv, CMes::saTestBatt, BodyTestAkkum, sizeof(BodyTestAkkum));
        lpDlg->m_pLift->Put();

        if (theApp.lktest_dlg->m_op2.get_status())
        {
            unsigned int orig = 0x3;
            unsigned int dest = 0;
            unsigned int lbid = theApp.lktest_dlg->m_op1.m_Ident;
            ((BYTE*)&dest)[0] = theApp.lktest_dlg->m_op2.get_addr();
            ((BYTE*)&dest)[1] = ((BYTE*)&lbid)[0];
            ((BYTE*)&dest)[2] = ((BYTE*)&lbid)[1];
            ((BYTE*)&dest)[3] = ((BYTE*)&lbid)[2];
            OVERLAPPED m_OverlappedW2;
            LPDWORD NumberOfBytesWritten;
            unsigned int NumberOfBytesToWrite = 1024;
            BYTE buf[1024];
            memset(buf, 0, 1024);
            buf[0] = 0x0;
            buf[1] = 0xFE;
            buf[2] = 'D';
            buf[3] = 'I';
            buf[4] = 'R';
            buf[5] = 'E';
            buf[6] = 'C';
            buf[7] = 'T';
            CPackV7 Pack(theApp.m_Receiv.m_Ident, CIdentV7((BYTE *)(&dest)));
            Pack.SetAsk(CPackV7::tpItv7C, CPackV7::taStatusRoute, CPackV7::tdQuery, buf, 8);
            theApp.m_Send.SendSocket(CPackV7(Pack));
        }

        //CPackV7 Pack(theApp.m_Receiv.m_Ident, theApp.m_Send.m_Ident);

        /*BYTE  buf[4] = {1, 1, 0, 0};

        int     Addr    = 1024+192; // Адрес кодов рстартов

        memcpy(buf, &Addr, 2);
        buf[2]  = 16;
        buf[3]  = 0;    // чтение*/

        /*BYTE buf[10];
        buf[0] = 0x86;
        buf[1] = 0x0;
        buf[2] = 0xFE;
        buf[3] = 'D';
        buf[4] = 'I';
        buf[5] = 'R';
        buf[6] = 'E';
        buf[7] = 'C';
        buf[8] = 'T';

        Pack.SetAsk(CPackV7::tpItv7C, CPackV7::taNVRAM, CPackV7::tdQuery, buf, 9);
        theApp.m_Send.Put(Pack);*/

        //Pack.SetAsk(CPackV7::tpDirect, CPackV7::taNVRAM, CPackV7::tdQuery, buf, 4);
        //theApp.m_Send.Put(Pack);

        ResetEvent(lpDlg->m_hEventAsk);

        Sleep(200);

        DWORD rc = WaitForSingleObject(lpDlg->m_hEventAsk, 15*1000);
    }

    CloseHandle(lpDlg->m_hAskThread);
    lpDlg->m_hAskThread = NULL;
    lpDlg->m_dwThreadAskID = 0;

    CloseHandle(lpDlg->m_hEventAsk);
    CloseHandle(lpDlg->m_hTypAnsw[0]);
    CloseHandle(lpDlg->m_hTypAnsw[1]);
    CloseHandle(lpDlg->m_hTypAnsw[2]);

    return TRUE;
}

LRESULT CMTestProDlg::OnClrPortGGS(WPARAM, LPARAM)
{
    OnBnClickedOffGgs();
    return 0;
}


LRESULT CMTestProDlg::OnGetIdent(WPARAM w, LPARAM l)
{
    if (!UpdateData(TRUE))
    {
        goto exit;
    }
    if (
          !m_bOpen
        || theApp.m_PortLB.m_Ident != m_Ident
        || theApp.m_PortLB.m_Port != m_Port
        || theApp.m_PortLB.m_PortGGS != m_PortGGS
        || theApp.m_PortLB.m_KeyInLb != m_KeyInLb
        || theApp.m_PortLB.m_IP != m_IP
    ){
        Stop();

        m_MAC = theApp.m_PortLB.m_MAC;
        m_Ident = theApp.m_PortLB.m_Ident;
        m_Port = theApp.m_PortLB.m_Port;
        m_PortGGS = theApp.m_PortLB.m_PortGGS;
        m_IP = theApp.m_PortLB.m_IP;
        m_KeyInLb = theApp.m_PortLB.m_KeyInLb;

        mac_last_char =
              m_MAC.Mid(6, 2)
            + _T("-")
            + m_MAC.Mid(8, 2)
            + _T("-")
            + m_MAC.Right(2)
        ;
        UpdateData(FALSE);

        m_IdentLB.SetNode(m_Ident);
        m_KeySys.Set(m_KeyInLb);

        //Start();

        //if(theApp.m_PortLB.m_TypConnect == 1) 19.08.2016
        //{
        //  Start();
        //}
        //else
        //{
        //  Disconnect();
        //  theApp.m_PortLB.Close();

        //  AfxMessageBox(_T("В конфигурации ЛБ указан недопустиный (не static) тип соединения. Обмен по IP сети с ЛБ невозможен"), MB_OK|MB_ICONEXCLAMATION);
        //}
    }
    else
    {
        m_MAC = theApp.m_PortLB.m_MAC;
        UpdateData(FALSE);
    }

exit:
    return 0;
}


LRESULT CMTestProDlg::OnExistsPack(WPARAM w, LPARAM l)
{
    CString buf;
    CPackV7 Pack;

    while(theApp.m_pDlg->m_InpQueue.Read(Pack))
    {
        switch(Pack.m_ClassPack)
        {
        case CPackV7::cpData:
            {
                switch(Pack.m_TypPack)
                {
                case CPackV7::tpItv7C:
                    if (theApp.lktest_dlg->m_op2.get_status())
                    {
                        theApp.lktest_dlg->m_op2.get_com_port()->MngPack(Pack);
                    }
                break;
                case CPackV7::tpDirect:
                    {
                        if(Pack.m_Direct == CPackV7::tdAnswer)
                        {
                            switch(Pack.m_TypAsk)
                            {
                            case CPackV7::taNVRAM:
                                {
                                    if(Pack.Data()[4] == 0)
                                    {
                                        int Addr = (Pack.Data()[1]&0xFF)|((Pack.Data()[2]<<8)&0xFF00);
                                        int Len  = Pack.Data()[3];

                                        if(Addr == 1024+192 && Len == 16)
                                        {
                                            CString buf;

                                            for(int i = 0; i < 16; i++)
                                            {
                                                buf.AppendFormat(_T(" %02X"), Pack.Data()[5+i]);
                                            }
                                            m_eCodRestarts.SetWindowText(buf);
                                        }
                                    }
                                    break;
                                }
                            }
                        }
                        break;
                    }
                case CPackV7::tpMsgCLB:
                    {
                        CMes    Mes;

                        if(Pack.GetMes(Mes))
                        {
                            switch(Mes.m_iTypMes)
                            {
                            case CMes::tmSostUnit:
                            case CMes::tmSostUnitD:
                                {
                                    if(Mes.m_iTypMes == CMes::tmSostUnitD)
                                    {
                                        if(Mes.IsGood())
                                        {
                                            SetTimer(tmBlik, 100, 0);
                                            m_iIndicator.SetIcon(m_iSend);
                                            m_LastGoodMes = COleDateTime::GetCurrentTime();

                                            switch(Mes.m_iAsk)
                                            {
                                            case CMes::amIAP:
                                                {
                                                    if(Mes.IsGood() && (Mes[2+0]&0x20) == 0)
                                                    {
                                                        BYTE    bufIAP[40];

                                                        int Len = Mes.GetIAP(bufIAP, sizeof(bufIAP));
                                                        CMes::subAskIAP SubAsk  = static_cast<CMes::subAskIAP>(Mes[2+0]&0x1F);

                                                        switch(SubAsk)
                                                        {
                                                        case CMes::saIAPTotal:
                                                            {
                                                                if(Len >= 4)
                                                                {
                                                                    m_IsExistLoader = true;

                                                                    if(bufIAP[3] == 0) m_IsActLoader = true;
                                                                }
                                                                break;
                                                            }
                                                        case CMes::saIAPPage:
                                                            {
                                                                if(Len >= 16 && (Mes[2+1]&0xF) == 1)
                                                                {
                                                                    if(bufIAP[0] == 0xAA)
                                                                    {
                                                                        m_IsExistPage1 = true;
                                                                        m_IsActPage1    = bufIAP[1]==0x55;
                                                                    }
                                                                }
                                                                break;
                                                            }
                                                        }
                                                        if(m_IsExistLoader && m_IsExistPage1 && (m_IsActLoader || m_IsActPage1))
                                                        {
                                                            m_bSwitchPage.EnableWindow(TRUE);
                                                            m_bSwitchPage.SetWindowText(m_IsActLoader ? "Активировать микропрограмму в стр.1" : "Активировать загрузчик");
                                                        }
                                                        else
                                                        {
                                                            m_bSwitchPage.EnableWindow(FALSE);
                                                            m_bSwitchPage.SetWindowText("Переключение страниц микропрограмм");
                                                        }
                                                    }
                                                    break;
                                                }
                                            case CMes::amClrTrig:
                                            case CMes::amOffGGS:
                                            case CMes::amOnGGS:
                                            case CMes::amOnGGSD:
                                            case CMes::amSetOut:
                                            case CMes::amClrOut:
                                                {
                                                    Mes.SetTypMes(CMes::tmSostUnit);
                                                    Mes.m_Sost.SetData(Mes.Data()+2, Mes.Len()-8);
                                                    Mes.m_Sost.m_InfoCLB.SetMove();
                                                    break;
                                                }
                                            case CMes::amNone:
                                                {
                                                    m_pLift->SetParDev(Mes);

                                                    buf = _T("MTestPro - ");
                                                    buf += m_pLift->TypLB();

                                                    if(Mes.Len() >= 30 && Mes[4+2] == 0)
                                                    {
                                                        buf += _T(" ");
                                                        buf += CDev::DesignLB(Mes[6+2]);
                                                        buf.AppendFormat(_T(" v%d.%d.%d от %d.%d.%d кодек "), Mes[7+2], Mes[8+2], Mes[9+2], Mes[11+2], Mes[12+2], Mes[13+2]|(Mes[14+2] << 6));
                                                        buf += CDev::TypCodec(Mes[15+2]);
                                                        buf.AppendFormat(_T(" v%d.%d"), Mes[16+2], Mes[17+2]);
                                                    }

                                                    SetWindowText(buf);

                                                    m_bOnOffOUT1.EnableWindow(TRUE);
                                                    m_bOnOffOUT2.EnableWindow(TRUE);
                                                    m_bEmulat.EnableWindow(TRUE);
                                                    m_bSostUSER1.EnableWindow(TRUE);
                                                    m_bSostUSER2.EnableWindow(TRUE);
                                                    m_bSostUSER3.EnableWindow(TRUE);
                                                    m_bSostUSER4.EnableWindow(TRUE);
                                                    m_bSostOUT1.EnableWindow(TRUE);
                                                    m_bSost220V.EnableWindow(TRUE);

                                                    if(m_pLift->m_SuppIONVRAM)
                                                    {
                                                        m_bGetVers.EnableWindow(TRUE);
                                                        OnBnClickedGetVers();
                                                    }
                                                    break;
                                                }
                                            case CMes::amValLift:
                                                {
                                                    //if(!m_bModeUB)
                                                    {
                                                        m_pLift->m_SostSens = Mes;

                                                        //m_pLift->m_SostSens.clrBit(CSostSens::bUSER1);
                                                        //m_pLift->m_SostSens.clrBit(CSostSens::bUSER2);
                                                        //m_pLift->m_SostSens.clrBit(CSostSens::bUSER3);
                                                        //m_pLift->m_SostSens.clrBit(CSostSens::bUSER4);
                                                        //m_pLift->m_SostSens.clrBit(CSostSens::bOUT1 );
                                                        //m_pLift->m_SostSens.clrBit(CSostSens::b220V );
                                                        //m_pLift->m_SostSens.clrBit(CSostSens::bSnDevUKSL );
                                                        m_bNewSost = m_Hist.Put(CHistSost( m_pLift->m_SostSens, m_Hist.GetSize()>0? m_Hist[m_Hist.GetSize()-1].m_SostUB: CSostDZ()));   
                                                        m_bKnovSost = true;
                                                        UpdateData(FALSE);

                                                        //SetEvent(m_hTypAnsw[0]);  //  хороший ответ
                                                        //SetEvent(m_hEventAsk); 03.09.2007
                                                    }
                                                    break;
                                                }
                                            case CMes::amValUB:
                                                {
                                                    //if(m_bModeUB)
                                                    {
                                                        m_pLift->m_SostDZ = Mes;

                                                        m_bNewSost = m_Hist.Put(CHistSost( m_Hist.GetSize()>0? m_Hist[m_Hist.GetSize()-1].m_Sost: CSostSens(), m_pLift->m_SostDZ ));    
                                                        m_bKnovSost = true;
                                                        UpdateData(FALSE);

                                                        SetEvent(m_hTypAnsw[0]);    //  хороший ответ
                                                        SetEvent(m_hEventAsk);
                                                    }
                                                    break;
                                                }
                                            case CMes::amNumKey:
                                                {
                                                    m_pLift->m_NumKey   =   (   ((Mes[0+2] & 0x3F))      | ((Mes[1+2] & 0x3) << 6) ) | 
                                                                            ( ( ((Mes[1+2] & 0x3C) >> 2) | ((Mes[2+2] & 0xF) << 4) ) << 8 );

                                                    if(static_cast<short>(m_pLift->m_NumKey) != -1)
                                                    {
                                                        buf.Format(_T("%d"), m_pLift->m_NumKey);
                                                        m_eNumSK.SetWindowText(buf);
                                                    }
                                                    else
                                                    {
                                                        m_pLift->m_NumKey = 0;
                                                        m_eNumSK.SetWindowText(_T(""));
                                                    }
                                                    break;
                                                }
                                            case CMes::amGetNVRAM:
                                                {
                                                    int             Addr    = 0;
                                                    unsigned short  Val     = 0;
                                                    bool            IsWord;

                                                    if(Mes.GetLbNVRAM(Addr, Val, IsWord))
                                                    {
                                                        if( Addr==0 && IsWord)
                                                        {
                                                            int Low     = Val&0xFF;
                                                            int High    = (Val>>8)&0xFF;

                                                            m_Vers.Format(_T("%d.%d.%d"), Low/10, Low%10, High);
                                                            m_eVers.SetWindowText(m_Vers);
                                                        }
                                                    }
                                                    break;
                                                }
                                            case CMes::amServ18:
                                                {
                                                    if(Mes[2+0] == CMes::saTestBatt)
                                                    {
                                                        BYTE    Dann[34] = {0};

                                                        Mes.Conv6To8(44, 2, 33, Dann, sizeof(Dann));

                                                        CString buf;

                                                        buf.Format(_T("%.1f"), (Dann[1]&0xFF) * 0.0632);

                                                        m_cAkkum.SetWindowText(buf);
                                                    }
                                                    break;
                                                }
                                            }
                                        }
                                        else
                                        {
                                            switch(Mes.m_iAsk)
                                            {
                                            case CMes::amValUB:
                                            case CMes::amValLift:
                                                {
                                                    CString buf;
                                                    BYTE    UB[4]   = {0xFF,0xFF,0xFF,0xFF};

                                                    m_bNewSost = m_Hist.Put(CHistSost( CSostSens(), CSostDZ(), false, 1));  

                                                    m_pLift->m_Sost.Clear();
                                                    //m_pLift->m_SostDZ.Clear();
                                                    m_pLift->m_SostDZ       = UB;
                                                    m_pLift->m_SostSens.Clear();
                                                    m_pLift->m_SostSens.setBit(CSostSens::bSnCall);
                                                    m_pLift->m_TypDev       = CDev::tdNone;
                                                    m_pLift->m_TypExt       = CDev::teNone;
                                                    m_pLift->m_BitProt      = 0;
                                                    m_pLift->m_SuppIONVRAM  = false;

                                                    buf.Format(_T("MTestPro - %s")
                                                        , static_cast<const TCHAR*>(m_pLift->TypLB()));

                                                    SetWindowText(buf);

                                                    m_bKnovSost = true;
                                                    UpdateData(FALSE);

                                                    SetEvent(m_hTypAnsw[2]);    //  плохой ответ
                                                    SetEvent(m_hEventAsk);

                                                    m_eNumSK.SetWindowText(_T(""));
                                                    m_Vers  = "";
                                                    m_eVers.SetWindowText(m_Vers);
                                                    m_pLift->m_NumKey   = 0;

                                                    break;
                                                }
                                            }
                                            //m_eNumSK.SetWindowText(_T(""));
                                            //m_Vers    = "";
                                            //m_eVers.SetWindowText(m_Vers);
                                            //m_pLift->m_NumKey = 0;
                                        }
                                    }
                                    if(Mes.m_iTypMes == CMes::tmSostUnit)
                                    {
                                        CSostDev    oldSost = m_pLift->m_Sost;

                                        m_pLift->m_Sost = Mes.m_Sost;

                                        if(m_pLift->m_Sost.tstBit(CSostDev::sbKeyServ))
                                        {
                                            if(!oldSost.tstBit(CSostDev::sbKeyServ) || m_pLift->m_NumKey    == 0)
                                            {
                                                m_pLift->AddMes(prAskDriv, CMes::tmGetSostD, CMes::amNumKey);
                                                m_pLift->Put();
                                                m_pLift->m_NumKey   = 0;
                                            }
                                        }
                                        else
                                        {
                                            if(oldSost.tstBit(CSostDev::sbKeyServ) || m_pLift->m_NumKey != 0)
                                            {
                                                m_eNumSK.SetWindowText(_T(""));
                                                m_pLift->m_NumKey   = 0;
                                            }
                                        }
                                        UpdateData(FALSE);
                                    }
                                    break;
                                }
                            }
                        }
                    }
                    break;
                }
            }
            break;
        }
    }
    return 0;
}

void CMTestProDlg::OnBnClickedGetVers()
{
    if (m_pLift->m_SuppIONVRAM)
    {
        BYTE Mask[4];

        memset(Mask, 0, sizeof(Mask));

        m_pLift->AddMes(prAskDriv, CMes::tmGetSostD, CMes::amGetNVRAM, Mask, sizeof(Mask));
        m_pLift->Put();
    }
}

HBRUSH CMTestProDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

    // TODO:  Change any attributes of the DC here

    if (pWnd->GetDlgCtrlID() == IDC_MAC && duplicate)
    {
        pDC->SetBkMode(TRANSPARENT);
        pDC->SetTextColor(RGB(255, 255, 255));
        return m_Red;
    }

    if (pWnd->GetDlgCtrlID() == IDC_COM_PORT)
    {
        if (duplicate2)
        {
            pDC->SetBkMode(TRANSPARENT);
            pDC->SetTextColor(RGB(255, 255, 255));
            return m_Red;
        }
        else
        {
            pDC->SetBkMode(TRANSPARENT);
            pDC->SetTextColor(RGB(0, 0, 0));
            return m_White;
        }
    }

    if (pWnd->GetDlgCtrlID() == IDC_BATTERY_IND)
    {
        pDC->SetBkMode(TRANSPARENT);
        if (battery_level < 0)
        {
            return m_Red;
        }
        if (battery_level > 0)
        {
            return m_Green;
        }
        //if(m_bOpen) return m_Red;
        return m_White;
    }

    if (pWnd->GetDlgCtrlID() == IDC_VERS && m_pLift)
    {
        if (
               m_pLift->m_TypDev != CDev::tdNone
            && m_pLift->m_SuppIONVRAM
            && m_Vers.GetLength() > 0
        ){
            CString Vers = m_ValidVers.Find(m_pLift->m_TypDev, m_pLift->m_TypExt);

            if (Vers.GetLength() == 0)
            {
                pDC->SetBkMode(TRANSPARENT);
                return m_Yello;
            }
            else
            {
                if(Vers != m_Vers)
                {
                    pDC->SetBkMode(TRANSPARENT);
                    return m_Red;
                }
            }
        }
    }

    // TODO:  Return a different brush if the default is not desired
    return hbr;
}

BOOL CMTestProDlg::PreTranslateMessage(MSG* pMsg)
{
    if(pMsg->message == WM_KEYDOWN && m_bOpen)
    {
        if ((dlgSettings->getHotKey(0)).GetAt(0) == pMsg->wParam)
        {
            OnBnClickedReset();
            m_bReset.SetState(TRUE);
        }
        if ((dlgSettings->getHotKey(1)).GetAt(0) == pMsg->wParam)
        {
            OnBnClickedOffGgs();
            mazaflag = 0;
            m_bOffGgs.SetState(TRUE);
        }
        if ((dlgSettings->getHotKey(2)).GetAt(0) == pMsg->wParam)
        {
            if (mazaflag == 1)
            {
                OnBnClickedOffGgs();
                mazaflag = 0;
            }
            OnBnClickedOnGgsMp();
            mazaflag = 1;
            m_bOnGgsMp.SetState(TRUE);
        }
        if ((dlgSettings->getHotKey(3)).GetAt(0) == pMsg->wParam)
        {
            if (mazaflag == 1)
            {
                OnBnClickedOffGgs();
                mazaflag = 0;
            }
            OnBnClickedOnGgsLift();
            mazaflag = 1;
            m_bOnGgsLift.SetState(TRUE);
        }
        if ((dlgSettings->getHotKey(4)).GetAt(0) == pMsg->wParam)
        {
            OnBnClickedOnOffOut1();
            m_bOnOffOUT1.SetState(TRUE);
        }
        if ((dlgSettings->getHotKey(5)).GetAt(0) == pMsg->wParam)
        {
            OnBnClickedGetSk();
            m_bGetSK.SetState(TRUE);
        }
        if ((dlgSettings->getHotKey(6)).GetAt(0) == pMsg->wParam)
        {
            OnBnClickedOn();
            m_bOn.SetState(TRUE);
        }
        if ((dlgSettings->getHotKey(7)).GetAt(0) == pMsg->wParam)
        {
            OnBnClickedOff();
            m_bOff.SetState(TRUE);
        }
        if ((dlgSettings->getHotKey(8)).GetAt(0) == pMsg->wParam)
        {
            OnBnClickedOnOffOut2();
            m_bOnOffOUT2.SetState(TRUE);
        }
        if ((dlgSettings->getHotKey(9)).GetAt(0) == pMsg->wParam)
        {
            OnBnClickedGetVers();
            m_bGetVers.SetState(TRUE);
        }
        if ((dlgSettings->getHotKey(10)).GetAt(0) == pMsg->wParam)
        {
            OnBnClickedConnect();
            m_bConnect.SetState(TRUE);
        }

        SetTimer(tmPushBut, 200, 0);
        ((CEdit*)GetDlgItem(IDC_MAC))->SetFocus();
        /*switch(pMsg->wParam)
        {
            case VK_F5:
                {
                    OnBnClickedGetSk();

                    m_bGetSK.SetState(TRUE);
                    SetTimer(tmPushBut, 200, 0);
                    break;
                }
            case 'm':
            case 'M':
                {
                    OnBnClickedOnGgsMp();

                    m_bOnGgsMp.SetState(TRUE);
                    SetTimer(tmPushBut, 200, 0);
                    break;
                }
            case 'k':
            case 'K':
                {
                    OnBnClickedOnGgsLift();

                    m_bOnGgsLift.SetState(TRUE);
                    SetTimer(tmPushBut, 200, 0);
                    break;
                }
            case 'o':
            case 'O':
                {
                    OnBnClickedOffGgs();

                    m_bOffGgs.SetState(TRUE);
                    SetTimer(tmPushBut, 200, 0);
                    break;
                }
            case '4':
            case VK_F4:
                {
                    OnBnClickedOnOffOut1();

                    m_bOnOffOUT1.SetState(TRUE);
                    SetTimer(tmPushBut, 200, 0);
                    break;
                }
            case '5':
                {
                    OnBnClickedOnOffOut2();

                    m_bOnOffOUT2.SetState(TRUE);
                    SetTimer(tmPushBut, 200, 0);
                    break;
                }
            case '6':
                {
                    OnBnClickedGetVers();

                    m_bGetVers.SetState(TRUE);
                    SetTimer(tmPushBut, 200, 0);
                    break;
                }
            case '7':
                {
                    OnBnClickedOn();

                    m_bOn.SetState(TRUE);
                    SetTimer(tmPushBut, 200, 0);
                    break;
                }
            case '8':
                {
                    OnBnClickedOff();
                    OnBnClickedOffOut1();
                    OnBnClickedOffOut2();

                    m_bOff.SetState(TRUE);
                    m_bOnOffOUT1.SetState(TRUE);
                    m_bOnOffOUT2.SetState(TRUE);
                    SetTimer(tmPushBut, 200, 0);
                    break;
                }
            case 'b':
            case 'B':
                {
                    if(m_bModeUB)
                    {
                        m_bModeUB = false;
                    }
                    else
                    {
                        m_bModeUB = true;
                    }
                    m_Panel.Invalidate();
                    SetEvent(m_hEventAsk);
                }
        }*/
    }

    return CDialog::PreTranslateMessage(pMsg);
}

void CMTestProDlg::OnBnClickedOnGgsMp()
{
    if (theApp.m_GGS.IsStart())
    {
        return;
    }

    try
    {
        theApp.m_GGS.Start(
             m_ComGGS
            ,m_IdentOWN
            ,m_IdentLB
            ,0
            ,htonl(m_IP)
            ,0
            ,m_PortGGS
            ,m_KeySys
        );

        m_pSlot->AddMes(prDial, CMes::tmSetSost, CMes::amOnGGS, numDev);
        m_pSlot->Put();

        m_bOffGgs.SetFocus();

        m_bOnGgsLift.EnableWindow(FALSE);
        m_bOnGgsMp.EnableWindow(FALSE);
        m_bOffGgs.EnableWindow(TRUE);
    }
    catch (ErrorXML aErr)
    {
        theApp.m_Log.PutErr(aErr.m_Message, aErr.m_Hr);
    }
}

void CMTestProDlg::OnBnClickedOnGgsLift()
{
    if (theApp.m_GGS.IsStart())
    {
        return;
    }

    try
    {
        theApp.m_GGS.Start(
             m_ComGGS
            ,m_IdentOWN
            ,m_IdentLB
            ,0
            ,htonl(m_IP)
            ,0
            ,m_PortGGS
            ,m_KeySys
        );

        m_pSlot->AddMes(prDial, CMes::tmSetSost, CMes::amOnGGSD, numDev);
        m_pSlot->Put();

        m_bOffGgs.SetFocus();

        m_bOnGgsLift.EnableWindow(FALSE);
        m_bOnGgsMp.EnableWindow(FALSE);
        m_bOffGgs.EnableWindow(TRUE);
    }
    catch (ErrorXML aErr)
    {
        theApp.m_Log.PutErr(aErr.m_Message, aErr.m_Hr);
    }
}

void CMTestProDlg::OnBnClickedOffGgs()
{
    if (!theApp.m_GGS.IsStart())
    {
        return;
    }

    m_pSlot->AddMes(prDial, CMes::tmSetSost, CMes::amOffGGS, numDev);
    m_pSlot->Put();

    theApp.m_GGS.Stop();

    m_bOnGgsLift.EnableWindow(TRUE);
    m_bOnGgsMp.EnableWindow(TRUE);
    m_bOffGgs.EnableWindow(FALSE);
}

void CMTestProDlg::OnBnClickedOn()
{
    m_pLift->AddMes(prAskDriv, CMes::tmGetSostD,    CMes::amOnLift);
    m_pLift->Put();
}

void CMTestProDlg::OnBnClickedOff()
{
    m_pLift->AddMes(prAskDriv, CMes::tmGetSostD,    CMes::amOffLift);
    m_pLift->Put();
}

void CMTestProDlg::OnBnClickedReset()
{
    m_pLift->AddMes(prAskDriv, CMes::tmGetSostD,    CMes::amClrTrig);
    m_pLift->Put();
}

void CMTestProDlg::OnBnClickedGetSk()
{
    m_pLift->AddMes(prAskDriv, CMes::tmGetSostD,    CMes::amNumKey);
    m_pLift->Put();
}

void CMTestProDlg::OnBnClickedOnOffOut1()
{
    BYTE Mask[4];

    memset(Mask, 0, sizeof(Mask));
    Mask[0] = 0x1;

    m_pLift->AddMes(
         prAskDriv
        ,CMes::tmGetSostD
        ,m_SostOUT1 ? CMes::amOffLift : CMes::amOnLift
        ,Mask
        ,sizeof(Mask)
    );
    m_pLift->Put();
}

void CMTestProDlg::OnBnClickedOffOut1()
{
    BYTE Mask[4];

    memset(Mask, 0, sizeof(Mask));
    Mask[0] = 0x1;

    m_pLift->AddMes(
         prAskDriv
        ,CMes::tmGetSostD
        ,CMes::amOffLift
        ,Mask
        ,sizeof(Mask)
    );
    m_pLift->Put();
}

void CMTestProDlg::OnBnClickedOnOffOut2()
{
    BYTE Mask[4];

    memset(Mask, 0, sizeof(Mask));
    Mask[0] = 0x8;

    m_pLift->AddMes(
         prAskDriv
        ,CMes::tmGetSostD
        ,m_SostUSER4 ? CMes::amOffLift : CMes::amOnLift
        ,Mask
        ,sizeof(Mask)
    );
    m_pLift->Put();
}

void CMTestProDlg::OnBnClickedOffOut2()
{
    BYTE Mask[4];

    memset(Mask, 0, sizeof(Mask));
    Mask[0] = 0x8;

    m_pLift->AddMes(
         prAskDriv
        ,CMes::tmGetSostD
        ,CMes::amOffLift
        ,Mask
        ,sizeof(Mask)
    );
    m_pLift->Put();
}


void CMTestProDlg::OnBnClickedEmulat()
{
    CString buf;
    TCHAR *lpFilePart = NULL;
    TCHAR name[_MAX_PATH+1] = { 0 };

    DWORD num = ::SearchPath(
         NULL
        ,_T("EmulMCS.exe")
        ,NULL
        ,_MAX_PATH
        ,name
        ,&lpFilePart
    );

    if (num > 0)
    {
        CString arg;
        int TypEmul = 0;

        switch (m_pLift->m_TypExt)
        {
        case CDev::te6Load: // Загрузчик.
            {
                break;
            }
        case CDev::te6Mog: // Могилевлифтмаш (УУЛ)
            {
                break;
            }
        case CDev::te6MEL: // МЭЛ (СУЛ1)
            {
                TypEmul = 2;
                break;
            }
        case CDev::te6MPPL: // МППЛ (НПО <Вектор>)
            {
                break;
            }
        case CDev::te6Otis: // ОТИС
            {
                TypEmul = 10;
                break;
            }
        case CDev::te6S17: // ШУЛК17 
            {
                TypEmul = 11;
                break;
            }
        case CDev::te6S32: // ШУЛК32
            {
                TypEmul = 4;
                break;
            }
        case CDev::te6UCL: // УКЛ/УЛ
            {
                TypEmul = 3;
                break;
            }
        case CDev::te6Rele: // Релейный лифт.
            {
                TypEmul = 5;
                break;
            }
        case CDev::te6Sodi: // SODIMAS
            {
                TypEmul = 1;
                break;
            }
        case CDev::te6LGDI: // LG_DI
            {
                TypEmul = 6;
                break;
            }
        case CDev::te6LGDSS: // LG_DSS
            {
                TypEmul = 8;
                break;
            }
        case CDev::teEscal:
            {
                break;
            }
        case CDev::te6AXEL:
            {
                break;
            }
        case CDev::te6ELEX:
            {
                break;
            }
        case CDev::te6SPUL:
            {
                TypEmul = 13;
                break;
            }
        case CDev::te6THYSSEN:
            {
                TypEmul = 12;
                break;
            }
        case CDev::te6OLIMP:
            {
                TypEmul = 14;
                break;
            }
        case CDev::te6Doppler:
            {
                TypEmul = 15;
                break;
            }
        case CDev::te6THYSSEN3:
            {
                TypEmul = 16;
                break;
            }
        case CDev::te6EXPRESS:
            {
                TypEmul = 17;
                break;
            }
        case CDev::te6KONE:
            {
                TypEmul = 24;
                break;
            }
        }

        arg.Format(_T(" -e%d"), TypEmul);

        STARTUPINFO StartInfo;
        PROCESS_INFORMATION ProcessInformation;

        GetStartupInfo(&StartInfo);

        BOOL create_process = ::CreateProcess(
             name
            ,(TCHAR*)(const TCHAR*)arg
            ,NULL
            ,NULL
            ,FALSE
            ,0
            ,NULL
            ,NULL
            ,&StartInfo
            ,&ProcessInformation
        );
        if (!create_process)
        {
            AfxMessageBox(
                 _T("Ошибка при запуске эмулятора (EmulMCS.exe)")
                , MB_OK | MB_ICONEXCLAMATION
            );
        }
    }
    else
    {
        AfxMessageBox(
             _T("Не найден эмулятор (EmulMCS.exe)")
            , MB_OK | MB_ICONEXCLAMATION
        );
    }
}


void CMTestProDlg::OnTimer(UINT nIDEvent)
{
    switch (nIDEvent)
    {
    case batLevel:
    {
        UpdateData(TRUE);
        battery_value = _ttof(battery_value_str);

        if (last_level > battery_value)
        {
            battery_level = -1;
        }
        else if (last_level < battery_value)
        {
            battery_level = 1;
        }
        last_level = battery_value;
        InvalidateRect(&Rect, FALSE);
        break;
    }
    case tmBlik:
    {
        m_iIndicator.SetIcon(m_iReceive);
        KillTimer(tmBlik);
        break;
    }
    case tmPushBut:
    {
        if ((m_bOnGgsLift.GetState() & 0x0004) != 0) m_bOnGgsLift.SetState(FALSE);
        if ((m_bOnGgsMp.GetState() & 0x0004) != 0) m_bOnGgsMp.SetState(FALSE);
        if ((m_bOffGgs.GetState() & 0x0004) != 0) m_bOffGgs.SetState(FALSE);
        if ((m_bOn.GetState() & 0x0004) != 0) m_bOn.SetState(FALSE);
        if ((m_bOff.GetState() & 0x0004) != 0) m_bOff.SetState(FALSE);
        if ((m_bReset.GetState() & 0x0004) != 0) m_bReset.SetState(FALSE);
        if ((m_bGetSK.GetState() & 0x0004) != 0) m_bGetSK.SetState(FALSE);
        if ((m_bOnOffOUT1.GetState() & 0x0004) != 0) m_bOnOffOUT1.SetState(FALSE);
        if ((m_bOnOffOUT2.GetState() & 0x0004) != 0) m_bOnOffOUT2.SetState(FALSE);
        if ((m_bEmulat.GetState() & 0x0004) != 0) m_bEmulat.SetState(FALSE);
        if ((m_bGetVers.GetState() & 0x0004) != 0) m_bGetVers.SetState(FALSE);

        KillTimer(tmPushBut);
        break;
    }
    case tmGetIdent:
    {
        if (!connect_btn_flag)
        {
            break;
        }
        m_cComLB.GetWindowText(m_ComLB);

        if ((!m_AutoParam || !no_com_port_flag) && !no_no_flag)
        {
            /*if (!theApp.m_PortLB.IsOpen())
            {
                try
                {
                    theApp.m_PortLB.Open(m_ComLB);
                }
                catch (ErrorXML aErr)
                {
                    no_com_port_flag = true;
                    no_no_flag = true;
                    Disconnect();
                    theApp.m_Log.PutErr(aErr.m_Message, aErr.m_Hr, true);
                }
                mac_last_char = m_MAC.Mid(6, 2) + _T("-") + m_MAC.Mid(8, 2) + _T("-") + m_MAC.Right(2);
                UpdateData(FALSE);
            }
            if (theApp.m_PortLB.IsOpen())
            {
                theApp.m_PortLB.SendAsk();
            }
            no_com_port_flag = true;*/
        }
        else
        {
            no_com_port_flag = true;
            if (theApp.m_PortLB.IsOpen())
            {
                theApp.m_PortLB.Close();
                no_no_flag = true;
            }
        }
        break;
    }
    case tmWaitChgStr:
    {
        if (m_CountWaitAct == 1)
        {
            m_cAutoParam.EnableWindow(TRUE);
            m_bConnect.EnableWindow(TRUE);
            m_AutoParam = m_SaveAutoParam;
            UpdateData(FALSE);
            Start();
            KillTimer(tmWaitChgStr);
        }
        else
        {
            CString buf;
            buf.Format("Ожидание активации %d", m_CountWaitAct);
            m_bSwitchPage.SetWindowText(buf);
        }
        break;
    }
    }
    CDialog::OnTimer(nIDEvent);
}

void CMTestProDlg::OnBnClickedConnect()
{
    //m_AutoParam = FALSE;
    //UpdateData(TRUE);
    //if (m_AutoParam && m_bOpen) no_com_port_flag = false;
    //ShellExecute(NULL, _T("open"), _T("C:\\Windows\\System32\\arp.exe"), _T("-d *"), NULL, SW_HIDE);

    UpdateData(TRUE);

    if ((!m_AutoParam || !no_com_port_flag) && !no_no_flag)
    {
        if (!theApp.m_PortLB.IsOpen())
        {
            try
            {
                m_cComLB.GetWindowText(m_ComLB);
                theApp.m_PortLB.Open(m_ComLB);
            }
            catch (ErrorXML aErr)
            {
                no_com_port_flag = true;
                no_no_flag = true;
                Disconnect();
                theApp.m_Log.PutErr(aErr.m_Message, aErr.m_Hr, true);
            }
            mac_last_char = m_MAC.Mid(6, 2) + _T("-") + m_MAC.Mid(8, 2) + _T("-") + m_MAC.Right(2);
            UpdateData(FALSE);
        }
        if (theApp.m_PortLB.IsOpen())
        {
            theApp.m_PortLB.SendAsk();
        }
        no_com_port_flag = true;
    }
    if (m_bOpen)
    {
        theApp.lktest_dlg->m_op2.remove_lb_connect();
        connect_btn_flag = false;
        Stop();
    }
    else
    {
        if (UpdateData(TRUE))
        {
            m_IdentLB.SetNode(m_Ident);

            if(m_Key.IsEmpty())
            {
                m_KeySys.SetDefault();
            }
            else
            {
                m_KeySys.Create(m_Key);
            }

            Start();
            theApp.lktest_dlg->m_op2.add_lb_connect();
        }
    }
}

LRESULT CMTestProDlg::OnClrPortLb(WPARAM, LPARAM)
{
    Disconnect();
    theApp.m_PortLB.Close();

    AfxMessageBox(_T("Ошибка при работе с COM портом ЛБ"), MB_OK|MB_ICONEXCLAMATION);

    return 0;
}

void CMTestProDlg::OnBnClickedAutoParam()
{
    m_AutoParam = m_cAutoParam.GetCheck() == BST_CHECKED? TRUE: FALSE;
    //no_no_flag = false;
    EnableParam();

    if (m_AutoParam)
    {
        no_com_port_flag = false;
    }
    //Stop();
}

void CMTestProDlg::EnableParam()
{
    m_cIP.EnableWindow(m_AutoParam && !m_bOpen);
    m_cPort.EnableWindow(m_AutoParam && !m_bOpen);
    m_cPortGGS.EnableWindow(m_AutoParam && !m_bOpen);
    m_cIdent.EnableWindow(/*m_AutoParam && !m_bOpen*/TRUE);
    m_cKey.EnableWindow(m_AutoParam && !m_bOpen);
}

void CMTestProDlg::EnableButton()
{
    EnableParam();

    m_bOnGgsLift.EnableWindow(m_bOpen ? TRUE : FALSE);
    m_bOnGgsMp.EnableWindow(m_bOpen ? TRUE : FALSE);
    m_bOffGgs.EnableWindow(m_bOpen ? TRUE : FALSE);
    m_bOn.EnableWindow(m_bOpen ? TRUE : FALSE);
    m_bOff.EnableWindow(m_bOpen ? TRUE : FALSE);
    m_bReset.EnableWindow(m_bOpen ? TRUE : FALSE);
    m_bGetSK.EnableWindow(m_bOpen ? TRUE : FALSE);
    m_bGetVers.EnableWindow(m_bOpen ? TRUE : FALSE);
    m_bOnOffOUT1.EnableWindow(m_bOpen ? TRUE : FALSE);
    m_bOnOffOUT2.EnableWindow(m_bOpen ? TRUE : FALSE);
    m_bEmulat.EnableWindow(m_bOpen ? TRUE : FALSE);
}

void CMTestProDlg::OnCbnDropdownComPort()
{
    HKEY hKey;
    long lResult = ::RegOpenKeyEx(
         HKEY_LOCAL_MACHINE
        ,_T("HARDWARE\\DEVICEMAP\\SERIALCOMM")
        ,0 
        ,KEY_READ 
        ,&hKey
    );

    if (lResult != ERROR_SUCCESS)
    {
        return;
    }

    for (int i = m_cComLB.GetCount() - 1; i >= 0; i--)
    {
        m_cComLB.DeleteString(i);
    }

    for (DWORD  i = 0;; i++) 
    { 
        TCHAR Value[80];
        TCHAR Data[80];
        DWORD LenValue = sizeof(Value);
        DWORD LenData = sizeof(Data);

        DWORD retCode = RegEnumValue(
                hKey
            ,i
            ,Value
            ,&LenValue
            ,NULL
            ,NULL
            ,reinterpret_cast<BYTE*>(Data)
            ,&LenData
        );

        if (retCode == ERROR_SUCCESS)
        {
            m_cComLB.InsertString(-1, Data);
        }
        else
        {
            break;
        }
    } 
    RegCloseKey(hKey);
}

void CMTestProDlg::OnCbnDropdownComUsb()
{
    HKEY hKey;
    long lResult = ::RegOpenKeyEx(
         HKEY_LOCAL_MACHINE
        ,_T("HARDWARE\\DEVICEMAP\\SERIALCOMM")
        ,0
        ,KEY_READ
        ,&hKey
    );

    if (lResult != ERROR_SUCCESS)
    {
        return;
    }

    for (int i = m_cComGGS.GetCount() - 1; i >= 0; i--)
    {
        m_cComGGS.DeleteString(i);
    }

    for (DWORD  i = 0;; i++) 
    { 
        TCHAR Value[80];
        TCHAR Data[80];
        DWORD LenValue = sizeof(Value);
        DWORD LenData = sizeof(Data);

        DWORD retCode = RegEnumValue(
             hKey
            ,i
            ,Value
            ,&LenValue
            ,NULL
            ,NULL
            ,reinterpret_cast<BYTE*>(Data)
            ,&LenData
        );

        if (retCode == ERROR_SUCCESS)
        {
            m_cComGGS.InsertString(-1, Data);
        }
        else
        {
            break;
        }
    } 
    RegCloseKey(hKey);
}


LRESULT CMTestProDlg::OnNoLink(WPARAM w, LPARAM l)
{
    MessageBeep(MB_ICONEXCLAMATION);
    return 0;
}

LRESULT CMTestProDlg::OnErrSocket(WPARAM w, LPARAM l)
{
    Disconnect();
    return 0;
}

afx_msg void CMTestProDlg::OnBnClickedSwitchPage()
{
    BYTE BodyIAP[4] = {
         CMes::saIAPActiv
        ,m_IsActLoader ? 1 : 0
        ,0
        ,0
    };

    if (theApp.m_PortLB.IsOpen()) 
    {
        theApp.m_PortLB.Close();
    }
    m_pLift->AddMes( // запрос активации
         prAskDriv
        ,CMes::tmGetSostD
        ,CMes::amIAP
        ,BodyIAP
        ,sizeof(BodyIAP)
    );
    Sleep(500);
    Stop();
    m_CountWaitAct  = 12;
    CString buf;
    buf.Format("Ожидание активации %d", m_CountWaitAct);
    m_bSwitchPage.SetWindowText(buf);
    m_SaveAutoParam = m_AutoParam;
    m_AutoParam = FALSE;
    m_cAutoParam.EnableWindow(FALSE);
    m_bConnect.EnableWindow(FALSE);
    SetTimer(tmWaitChgStr, 1000, 0);
}


void CMTestProDlg::on_settings()
{
    if (dlgSettings)
    {
        dlgSettings->DoModal();
    }
}

void CMTestProDlg::OnSize(UINT nType, int cx, int cy)
{
    CDialog::OnSize(nType, cx, cy);

    /*if (GetDlgItem(IDD_MTESTPRO_DIALOG) != NULL)
    {
        ((CTabCtrl*)GetDlgItem(IDD_MTESTPRO_DIALOG))->MoveWindow(0, 0, cx, cy);
    }*/
}

void CMTestProDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    CDialog::OnKeyDown(nChar, nRepCnt, nFlags);
}


void CMTestProDlg::OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    CDialog::OnSysKeyDown(nChar, nRepCnt, nFlags);
}


void CMTestProDlg::OnWindowPosChanged(WINDOWPOS* lpWP)
{
    CDialog::OnWindowPosChanged(lpWP);
    WINDOWPLACEMENT wndpl;
    // remember current position and size
    CWinApp* pApp = AfxGetApp();
    pApp->WriteProfileInt(_T("MTestParam"), _T("Left"), lpWP->x);
    pApp->WriteProfileInt(_T("MTestParam"), _T("Top"), lpWP->y);
    pApp->WriteProfileInt(_T("MTestParam"), _T("Width"), lpWP->cx);
    pApp->WriteProfileInt(_T("MTestParam"), _T("Height"), lpWP->cy);

    /*// obtain and remember current placement
    GetWindowPlacement(&wndpl);
    pApp->WriteProfileInt(_T("MTestParam"), _T("Show"), wndpl.showCmd);*/

    // flush profile under 9x
    ::WritePrivateProfileString(NULL, NULL, NULL, pApp->m_pszProfileName);
}


BOOL CMTestProDlg::PreCreateWindow(CREATESTRUCT& cs)
{
    //MessageBox(_T("suka"));
    //if (CDialog::PreCreateWindow(cs)) {
        // restore remembered position and size
        /*CWinApp* pApp = AfxGetApp();
        pApp->m_nCmdShow = pApp->GetProfileInt(_T("Param"), _T("Show"), SW_SHOWNORMAL);
        if (pApp->m_nCmdShow == SW_SHOWNORMAL) {
            cs.x = pApp->GetProfileInt(_T("Param"), _T("Left"), CW_USEDEFAULT);
            cs.y = pApp->GetProfileInt(_T("Param"), _T("Top"), CW_USEDEFAULT);
            cs.cx = pApp->GetProfileInt(_T("Param"), _T("Width"), CW_USEDEFAULT);
            cs.cy = pApp->GetProfileInt(_T("Param"), _T("Height"), CW_USEDEFAULT);
        }
        return (TRUE);
    /*}
    else {
        return (FALSE);
    }*/
    return CDialog::PreCreateWindow(cs);
}

