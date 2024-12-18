#pragma once

#include <cstdlib>
#include <cstddef>

#include "Foundation/Memory/IMemoryApi.h"

namespace Kitsune
{
    class CMallocApi : public IMemoryApi
    {
    public:
        CMallocApi() = default;
        ~CMallocApi() = default;

    public:
        inline void* TryAllocate(Usize bytes, Usize alignment) override
        {
            // MSVC doesn't support the aligned_alloc function.
            // https://learn.microsoft.com/en-us/cpp/standard-library/cstdlib?view=msvc-170#remarks-6
    #if defined(KITSUNE_OS_WINDOWS)
            return ::_aligned_malloc(bytes, alignment);
    #else
            return std::aligned_alloc(alignment, size);
    #endif
        }

        inline void Free(void* ptr) override
        {
#if defined(KITSUNE_OS_WINDOWS)
            return ::_aligned_free(ptr);
#else
            return std::free(ptr);
#endif
        }

        inline Usize GetDefaultAlignment() const override { return s_DefaultAlignment; }

    private:
        static constexpr Usize s_DefaultAlignment = alignof(std::max_align_t);
    };
}
