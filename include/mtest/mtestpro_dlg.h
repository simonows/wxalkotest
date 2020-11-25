/*
 * MTestProDlg.h : header file
*/

#pragma once

#include <mtest/panel.h>
#include <mtest/mac_checker.h>

#define ID_EXISTS_PACK (WM_APP + 100)
#define ID_GET_IDENT (WM_APP + 101)
#define ID_CLR_PORT_LB (WM_APP + 102)
#define ID_CLR_PORT_GGS (WM_APP + 103)
#define NO_LINK (WM_APP + 104)
#define ERR_SOCKET (WM_APP + 105)

// световая кнопка с измененным курсором
class CCursButton: public CButton
{
    DECLARE_DYNCREATE(CCursButton)

public:
    afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
    DECLARE_MESSAGE_MAP()
};

// история состояний
class CHistSost
{
public:
    COleDateTime m_Time;
    CSostSens m_Sost;
    CSostDZ m_SostUB;
    bool m_bValid;
    long m_Count;
    int m_NumLift;

    CHistSost(CSostSens& aSost, CSostDZ& aSostUB, bool aValid = true, int aNumLift = 0)
    {
        m_Time = COleDateTime::GetCurrentTime();
        m_Sost = aSost;
        m_SostUB = aSostUB;
        m_bValid = aValid;
        m_Count = 1;
        m_NumLift = aNumLift;
    };

    CHistSost(const CHistSost& aHist)
    {
        m_Time = aHist.m_Time;
        m_Sost = aHist.m_Sost;
        m_SostUB = aHist.m_SostUB;
        m_bValid = aHist.m_bValid;
        m_Count = aHist.m_Count;
        m_NumLift = aHist.m_NumLift;
    }

    void operator = (const CHistSost& sl) {m_Time = sl.m_Time; m_Sost = sl.m_Sost; m_SostUB = sl.m_SostUB;};
    int operator == (CHistSost& sl);
    int operator != (CHistSost& sl);
};

// массив историй изменения состояний
class CHistSostArray : public CSimpleArray<CHistSost>
{
public:
    //enum {maxLenHist = 8};
    enum {maxLenHist = 16};
    CHistSostArray(){};

    bool Put(CHistSost& aSost);
};

// неизменяемый признак
class CFlagOut: public CButton
{
    DECLARE_DYNCREATE(CFlagOut)

public:
    afx_msg void OnLButtonDown( UINT nFlags, CPoint point ) {};
    afx_msg void OnLButtonDblClk( UINT nFlags, CPoint point ) {};
    DECLARE_MESSAGE_MAP()
};

// допустимое значение прошивки
class CValidVers
{
public:
    CDev::typDev m_TypDev;
    CDev::typExt m_TypExt;
    CString m_Vers;

    CValidVers(CDev::typDev aTypDev, CDev::typExt aTypExt, const char* aVers)
        :m_TypDev(aTypDev)
        ,m_TypExt(aTypExt)
        ,m_Vers(aVers)
    {};
};

// массив допустимых значений прошивок
class CListValidVers : public list<CValidVers>
{
public:
    bool Load();
    CString Find(CDev::typDev aTypDev, CDev::typExt aTypExt);

    static const TCHAR* s_File;
};

// CTestSulPult
class CMTestProDlg;

class CMTestProDisp: public CDisp
{
protected:
    CMTestProDlg *m_pDlg;
public:
    CMTestProDisp() { m_pDlg = 0; m_pPult = 0; };
    void Set(CMTestProDlg *aDlg) { m_pDlg = aDlg; };
    virtual void Put(CPult *aPult, CPackMes& aPack);

    CPult *m_pPult;
};

// CMTestProDlg dialog
class CMTestProDlg : public CDialog
{
public:
    CMTestProDlg(CWnd* pParent = NULL);

    void on_settings();
    MacChecker* macfile;
    int battery_level = 0;
    bool no_com_port_flag = false, no_no_flag = false;
    bool connect_btn_flag = false;
    float last_level = 0;
    RECT Rect;
    int mazaflag = 0;
    CString battery_value_str;
    float battery_value = 0;
    CString bat_value;

