#pragma once

#include "Foundation/Common/Macros.h"
#include "Foundation/Memory/Memory.h"

namespace Kitsune
{
    // The default allocator used for allocations/deallocations in containers
    // and smart pointers, if none was specified.
    class GlobalAllocator
    {
    public:
        // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
        [[nodiscard]] KITSUNE_FORCEINLINE void* Allocate(Usize bytes)
        {
            return Memory::Allocate(bytes);
        }

        // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
        [[nodiscard]] KITSUNE_FORCEINLINE void* Allocate(Usize bytes, Usize alignment)
        {
            return Memory::Allocate(bytes, alignment);
        }

        // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
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
