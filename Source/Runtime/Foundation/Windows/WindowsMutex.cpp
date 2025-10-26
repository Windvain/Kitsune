#include "Foundation/Threading/Mutex.h"

#include <Windows.h>
#include "Foundation/Diagnostics/SystemException.h"

namespace Kitsune
{
    class WindowsMutex : public Details::IMutexImpl
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
        inline void Acquire() override
        {
            if (!TryAcquire())
                throw SystemException("Failed to acquire a mutex.");
        }

        inline bool TryAcquire() override { return ::TryEnterCriticalSection(&m_CritSection); }
        inline void Release() override    { ::LeaveCriticalSection(&m_CritSection); }

    private:
        CRITICAL_SECTION m_CritSection;
    };

    Mutex::Mutex()
        : m_MutexImpl(MakeScoped<WindowsMutex>())
    {
    }
}
