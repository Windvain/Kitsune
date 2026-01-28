#pragma once

#include "Foundation/Common/Types.h"
#include "Foundation/Utilities/NonCopyable.h"

namespace Kitsune
{
    class Mutex : public NonCopyable
    {
    public:
        Mutex();
        ~Mutex();

    public:
        void Acquire();
        bool TryAcquire();

        void Release();

    private:
        static constexpr Usize s_BufferSize = 64;

    private:
        // Not using the PImpl idiom here, because I'm trying to make
        // threading primitives as fast as possible.
        Uint8 m_Buffer[s_BufferSize];
    };
}
