#include "Foundation/Memory/CMallocApi.h"
#include <cstdlib>

namespace Kitsune
{
    void* CMallocApi::TryAllocate(Usize bytes, Usize alignment)
    {
        // MSVC doesn't support the aligned_alloc function.
        // https://learn.microsoft.com/en-us/cpp/standard-library/cstdlib?view=msvc-170#remarks-6
#if defined(KITSUNE_OS_WINDOWS)
        return _aligned_malloc(bytes, alignment);
#else
        return std::aligned_alloc(alignment, bytes);
#endif
    }

    void CMallocApi::Free(void* ptr)
    {
#if defined(KITSUNE_OS_WINDOWS)
        return _aligned_free(ptr);
#else
        return std::free(ptr);
#endif
    }
}
