/*
 * Класс окна пользовательской настройки. Класс реализует настройку Log-файла,
 * выбор горячих клавиu и задание пользовательских данных.
*/

#ifndef __SETTINGS_DLG_H__
#define __SETTINGS_DLG_H__

#define STRING_DEFAULT "-!None!-"

#include <fstream>
#include "../stdafx.h"
#include <mtest/mac_checker.h>
#include "../../src/MTestPro/res/resource.h"
using namespace std;

class CSettingsDlg: public CDialog
{
private:
    MacChecker** custom_checker; // формирователь Log-файла для mac-адресов
    CString name_value; // значение имени пользователя (оригинал хранится в custom_checker)
    CString hot_keys[11]; // значения горячих клавиш
    void readFile(); // чтение настроек из файла
    void writeFile(); // запись настроек в файл или создание нового файла
    bool FileIsExist(CString filePath); // проверка существования файла
    char* convertW(CString str); // преобразование CString строки в char*
    CStatic* path_label; // указатель на статический текст окна настройки, отвечающий за путь
    CStatic* name_label;
    void setMacPathComponent();

public:
    CSettingsDlg();
    ~CSettingsDlg();
    bool CSettingsDlg::FileExist();
    CSettingsDlg(MacChecker** checker);
    enum { IDD = IDD_SETTINGS_DIALOG }; // Dialog Data
    virtual void DoDataExchange(CDataExchange* pDX); // DDX/DDV support
    afx_msg void OnBnClickedOk();
    void setMacChecker(MacChecker** checker); // задать формирователя, если не был задан при создании объекта
    CString getHotKey(int index); // получить значение горячей клавиши под индексом index

protected:
    virtual BOOL OnInitDialog();
    DECLARE_MESSAGE_MAP()

public:
    afx_msg void OnBnClickedButton1();
};


bool CSettingsDlg::FileExist()
{
    return STRING_DEFAULT != theApp.GetProfileString(
         _T("MTestParam")
        ,_T("name_value")
        ,STRING_DEFAULT
    );
}


void CSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT1, name_value);
    DDX_Text(pDX, IDC_EDIT2, hot_keys[0]);
    DDX_Text(pDX, IDC_EDIT3, hot_keys[1]);
    DDX_Text(pDX, IDC_EDIT4, hot_keys[2]);
    DDX_Text(pDX, IDC_EDIT5, hot_keys[3]);
    DDX_Text(pDX, IDC_EDIT6, hot_keys[4]);
    DDX_Text(pDX, IDC_EDIT7, hot_keys[5]);
    DDX_Text(pDX, IDC_EDIT8, hot_keys[6]);
    DDX_Text(pDX, IDC_EDIT9, hot_keys[7]);
    DDX_Text(pDX, IDC_EDIT10, hot_keys[8]);
    DDX_Text(pDX, IDC_EDIT11, hot_keys[9]);
    DDX_Text(pDX, IDC_EDIT12, hot_keys[10]);
}


char* CSettingsDlg::convertW(CString str)
{
    int size = str.GetLength();
    char* temp = new char[size + 1];

    for (int i = 0; i < size; ++i)
    {
        temp[i] = str.GetAt(i);
    }
    temp[size] = '\0';

    return temp;
}


void CSettingsDlg::setMacPathComponent(){
    int i, size;

    CString path = (*custom_checker)->getFilePath();
    size = path.GetLength();
    for (i = 0; path.GetAt(size - i - 1) != '\\'; ++i);
    path_label->SetWindowText(path.Left(size - i - 1));
    name_label->SetWindowText(path.Right(i));
}


bool CSettingsDlg::FileIsExist(CString filePath)
{
    bool isExist = false;
    ifstream fin(filePath);
    if (fin.is_open())
    {
        isExist = true;
    }
    fin.close();
    return isExist;
}

void CSettingsDlg::readFile(){
    CString read_param;

    if (STRING_DEFAULT == (read_param = theApp.GetProfileString(
         _T("MTestParam")
        ,_T("name_value")
        ,STRING_DEFAULT))
    ){
        hot_keys[0] = _T("1");
        hot_keys[1] = _T("O");
        hot_keys[2] = _T("M");
        hot_keys[3] = _T("K");
        hot_keys[4] = _T("C");
        hot_keys[5] = _T("6");
        hot_keys[6] = _T("7");
        hot_keys[7] = _T("8");
        hot_keys[8] = _T("V");
        hot_keys[9] = _T("0");
        hot_keys[10] = _T("P");
        return;
    }

    name_value = read_param;

    if (custom_checker)
    {
        if (*custom_checker)
        {
            (*custom_checker)->setUserName(name_value);
        }
    }

    for (int i = 0; i < 10; ++i)
    {
        read_param.Format(_T("hot_key%d"), i);
        hot_keys[i] = theApp.GetProfileString(_T("MTestParam"), read_param, STRING_DEFAULT);
    }
    read_param = theApp.GetProfileString(_T("MTestParam"), _T("file_path"), STRING_DEFAULT);

    if (*custom_checker)
    {
        (*custom_checker)->setFilePath(read_param);
    }
}

