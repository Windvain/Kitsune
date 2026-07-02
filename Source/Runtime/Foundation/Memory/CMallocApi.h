#pragma once

#include "Foundation/Common/Macros.h"
#include "Foundation/Memory/MemoryApi.h"

namespace Kitsune
{
    // Implementation of the Memory API interface using the default
    // C runtime malloc and free. (Well, actually its std::aligned_alloc..)
    class KITSUNE_API CMallocApi : public MemoryApi
    {
    public:
        [[nodiscard]] void* TryAllocate(Usize bytes) override;
        [[nodiscard]] void* TryAllocate(Usize bytes, Usize alignment) override;

        void Free(void* pointer, Usize bytes) override;

    public:
        [[nodiscard]]
        inline constexpr Usize GetDefaultAlignment() const override
        {
            return s_DefaultAlignment;
        }

    public:
        [[nodiscard]]
        inline void* TryNew(Usize bytes, Usize alignment) override
        {
            return TryAllocate(bytes, alignment);
        }

        void Delete(void* pointer) override
        {
            Free(pointer, /* Unused: */ 0);
        }

    private:
        static constexpr Usize s_DefaultAlignment = __STDCPP_DEFAULT_NEW_ALIGNMENT__;
    };
}
