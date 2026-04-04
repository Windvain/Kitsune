#include "Foundation/Memory/CMallocApi.h"
#include "Foundation/Maths/Maximum.h"

#include "Foundation/Common/Predefined.h"
#include "Foundation/Diagnostics/Assert.h"

#if defined(KITSUNE_OS_WINDOWS)
    #include <malloc.h>
#else
    #include <cstdlib>
#endif

namespace Kitsune
{
    void* CMallocApi::TryAllocate(Usize bytes)
    {
        return TryAllocate(bytes, s_DefaultAlignment);
    }

    void* CMallocApi::TryAllocate(Usize bytes, Usize alignment)
    {
        KITSUNE_ASSERT(
            bytes != 0,
            "Cannot allocate zero bytes of memory. "
            "This should have been prevented in the Memory class.");

        KITSUNE_ASSERT(
            (alignment & (alignment - 1)) == 0,
            "Alignment has to be a power of two. This should have been "
            "checked in the Memory class before sending a request for "
            "memory to MemoryApi.");

        alignment = Maths::Maximum(alignment, s_DefaultAlignment);

        // MSVC doesn't support the aligned_alloc function.
#if defined(KITSUNE_OS_WINDOWS)
        return ::_aligned_malloc(bytes, alignment);
#else
        return std::aligned_alloc(alignment, bytes);
#endif
    }

    void CMallocApi::Free(void* pointer, Usize bytes)
    {
        KITSUNE_UNUSED(bytes);
        KITSUNE_ASSERT(
            pointer != nullptr,
            "Pointer should not be a null pointer. "
            "This should have been checked in the Memory class.");

#if defined(KITSUNE_OS_WINDOWS)
        ::_aligned_free(pointer);
#else
        std::free(pointer);
#endif
    }
}