void CSettingsDlg::writeFile()
{
    CWinApp* pApp = AfxGetApp();
    CString temp_string;
    pApp->WriteProfileString(_T("MTestParam"), _T("name_value"), name_value);
    for (int i = 0; i < 11; ++i)
    {
        temp_string.Format(_T("hot_key%d"), i);
        pApp->WriteProfileString(_T("MTestParam"), temp_string, hot_keys[i]);
    }
    temp_string = (*custom_checker)->getFilePath();
    pApp->WriteProfileString(_T("MTestParam"), _T("file_path"), temp_string);
}

CSettingsDlg::CSettingsDlg(MacChecker** checker) : CDialog(CSettingsDlg::IDD)
{
    custom_checker = checker;
    readFile();
}

CSettingsDlg::~CSettingsDlg()
{

}

CSettingsDlg::CSettingsDlg() : CDialog(CSettingsDlg::IDD)
{
    custom_checker = NULL;
    readFile();
}

void CSettingsDlg::setMacChecker(MacChecker** checker){
    custom_checker = checker;
}

CString CSettingsDlg::getHotKey(int index){
    if (index >= 0 && index < 10)
    {
        return hot_keys[index];
    }
    else
    {
        return _T("0");
    }
}


BOOL CSettingsDlg::OnInitDialog()
{
    CDialog::OnInitDialog();
    name_value = (*custom_checker)->getUserName();
    path_label = (CStatic*)GetDlgItem(IDC_LOG_PATH);
    name_label = (CStatic*)GetDlgItem(IDC_LOG_NAME);
    setMacPathComponent();
    UpdateData(FALSE);

    return TRUE;
}

BEGIN_MESSAGE_MAP(CSettingsDlg, CDialog)
    ON_BN_CLICKED(IDOK, &CSettingsDlg::OnBnClickedOk)
    ON_BN_CLICKED(IDC_BUTTON1, &CSettingsDlg::OnBnClickedButton1)
END_MESSAGE_MAP()

void CSettingsDlg::OnBnClickedOk()
{
    UpdateData(TRUE);
    for (int i = 0; i < 10; i++)
    {
        switch (hot_keys[i].GetAt(0))
        {
            case 'a': hot_keys[i].SetAt(0, 'A'); break;
            case 'b': hot_keys[i].SetAt(0, 'B'); break;
            case 'c': hot_keys[i].SetAt(0, 'C'); break;
            case 'd': hot_keys[i].SetAt(0, 'D'); break;
            case 'e': hot_keys[i].SetAt(0, 'E'); break;
            case 'f': hot_keys[i].SetAt(0, 'F'); break;
            case 'g': hot_keys[i].SetAt(0, 'G'); break;
            case 'h': hot_keys[i].SetAt(0, 'H'); break;
            case 'i': hot_keys[i].SetAt(0, 'I'); break;
            case 'j': hot_keys[i].SetAt(0, 'J'); break;
            case 'k': hot_keys[i].SetAt(0, 'K'); break;
            case 'l': hot_keys[i].SetAt(0, 'L'); break;
            case 'm': hot_keys[i].SetAt(0, 'M'); break;
            case 'n': hot_keys[i].SetAt(0, 'N'); break;
            case 'o': hot_keys[i].SetAt(0, 'O'); break;
            case 'p': hot_keys[i].SetAt(0, 'P'); break;
            case 'q': hot_keys[i].SetAt(0, 'Q'); break;
            case 'r': hot_keys[i].SetAt(0, 'R'); break;
            case 's': hot_keys[i].SetAt(0, 'S'); break;
            case 't': hot_keys[i].SetAt(0, 'T'); break;
            case 'u': hot_keys[i].SetAt(0, 'U'); break;
            case 'v': hot_keys[i].SetAt(0, 'V'); break;
            case 'w': hot_keys[i].SetAt(0, 'W'); break;
            case 'x': hot_keys[i].SetAt(0, 'X'); break;
            case 'y': hot_keys[i].SetAt(0, 'Y'); break;
            case 'z': hot_keys[i].SetAt(0, 'Z'); break;
        }
    }
    UpdateData(FALSE);
    UpdateData(TRUE);
    (*custom_checker)->setUserName(name_value);
    CDialog::OnOK();
    writeFile();
}

void CSettingsDlg::OnBnClickedButton1()
{
    CFileDialog fileDialog(true, NULL, "*.log"); // объект класса выбора файла
    int result = fileDialog.DoModal(); //запустить диалоговое окно

    if (result == IDOK) //если файл выбран
    {
        name_value = _T("default_user");
        if (*custom_checker)
        {
            name_value=(*custom_checker)->getUserName();
            delete *custom_checker;
        }
        *custom_checker = new MacChecker(fileDialog.GetPathName(), name_value); // показать полный путь
    }
    setMacPathComponent();
}

#endif

