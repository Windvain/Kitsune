kitsune_add_library(
    TARGET KitsuneFoundation
    SOURCES
    "Algorithms/Copy.h"
    "Algorithms/Count.h"
    "Algorithms/Destroy.h"
    "Algorithms/Distance.h"
    "Algorithms/Equal.h"
    "Algorithms/Fill.h"
    "Algorithms/Find.h"
    "Algorithms/ForEach.h"
    "Algorithms/Move.h"
    "Algorithms/Replace.h"
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
    "Diagnostics/IException.cpp"
    "Diagnostics/IException.h"
    "Diagnostics/InvalidArgumentException.h"
    "Diagnostics/IsDebuggerAttached.h"
    "Diagnostics/MessageBox.h"
    "Diagnostics/OutOfRangeException.h"
    "Diagnostics/SourceLocation.h"
    "Diagnostics/StackFrame.h"
    "Diagnostics/StackTrace.h"
    "Diagnostics/StackTraceException.h"

    "Filesystem/Path.h"
    "Filesystem/PathChecks.h"
    "Filesystem/WindowsPath.cpp"
    "Filesystem/WindowsPath.h"
    "Filesystem/WindowsPathChecks.cpp"

    "Iterators/BackInsertIterator.h"
    "Iterators/Iterator.h"
    "Iterators/IteratorTraits.h"
    "Iterators/ReverseIterator.h"

    "Logging/AnsiColorSink.cpp"
    "Logging/AnsiColorSink.h"
    "Logging/ConsoleStream.cpp"
    "Logging/ConsoleStream.h"
    "Logging/GlobalLog.cpp"
    "Logging/GlobalLog.h"
    "Logging/ILogSink.h"
    "Logging/IStream.h"
    "Logging/LogMessage.h"
    "Logging/Logger.h"
    "Logging/StreamBuffer.h"
    "Logging/WriteStreamIterator.h"

    "Maths/VectorBase.h"
    "Maths/Vector2.h"

    "Memory/AddressOf.h"
    "Memory/Allocator.h"
    "Memory/BadAllocException.h"
    "Memory/BadWeakPtrException.h"
    "Memory/CMallocApi.cpp"
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
    "String/FormatArguments.h"
    "String/FormatException.h"
    "String/FormatScanner.cpp"
    "String/FormatScanner.h"
    "String/Formatter.h"
    "String/FromCodepoints.h"
    "String/String.h"
    "String/StringView.h"
    "String/ToCodepoints.h"
    "String/UnicodeConversion.h"
    "String/UnicodeException.h"
    "String/UnicodeTypes.h"
    "String/UnicodeValidity.h"

    "Templates/Exchange.h"
    "Templates/Forward.h"
    "Templates/IsAnyOf.h"
    "Templates/Move.h"

    "Threading/Interlocked.h"
    "Threading/LockGuard.h"
    "Threading/Mutex.h"
    "Threading/ThisThread.cpp"
    "Threading/ThisThread.h"
    "Threading/ThreadSafety.h"

    "Utilities/EnumFlags.h"
    "Utilities/Function.h"
    "Utilities/MemberFunction.h"
    "Utilities/NonCopyable.h"
    "Utilities/Process.cpp"
    "Utilities/Process.h"
)

kitsune_add_platform_sources(
    TARGET KitsuneFoundation
    CLANG
    "Diagnostics/ClangStackTrace.cpp"
    "Threading/ClangInterlocked.inl"

    "KitsuneFoundation.rc"

    MSVC
    "Diagnostics/MSVCStackTrace.cpp"
    "Threading/MSVCInterlocked.inl"

    "KitsuneFoundation.manifest"
    "KitsuneFoundation.natvis"

    WINDOWS
    "Diagnostics/WindowsDebuggerAttached.cpp"
    "Diagnostics/WindowsMessageBox.cpp"

    "Logging/WindowsConsoleStream.cpp"

    "Threading/WindowsMutex.cpp"

    "Utilities/WindowsProcess.cpp"
)

kitsune_add_platform_dependencies(
    TARGET KitsuneFoundation
    WINDOWS "comctl32.lib"

    MSVC  "DbgHelp.lib"
    CLANG "libbacktrace"
)
