#include <wx/wx.h>

class Simple: public wxFrame
{
public:
    Simple( const wxString& title);
};

Simple::Simple(const wxString& title):
    wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(250, 150))
{
    Centre();
}

class AlkoTestApp: public wxApp
{
public:
    virtual bool OnInit();

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

    virtual BOOL ProcessMessageFilter(int code, LPMSG lpMsg);

    void Mes(const TCHAR* aMes, UINT nType = MB_ICONSTOP);
};

bool AlkoTestApp::OnInit()
{
    m_Disp.Set(this);
    InitStruct();
    CDialog::OnInitDialog();

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
    m_bSostUSER1.EnableWindow(FALSE);
    m_bSostUSER2.EnableWindow(FALSE);
    m_bSostUSER3.EnableWindow(FALSE);
    m_bSostUSER4.EnableWindow(FALSE);
    m_bSostOUT1.EnableWindow(FALSE);
    m_bSost220V.EnableWindow(FALSE);
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

    Simple *simple = new Simple(wxT("Simple"));
    simple->Show(true);
    return true;
}

IMPLEMENT_APP(MyApp)

