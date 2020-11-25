#include <mtest/mutex_lock.h>

// макрос, занимающий мьютекс до конца области действия
#define SCOPE_LOCK_MUTEX(hMutex) CMutexLock _tmp_mtx_capt(hMutex);

// класс-оболочка, создающий и удаляющий мютекс (Windows)
class CAutoMutex
{
    HANDLE m_h_mutex; // дескриптор создаваемого мютекса
    CAutoMutex(const CAutoMutex&); // запрет копирования
    CAutoMutex& operator=(const CAutoMutex&);

public:
    CAutoMutex()
    {
        m_h_mutex = CreateMutex(NULL, FALSE, NULL);
        assert(m_h_mutex);
    }
    ~CAutoMutex()
    {
        CloseHandle(m_h_mutex);
    }
    HANDLE get()
    {
        return m_h_mutex;
    }
};
