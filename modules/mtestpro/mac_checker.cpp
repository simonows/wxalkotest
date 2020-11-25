#include "../stdafx.h"
#include <mtest/mac_checker.h>

bool MacChecker::FileIsExist(CString filePath)
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

char* MacChecker::convertW(CString str)
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

CString MacChecker::getFilePath(){
    SCOPE_LOCK_MUTEX(mac_check_mutex.get());
    return file_path;
}

MacChecker::MacChecker(): fstream()
{
    file_path = L"\\\\Micar\\LB_ETH\\OTK.log";
    //file_path = L"C:\\LKDS\\file.log";
    user_name = L"default_user";
}

MacChecker::~MacChecker()
{
    fstream::close();
}

MacChecker::MacChecker(CString path, CString name): fstream()
{
    file_path = path;
    user_name = name;
}

void MacChecker::open()
{
    SCOPE_LOCK_MUTEX(mac_check_mutex.get());
    if (FileIsExist(file_path))
    {
        fstream::open(file_path, ios::in | ios::out);
    }
    else
    {
        ofstream f(file_path);
        f.close();
        fstream::open(file_path, ios::in | ios::out);
    }
}

void MacChecker::writem(CString mac)
{
    char* str = NULL;
    SCOPE_LOCK_MUTEX(mac_check_mutex.get());
    // получаем дату
    SYSTEMTIME time;
    CString szTime = "";
    GetLocalTime(&time);
    // форматируем
    szTime.Format(
         _T(" %02d:%02d:%02d %02d.%02d.%d")
        ,time.wHour
        ,time.wMinute
        ,time.wSecond
        ,time.wDay
        ,time.wMonth
        ,time.wYear
    );
    clear();
    seekp(0, ios::end);
    str = convertW(mac + szTime + _T(" ") + user_name);
    *this << '\n' << str;
    delete str;
}

CheckAnswer MacChecker::check(CString mac)
{
    long file_size = 0;
    char buffer[100] = { 0 };
    CString* temp = NULL;
    CheckAnswer answ;

    SCOPE_LOCK_MUTEX(mac_check_mutex.get());

    clear();
    seekg(0, ios::end);
    file_size = tellg();
    answ.result = 0;
    answ.count = 0;

    if (file_size == 0)
    {
        goto exit;
    }
    seekg(0, ios::beg);
    while (!eof())
    {
        this->getline(buffer, 54, '\n');
        temp = new CString(buffer);
        if (mac.Compare(temp->Left(12)) == 0)
        {
            answ.last_date = temp->Mid(13,19);
            answ.user = temp->Mid(33);
            answ.count++;
            if (eof() && answ.result!=2)
            {
                answ.result = 1;
            }
            else
            {
                answ.result = 2;
            }
        }
        delete temp;
    }

exit:
    return answ;
}

void MacChecker::close()
{
    SCOPE_LOCK_MUTEX(mac_check_mutex.get());
    fstream::close();
}

void MacChecker::setUserName(CString name)
{
    SCOPE_LOCK_MUTEX(mac_check_mutex.get());
    user_name = name;
}

CString MacChecker::getUserName()
{
    SCOPE_LOCK_MUTEX(mac_check_mutex.get());
    return user_name;
}

void MacChecker::setFilePath(CString path)
{
    SCOPE_LOCK_MUTEX(mac_check_mutex.get());
    file_path = path;
}

