kitsune_add_library(
    TARGET KitsuneFoundation
    SOURCES
    "Algorithms/Internal/AlgoConcepts.h"
    "Algorithms/Copy.h"
    "Algorithms/Destroy.h"
    "Algorithms/Distance.h"
    "Algorithms/Equal.h"
    "Algorithms/Fill.h"
    "Algorithms/Find.h"
    "Algorithms/ForEach.h"
    "Algorithms/Move.h"
    "Algorithms/Reverse.h"
    "Algorithms/Swap.h"
    "Algorithms/Uninitialized.h"

    "Common/Features.h"
    "Common/Macros.h"
    "Common/Predefined.h"
    "Common/Types.h"

    "Concepts/Character.h"

    "Containers/Array.h"

    "Diagnostics/Assert.cpp"
    "Diagnostics/Assert.h"
    "Diagnostics/IException.h"
    "Diagnostics/OutOfRangeException.h"
    "Diagnostics/SourceLocation.h"

    "Iterators/Iterator.h"
    "Iterators/IteratorTraits.h"
    "Iterators/ReverseIterator.h"

    "Memory/AddressOf.h"
    "Memory/Allocator.h"
    "Memory/BadAllocException.h"
    "Memory/BadWeakPtrException.h"
    "Memory/CMallocApi.h"
    "Memory/Deleter.h"
    "Memory/GlobalAllocator.h"
    "Memory/IMemoryApi.h"
    "Memory/Memory.cpp"
    "Memory/Memory.h"
    "Memory/ScopedPtr.h"
    "Memory/SharedPtr.h"

    "String/CharTraits.h"
    "String/String.h"
    "String/StringView.h"

    "Templates/Exchange.h"
    "Templates/Forward.h"
    "Templates/IsAnyOf.h"
    "Templates/Move.h"

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
