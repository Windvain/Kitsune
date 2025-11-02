#pragma once

#include "Foundation/Common/Macros.h"
#include "Foundation/Memory/Memory.h"

namespace Kitsune
{
    class GlobalAllocator
    {
    public:
        KITSUNE_FORCEINLINE void* Allocate(Usize bytes)
        {
            return Memory::Allocate(bytes);
        }

        KITSUNE_FORCEINLINE void* Allocate(Usize bytes, Usize align)
        {
            return Memory::Allocate(bytes, align);
        }

        KITSUNE_FORCEINLINE void Free(void* ptr)
        {
            Memory::Free(ptr);
        }
    };

    inline bool operator==(const GlobalAllocator&, const GlobalAllocator&) { return true; }
    inline bool operator!=(const GlobalAllocator&, const GlobalAllocator&) { return false; }
}
