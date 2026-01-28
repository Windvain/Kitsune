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

        KITSUNE_FORCEINLINE void* Allocate(Usize bytes, Usize alignment)
        {
            return Memory::Allocate(bytes, alignment);
        }

        KITSUNE_FORCEINLINE void Free(void* pointer, Usize bytes)
        {
            Memory::Free(pointer, bytes);
        }
    };

    inline bool operator==(const GlobalAllocator&, const GlobalAllocator&)
    {
        return true;
    }
}
