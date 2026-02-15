#pragma once

#include "Foundation/Threading/Mutex.h"
#include "Foundation/Utilities/NonCopyable.h"

namespace Kitsune
{
    // Tries to acquire ownership of the mutex as soon as it is created, and
    // releases the mutex when this object goes out of scope.
    class LockGuard : public NonCopyable
    {
    public:
        LockGuard(Mutex& mutex)
            : m_Mutex(mutex)
        {
            m_Mutex.Acquire();
        }

        ~LockGuard()
        {
            m_Mutex.Release();
        }

    private:
        Mutex& m_Mutex;
    };
}
