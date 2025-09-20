project(KitsuneEngine)
add_library(
    KitsuneEngine
    STATIC

    "Application/Application.cpp"
    "Application/Application.h"

    "ApplicationCore/Null/NullMonitor.h"
    "ApplicationCore/Null/NullWindow.cpp"
    "ApplicationCore/Null/NullWindow.h"

    "ApplicationCore/CommandLineArguments.h"
    "ApplicationCore/Environment.h"
    "ApplicationCore/IMonitor.h"
    "ApplicationCore/IWindow.h"
    "ApplicationCore/MonitorException.h"
    "ApplicationCore/VideoMode.h"
    "ApplicationCore/WindowException.h"

    "Foundation/Algorithms/Copy.h"
    "Foundation/Algorithms/Count.h"
    "Foundation/Algorithms/Destroy.h"
    "Foundation/Algorithms/Distance.h"
    "Foundation/Algorithms/Equal.h"
    "Foundation/Algorithms/Fill.h"
    "Foundation/Algorithms/Find.h"
    "Foundation/Algorithms/ForEach.h"
    "Foundation/Algorithms/Move.h"
    "Foundation/Algorithms/Replace.h"
    "Foundation/Algorithms/Reverse.h"
    "Foundation/Algorithms/Swap.h"
    "Foundation/Algorithms/Uninitialized.h"

    "Foundation/Common/Features.h"
    "Foundation/Common/Macros.h"
    "Foundation/Common/Predefined.h"
    "Foundation/Common/Types.h"

    "Foundation/Concepts/Character.h"

    "Foundation/Containers/Array.h"

    "Foundation/Diagnostics/Assert.cpp"
    "Foundation/Diagnostics/Assert.h"
    "Foundation/Diagnostics/IException.cpp"
    "Foundation/Diagnostics/IException.h"
    "Foundation/Diagnostics/InvalidArgumentException.h"
    "Foundation/Diagnostics/IsDebuggerAttached.h"
    "Foundation/Diagnostics/MessageBox.h"
    "Foundation/Diagnostics/OutOfRangeException.h"
    "Foundation/Diagnostics/SourceLocation.h"
    "Foundation/Diagnostics/StackFrame.h"
    "Foundation/Diagnostics/StackTrace.h"
    "Foundation/Diagnostics/StackTraceException.h"

    "Foundation/Filesystem/Path.h"
    "Foundation/Filesystem/PathChecks.h"
    "Foundation/Filesystem/WindowsPath.cpp"
    "Foundation/Filesystem/WindowsPath.h"
    "Foundation/Filesystem/WindowsPathChecks.cpp"

    "Foundation/Iterators/BackInsertIterator.h"
    "Foundation/Iterators/Iterator.h"
    "Foundation/Iterators/IteratorTraits.h"
    "Foundation/Iterators/ReverseIterator.h"

    "Foundation/Logging/ConsoleInputStream.cpp"
    "Foundation/Logging/ConsoleInputStream.h"
    "Foundation/Logging/ConsoleLogger.cpp"
    "Foundation/Logging/ConsoleLogger.h"
    "Foundation/Logging/ConsoleOutputStream.cpp"
    "Foundation/Logging/ConsoleOutputStream.h"
    "Foundation/Logging/ILogger.h"
    "Foundation/Logging/IStream.h"
    "Foundation/Logging/WriteStreamIterator.h"

    "Foundation/Maths/VectorBase.h"
    "Foundation/Maths/Vector2.h"

    "Foundation/Memory/AddressOf.h"
    "Foundation/Memory/Allocator.h"
    "Foundation/Memory/BadAllocException.h"
    "Foundation/Memory/BadWeakPtrException.h"
    "Foundation/Memory/CMallocApi.cpp"
    "Foundation/Memory/CMallocApi.h"
    "Foundation/Memory/Deleter.h"
    "Foundation/Memory/GlobalAllocator.h"
    "Foundation/Memory/IMemoryApi.h"
    "Foundation/Memory/Memory.cpp"
    "Foundation/Memory/Memory.h"
    "Foundation/Memory/ScopedPtr.h"
    "Foundation/Memory/SharedPtr.h"

    "Foundation/String/CharTraits.h"
    "Foundation/String/Format.h"
    "Foundation/String/FormatException.h"
    "Foundation/String/Formatter.h"
    "Foundation/String/FromCodepoints.h"
    "Foundation/String/String.h"
    "Foundation/String/StringView.h"
    "Foundation/String/ToCodepoints.h"
    "Foundation/String/UnicodeConversion.h"
    "Foundation/String/UnicodeException.h"
    "Foundation/String/UnicodeTypes.h"
    "Foundation/String/UnicodeValidity.h"

    "Foundation/Templates/Exchange.h"
    "Foundation/Templates/Forward.h"
    "Foundation/Templates/IsAnyOf.h"
    "Foundation/Templates/Move.h"

    "Foundation/Threading/Interlocked.h"
    "Foundation/Threading/LockGuard.h"
    "Foundation/Threading/Mutex.h"
    "Foundation/Threading/ThreadSafety.h"

    "Foundation/Utilities/EnumFlags.h"
    "Foundation/Utilities/Function.h"
    "Foundation/Utilities/MemberFunction.h"
    "Foundation/Utilities/NonCopyable.h"
    "Foundation/Utilities/Process.cpp"
    "Foundation/Utilities/Process.h"

    "Launch/EngineLoop.cpp"
    "Launch/EngineLoop.h"
    "Launch/EngineMain.cpp"
)

