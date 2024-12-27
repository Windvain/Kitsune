#pragma once

#include "Foundation/Common/Types.h"
#include "Foundation/Memory/BadAllocException.h"

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

    public:
        [[noreturn]]
        virtual void OutOfMemory(Usize bytes, Usize alignment)
        {
            KITSUNE_UNUSED(bytes);
            KITSUNE_UNUSED(alignment);

            throw BadAllocException();
        }
    };
}
