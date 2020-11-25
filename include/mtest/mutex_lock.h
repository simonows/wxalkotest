#include <windows.h>
#include <cassert>

/*
 * класс-оболочка, занимающий и освобождающий мютекс
*/
class CMutexLock
{
    HANDLE m_mutex;
    CMutexLock(const CMutexLock&); // запрет копирования
    CMutexLock& operator=(const CMutexLock&);

public:
    CMutexLock(HANDLE mutex): m_mutex(mutex) // занимаем мютекс при конструировании объекта
    {
        const DWORD res = WaitForSingleObject(m_mutex, INFINITE);
        assert(res == WAIT_OBJECT_0);
    }
    ~CMutexLock() // освобождаем мютекс при удалении объекта
    {
        const BOOL res = ReleaseMutex(m_mutex);
        assert(res);
    }
};