    enum { IDD = IDD_MTESTPRO_DIALOG };
    enum
    {
        tmBlik = 1, // таймер мигания лампочки
        tmPushBut = 2, // таймер нажатия кнопки
        tmGetIdent = 3, // таймер чтения идентификатора ЛБ
        tmWaitChgStr= 4, // ожидание изменения активности загрузчик/страница 1
        numDev = 1, // номер ЛБ на "локальной шине"
        batLevel = 5, // таймер чтения состояния батареи
    };

    BOOL m_SostLb0;
    BOOL m_SostLb1;
    BOOL m_SostLb2;
    BOOL m_SostLb3;
    BOOL m_SostLb4;
    BOOL m_SostLb5;
    BOOL m_SostLb6;
    BOOL m_SostLb7;
    BOOL m_SostLb8;
    BOOL m_SostLb9;
    BOOL m_SostLb10;
    BOOL m_SostLb11;
    BOOL m_SostLb12;
    BOOL m_SostUSER1;
    BOOL m_SostUSER2;
    BOOL m_SostUSER3;
    BOOL m_SostUSER4;
    BOOL m_SostOUT1;
    BOOL m_Sost220V;
    BOOL m_SostAsk;
    BOOL m_SostMP;
    BOOL m_SostInnr;
    BOOL m_SostSecur;
    BOOL m_SostNoMCS;
    BOOL m_Door1;
    BOOL m_Door2;
    int  m_Ident;
    CString m_MAC;
    CString m_Key;
    CString m_KeyInLb; // ключ, вычитанный из самого ЛБ
    CString m_ComGGS; // COM порт голосового конвертера
    CString m_ComLB; // COM порт лифтового блока
    DWORD m_IP; // IP адрес ЛБ
    int m_Port; // IP порт данных ЛБ
    int m_PortGGS; // IP порт голоса ЛБ
    CIdentV7 m_IdentOWN; // собственный идентификатор
    CKeySys m_KeySys; // ключ системы
    CIdentV7 m_IdentLB; // идентификатор ЛБ
    BOOL m_AutoParam;
    BOOL m_SaveAutoParam;
    CString mac_last_char; // последние 6 символов mac-адреса
    CToolTipCtrl *m_pToolTip;
    bool m_bOpen;
    CMTestProDisp m_Disp; // структура оборудования
    CSlot *m_pSlot; // указатель на слот устройства
    CDev *m_pLift; // указатели на ЛБ одного КЛШ
    InpQueuePackV7 m_InpQueue;
    CHistSostArray m_Hist; // история состояний
    bool m_bNewSost; // приход нового состояния
    bool m_bKnovSost; // состояние известно, т.е. пришел ответ
    HICON m_iSend; // икона лампа горит
    HICON m_iReceive; // икона лампа погасла
    bool m_bModeUB; // режим показа датчиков УБ
    HANDLE m_hEventAsk; // событие ожидания посылки следующего опроса
    COleDateTime m_LastGoodMes; // время прихода последнего хорошего сообщения
    CListValidVers m_ValidVers; // правильные версии
    CString m_Vers; // версия микропрограммы
    CBrush m_Red; // красный фон
    CBrush m_Yello; // желтый фон
    CBrush m_Green; // зеленый фон
    CBrush m_White; // белый фон
    bool duplicate = false;
    bool duplicate2 = false;
    bool m_IsExistLoader; // загрузчик существует
    bool m_IsActLoader; // загрузчик активен
    bool m_IsExistPage1; // в странице 1 есть микропрограмма
    bool m_IsActPage1; // страница 1 активена
    int  m_CountWaitAct; // число секунт до завершения активации

    bool Start(); // запуск процессов
    void Stop(); // останов процессов
    void InitStruct(); // инициация структуры
    void EnableParam();
    void EnableButton();
    void Disconnect();

    HANDLE volatile m_hAskThread;
    DWORD m_dwThreadAskID;
    static DWORD WINAPI AskLift(CMTestProDlg *lpDlg);

