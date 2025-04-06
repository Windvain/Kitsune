#include "Foundation/Threading/Mutex.h"
#include <Windows.h>

namespace Kitsune
{
    class WindowsMutex : public Internal::IMutexImpl
    {
    public:
        inline WindowsMutex()
        {
            ::InitializeCriticalSection(&m_CritSection);
        }

        inline ~WindowsMutex()
        {
            ::DeleteCriticalSection(&m_CritSection);
        }

    public:
        void Acquire() override    { ::EnterCriticalSection(&m_CritSection); }
        bool TryAcquire() override { return ::TryEnterCriticalSection(&m_CritSection); }
        void Release() override    { ::LeaveCriticalSection(&m_CritSection); }

    private:
        CRITICAL_SECTION m_CritSection;
    };

    Mutex::Mutex()
        : m_MutexImpl(MakeScoped<WindowsMutex>())
    {
    }
}
