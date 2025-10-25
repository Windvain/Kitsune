#pragma once

#include "Foundation/Common/Types.h"

namespace Kitsune
{
    class IMemoryApi
    {
    public:
        virtual ~IMemoryApi() { /* ... */ }

        virtual void* TryAllocate(Usize bytes, Usize alignment) = 0;
        virtual void Free(void* ptr) = 0;

    public:
        virtual Usize GetDefaultAlignment() const = 0;
    };
}
