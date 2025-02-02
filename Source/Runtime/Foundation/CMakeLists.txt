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
    "Diagnostics/InvalidArgumentException.h"
    "Diagnostics/IsDebuggerAttached.h"
    "Diagnostics/MessageBox.h"
    "Diagnostics/OutOfRangeException.h"
    "Diagnostics/SourceLocation.h"

    "Iterators/Iterator.h"
    "Iterators/IteratorTraits.h"
    "Iterators/ReverseIterator.h"

    "Logging/ConsoleLoggerSink.cpp"
    "Logging/ConsoleLoggerSink.h"
    "Logging/ILoggerSink.h"
    "Logging/Logger.cpp"
    "Logging/Logger.h"
    "Logging/Logging.cpp"
    "Logging/Logging.h"
    "Logging/LogMessage.h"
    "Logging/LogSeverity.h"

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
    "String/Format.h"
    "String/FormatException.h"
    "String/Formatter.h"
    "String/InvalidUnicodeException.h"
    "String/String.h"
    "String/StringView.h"
    "String/ToString.h"

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
    "Threading/ClangInterlocked.inl"
    "KitsuneFoundation.rc"

    MSVC
    "Threading/MSVCInterlocked.inl"
    "KitsuneFoundation.manifest"

    WINDOWS
    "Diagnostics/WindowsDebuggerAttached.cpp"
    "Diagnostics/WindowsMessageBox.cpp"
)

kitsune_add_platform_dependencies(
    TARGET KitsuneFoundation
    WINDOWS "comctl32.lib"
)