    HANDLE m_hTypAnsw[3]; // событие хорошего [0], прерывания [1] и плохого [2] ответа на запрос


protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support


// Implementation
protected:
    HICON m_hIcon;
    CFlagOut m_bSostLb0;
    CFlagOut m_bSostLb1;
    CFlagOut m_bSostLb2;
    CFlagOut m_bSostLb3;
    CFlagOut m_bSostLb4;
    CFlagOut m_bSostLb5;
    CFlagOut m_bSostLb6;
    CFlagOut m_bSostLb7;
    CFlagOut m_bSostLb8;
    CFlagOut m_bSostLb9;
    CFlagOut m_bSostLb10;
    CFlagOut m_bSostLb11;
    CFlagOut m_bSostLb12;
    CFlagOut m_bSostUSER1;
    CFlagOut m_bSostUSER2;
    CFlagOut m_bSostUSER3;
    CFlagOut m_bSostUSER4;
    CFlagOut m_bSostOUT1;
    CFlagOut m_bSost220V;
    CFlagOut m_bSostAsk;
    CFlagOut m_bSostMP;
    CFlagOut m_bSostInnr;
    CFlagOut m_bSostSecur;
    CFlagOut m_bSostNoMCS;
    CFlagOut m_bDoor1;
    CFlagOut m_bDoor2;
    CPanel m_Panel;
    CCursButton m_bOnGgsLift;
    CCursButton m_bOnGgsMp;
    CCursButton m_bOffGgs;
    CCursButton m_bOn;
    CCursButton m_bOff;
    CCursButton m_bReset;
    CCursButton m_bGetSK;
    CCursButton m_bGetVers;
    CCursButton m_bOnOffOUT1;
    CCursButton m_bOnOffOUT2;
    CCursButton m_bEmulat;
    CCursButton m_bSwitchPage;
    CEdit m_eNumSK;
    CEdit m_eVers;
    CEdit m_eCodRestarts;
    CStatic m_iIndicator;
    CCursButton m_bConnect;
    CComboBox m_cComLB;
    CComboBox m_cComGGS;
    CIPAddressCtrl m_cIP;
    CEdit m_cPort;
    CEdit m_cPortGGS;
    CEdit m_cIdent;
    CEdit m_cMAC;
    CEdit m_cKey;
    CEdit m_cAkkum;
    CButton m_cAutoParam;

    // Generated message map functions
    virtual BOOL OnInitDialog();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnDestroy();
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    afx_msg LRESULT OnExistsPack(WPARAM, LPARAM);
    afx_msg LRESULT OnGetIdent(WPARAM, LPARAM);
    afx_msg LRESULT OnClrPortLb(WPARAM, LPARAM);
    afx_msg LRESULT OnClrPortGGS(WPARAM, LPARAM);
    afx_msg LRESULT OnNoLink(WPARAM, LPARAM);
    afx_msg LRESULT OnErrSocket(WPARAM, LPARAM);
    DECLARE_MESSAGE_MAP()
public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    afx_msg void OnTimer(UINT nIDEvent);
    afx_msg void OnBnClickedOnGgsMp();
    afx_msg void OnBnClickedOnGgsLift();
    afx_msg void OnBnClickedOffGgs();
    afx_msg void OnBnClickedOn();
    afx_msg void OnBnClickedOff();
    afx_msg void OnBnClickedReset();
    afx_msg void OnBnClickedGetSk();
    afx_msg void OnBnClickedOnOffOut1();
    afx_msg void OnBnClickedOffOut1();
    afx_msg void OnBnClickedOnOffOut2();
    afx_msg void OnBnClickedOffOut2();
    afx_msg void OnBnClickedEmulat();
    afx_msg void OnBnClickedGetVers();
    afx_msg void OnBnClickedConnect();
    afx_msg void OnBnClickedAutoParam();
    afx_msg void OnBnClickedSwitchPage();
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    afx_msg void OnCbnDropdownComPort();
    afx_msg void OnCbnDropdownComUsb();
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);
//  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
//  afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
    BOOL PreCreateWindow(CREATESTRUCT& cs);
    afx_msg void OnSize(UINT nType, int cx, int cy);
};

