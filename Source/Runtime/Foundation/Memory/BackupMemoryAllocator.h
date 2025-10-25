#pragma once

#include "Foundation/Common/Types.h"

namespace Kitsune
{
    class BackupMemoryAllocator
    {
    public:
        [[nodiscard]] void* Allocate(Usize size, Usize alignment);
        [[nodiscard]] void* Allocate(Usize size);

        void Free(void* ptr);

    public:
        inline bool operator==(const BackupMemoryAllocator&) const { return true; }
    };
}
