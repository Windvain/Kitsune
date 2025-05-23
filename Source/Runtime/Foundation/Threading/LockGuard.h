#pragma once

#include "Foundation/Threading/Mutex.h"
#include "Foundation/Utilities/NonCopyable.h"

namespace Kitsune
{
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