if (WIN32)
    target_sources(KitsuneEngine PRIVATE
        "Application/Windows/WindowsApplication.cpp"

        "ApplicationCore/Windows/WindowsEnvironment.cpp"
        "ApplicationCore/Windows/WindowsMonitor.cpp"
        "ApplicationCore/Windows/WindowsMonitor.h"
        "ApplicationCore/Windows/WindowsWindow.cpp"
        "ApplicationCore/Windows/WindowsWindow.h"

        "Foundation/Diagnostics/WindowsDebuggerAttached.cpp"
        "Foundation/Diagnostics/WindowsMessageBox.cpp"

        "Foundation/Logging/WindowsConsoleInputStream.cpp"
        "Foundation/Logging/WindowsConsoleOutputStream.cpp"

        "Foundation/Threading/WindowsMutex.cpp"

        "Foundation/Utilities/WindowsProcess.cpp"

        "Launch/Windows/WindowsEntry.cpp"
    )

    target_link_libraries(KitsuneEngine PRIVATE
        "comctl32.lib"
    )
endif()

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    target_sources(KitsuneEngine PRIVATE
        "Foundation/Diagnostics/ClangStackTrace.cpp"
        "Foundation/Diagnostics/GeneralStackFrame.h"

        "Foundation/Threading/ClangInterlocked.inl"
    )

    # Resources cannot be stored in a static library, and therefore
    # have to be compiled into the executable, not the library.
    #
    # Might be removed in the future, forcing .rc files on our clients is just stupid.
    target_sources(KitsuneEngine PUBLIC "KitsuneEngine.rc")

    target_link_libraries(KitsuneEngine PRIVATE
        "libbacktrace"
    )
elseif (MSVC)
    target_sources(KitsuneEngine PRIVATE
        "Foundation/Diagnostics/MsvcStackFrame.h"
        "Foundation/Diagnostics/MsvcStackTrace.cpp"

        "Foundation/Threading/MSVCInterlocked.inl"

        "KitsuneEngine.natvis"
    )

    # Resources cannot be stored in a static library, and therefore
    # have to be compiled into the executable, not the library.
    target_sources(KitsuneEngine PUBLIC "KitsuneEngine.manifest")

    target_link_libraries(KitsuneEngine PRIVATE
        "DbgHelp.lib"
    )
endif()

target_compile_options(KitsuneEngine PRIVATE ${KITSUNE_GLOBAL_COMPILER_FLAGS})
target_compile_definitions(KitsuneEngine PRIVATE ${KITSUNE_GLOBAL_COMPILER_DEFINITIONS})

target_link_options(KitsuneEngine PRIVATE ${KITSUNE_GLOBAL_LINKER_FLAGS})
target_include_directories(KitsuneEngine PUBLIC ${KITSUNE_GLOBAL_INCLUDE_DIRECTORIES})
