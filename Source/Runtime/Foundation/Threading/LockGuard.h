#pragma once

#include "Foundation/Threading/Mutex.h"

namespace Kitsune
{
    class LockGuard
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

    public:
        LockGuard(const LockGuard&) = delete;
        LockGuard& operator=(const LockGuard&) = delete;

    private:
        Mutex& m_Mutex;
    };
}
