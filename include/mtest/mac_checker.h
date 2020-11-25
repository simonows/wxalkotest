// MacChecker - класс для ведения логов контроля повторяющихся mac-адресов подключаемых устройств

#pragma once

#include "../stdafx.h"
#include <mtest/auto_mutex.h>
#include <fstream>
using namespace std;

struct CheckAnswer
{
    int result; // результат проверки
    int count; // количество подключений устройства с mac-адресом
    CString last_date; // дата&время последнего подключения
    CString user; // имя пользователя
};

class MacChecker: public fstream
{
private:
    CString file_path; // путь файла
    CString user_name; // имя оператора (максимальная длинна - 20)
    CAutoMutex mac_check_mutex; // мьютекс доступа к файлу

    bool FileIsExist(CString filePath); // проверка существования файла
    char* convertW(CString str); // преобразование из CString в char*
public:
    MacChecker();
    MacChecker(CString path, CString name);
    ~MacChecker();

    void open(); // открытие файла для чтения/записи
    void writem(CString mac); // запись лога
    CheckAnswer check(CString mac); // проверка mac-адреса
    void close(); // закрытие файла
    void setUserName(CString name); // изменение имени оператора
    CString getUserName(); // изменение имени оператора
    CString getFilePath(); // путь log-файла
    void setFilePath(CString path); // путь log-файла
};

