#pragma once

#include "Foundation/Common/Macros.h"
#include "Foundation/Memory/ScopedPtr.h"

namespace Kitsune
{
    namespace Internal
    {
        class IMutexImpl
        {
        public:
            virtual ~IMutexImpl() { /* ... */ }

        public:
            virtual void Acquire() = 0;
            virtual bool TryAcquire() = 0;

            virtual void Release() = 0;
        };
    }

    class Mutex
    {
    public:
        KITSUNE_API_ Mutex();
        ~Mutex() = default;

    public:
        Mutex(Mutex&) = delete;
        Mutex& operator=(const Mutex&) = delete;

    public:
        inline void Acquire() { m_MutexImpl->Acquire(); }
        inline bool TryAcquire()
        {
            return m_MutexImpl->TryAcquire();
        }

        inline void Release()
        {
            return m_MutexImpl->Release();
        }

    private:
        ScopedPtr<Internal::IMutexImpl> m_MutexImpl;
    };
}
