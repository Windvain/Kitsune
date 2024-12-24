kitsune_add_library(
    TARGET KitsuneFoundation
    SOURCES
    "Common/Features.h"
    "Common/Macros.h"
    "Common/Predefined.h"
    "Common/Types.h"

    "Diagnostics/BadAllocException.h"
    "Diagnostics/IException.h"
    "Diagnostics/OutOfRangeException.h"

    "Memory/AddressOf.h"
    "Memory/Allocator.h"
    "Memory/BadWeakPtrException.h"
    "Memory/CMallocApi.h"
    "Memory/Deleter.h"
    "Memory/GlobalAllocator.h"
    "Memory/IMemoryApi.h"
    "Memory/Memory.cpp"
    "Memory/Memory.h"
    "Memory/ScopedPtr.h"
    "Memory/SharedPtr.h"

    "Templates/Exchange.h"
    "Templates/Forward.h"
    "Templates/Move.h"
    "Templates/Swap.h"

    "Threading/Interlocked.h"
    "Threading/ThreadSafety.h"
)

kitsune_add_platform_sources(
    TARGET KitsuneFoundation
    CLANG
    "Clang/ClangInterlocked.inl"

    MSVC
    "MSVC/MSVCInterlocked.inl"
)
