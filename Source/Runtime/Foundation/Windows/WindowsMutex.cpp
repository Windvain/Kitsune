#include "Foundation/Threading/Mutex.h"
#include <Windows.h>

#include "Foundation/Common/Macros.h"
#include "Foundation/Diagnostics/Assert.h"

#include "Foundation/Diagnostics/SystemException.h"

namespace Kitsune
{
    struct WindowsMutexData
    {
        SRWLOCK ReadWriteLock;
        DWORD ThreadId;
    };

    template<bool BlockThread>
    static bool AcquireMutex(WindowsMutexData* data)
    {
        DWORD threadId = ::GetCurrentThreadId();
        if (data->ThreadId == threadId)
            throw SystemException("The calling thread already owns this mutex.");

        data->ThreadId = threadId;

        if constexpr (!BlockThread)
            return ::TryAcquireSRWLockExclusive(&data->ReadWriteLock);
        else
        {
            ::AcquireSRWLockExclusive(&data->ReadWriteLock);
            return true;
        }
    }

    Mutex::Mutex()
    {
        static_assert(sizeof(WindowsMutexData) <= s_BufferSize,
                      "The platform mutex data will not fit into the buffer.");

        auto* data = reinterpret_cast<WindowsMutexData*>(m_Buffer);
        data->ThreadId = 0;     // This is an invalid thread ID. Thank you Raymond!

        ::InitializeSRWLock(&data->ReadWriteLock);
    }

    Mutex::~Mutex()
    {
        auto* data = reinterpret_cast<WindowsMutexData*>(m_Buffer);
        KITSUNE_UNUSED(data);

        KITSUNE_ASSERT(data->ThreadId == 0,
                       "The owning thread did not release ownership of this mutex.");
    }

    void Mutex::Acquire()
    {
        // Ignore the return value of AcquireMutex<true>(), it's always going to be
        // true anyways.
        auto* mutexData = reinterpret_cast<WindowsMutexData*>(m_Buffer);
        KITSUNE_UNUSED(AcquireMutex<true>(mutexData));
    }

    bool Mutex::TryAcquire()
    {
        return AcquireMutex<false>(reinterpret_cast<WindowsMutexData*>(m_Buffer));
    }

    void Mutex::Release()
    {
        auto* data = reinterpret_cast<WindowsMutexData*>(m_Buffer);
        if (data->ThreadId != ::GetCurrentThreadId())
            throw SystemException("Cannot free a mutex owned by a different thread.");

        ::ReleaseSRWLockExclusive(&data->ReadWriteLock);
        data->ThreadId = 0;
    }
}
