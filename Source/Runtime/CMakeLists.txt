project(KitsuneEngine)
add_library(
    KitsuneEngine
    STATIC

    "Application/CommandLineArguments.h"
    "Application/IApplication.cpp"
    "Application/IApplication.h"
    "Application/IDisplayManager.cpp"
    "Application/IDisplayManager.h"
    "Application/IScreen.h"
    "Application/IWindow.h"

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
    "Foundation/Concepts/Comparable.h"
    "Foundation/Concepts/Container.h"
    "Foundation/Concepts/Invokable.h"
    "Foundation/Concepts/Swappable.h"

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
    "Foundation/Diagnostics/SystemException.h"

    "Foundation/Filesystem/Path.h"
    "Foundation/Filesystem/PathChecks.h"
    "Foundation/Filesystem/WindowsPath.cpp"
    "Foundation/Filesystem/WindowsPath.h"

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
    "Foundation/Logging/GlobalLog.cpp"
    "Foundation/Logging/GlobalLog.h"
    "Foundation/Logging/ILogger.h"
    "Foundation/Logging/IStream.h"
    "Foundation/Logging/LogPayload.h"
    "Foundation/Logging/WriteStreamIterator.h"

    "Foundation/Maths/Vector2.h"
    "Foundation/Maths/VectorBase.h"

    "Foundation/Memory/AddressOf.h"
    "Foundation/Memory/Allocator.h"
    "Foundation/Memory/BackupMemoryAllocator.cpp"
    "Foundation/Memory/BackupMemoryAllocator.h"
    "Foundation/Memory/BadAllocException.h"
    "Foundation/Memory/BadWeakPtrException.h"
    "Foundation/Memory/CMallocApi.cpp"
    "Foundation/Memory/CMallocApi.h"
    "Foundation/Memory/Deleter.h"
    "Foundation/Memory/GlobalAllocator.h"
    "Foundation/Memory/IMemoryApi.h"
    "Foundation/Memory/Memory.cpp"
    "Foundation/Memory/Memory.h"
    "Foundation/Memory/MemoryProtection.h"
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
    "Foundation/Templates/Swap.h"

    "Foundation/Threading/Interlocked.h"
    "Foundation/Threading/LockGuard.h"
    "Foundation/Threading/Mutex.h"
    "Foundation/Threading/ThreadSafety.h"

    "Foundation/Utilities/BadCallException.h"
    "Foundation/Utilities/EnumFlags.h"
    "Foundation/Utilities/Function.h"
    "Foundation/Utilities/MemberFunction.h"
    "Foundation/Utilities/NonCopyable.h"

    "Launch/DefaultEngineLoop.cpp"
    "Launch/DefaultEngineLoop.h"
    "Launch/EngineMain.cpp"
    "Launch/IEngineLoop.h"
)

if (WIN32)
    target_sources(KitsuneEngine PRIVATE
        "Application/Windows/WindowsDisplayManager.cpp"
        "Application/Windows/WindowsDisplayManager.h"
        "Application/Windows/WindowsScreen.cpp"
        "Application/Windows/WindowsScreen.h"
        "Application/Windows/WindowsWindow.cpp"
        "Application/Windows/WindowsWindow.h"

        "Foundation/Windows/WindowsConsoleInputStream.cpp"
        "Foundation/Windows/WindowsConsoleOutputStream.cpp"
        "Foundation/Windows/WindowsDebuggerAttached.cpp"
        "Foundation/Windows/WindowsMemory.cpp"
        "Foundation/Windows/WindowsMessageBox.cpp"
        "Foundation/Windows/WindowsMutex.cpp"
        "Foundation/Windows/WindowsPathChecks.cpp"

        "Launch/Windows/WindowsDefaultEngineLoop.cpp"
        "Launch/Windows/WindowsEntry.cpp"
    )
endif()

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    target_sources(KitsuneEngine PRIVATE
        "Foundation/Clang/ClangInterlocked.inl"
        "Foundation/Clang/ClangStackTrace.cpp"

        "Foundation/GeneralPlatform/GeneralStackFrame.h"
    )

    target_sources(KitsuneEngine PUBLIC
        "KitsuneEngine.rc"
    )

    target_link_libraries(KitsuneEngine PRIVATE
        "libbacktrace"
    )
elseif (MSVC)
    target_sources(KitsuneEngine PRIVATE
        "Foundation/Msvc/MsvcInterlocked.inl"
        "Foundation/Msvc/MsvcStackFrame.h"
        "Foundation/Msvc/MsvcStackTrace.cpp"

        "KitsuneEngine.natvis"
    )

    target_sources(KitsuneEngine PUBLIC
        "KitsuneEngine.manifest"
    )

    target_link_libraries(KitsuneEngine PRIVATE
        "DbgHelp.lib"
    )
endif()

set_target_properties(KitsuneEngine PROPERTIES FOLDER "Engine")

# Temporary.
target_compile_definitions(KitsuneEngine PRIVATE "KITSUNE_VERSION_MAJOR=0")
target_compile_definitions(KitsuneEngine PRIVATE "KITSUNE_VERSION_MINOR=0")
target_compile_definitions(KitsuneEngine PRIVATE "KITSUNE_VERSION_PATCH=1")

target_compile_options(KitsuneEngine PRIVATE ${KITSUNE_GLOBAL_COMPILER_FLAGS})
target_compile_definitions(KitsuneEngine PRIVATE ${KITSUNE_GLOBAL_COMPILER_DEFINITIONS})

target_link_options(KitsuneEngine PRIVATE ${KITSUNE_GLOBAL_LINKER_FLAGS})
target_include_directories(KitsuneEngine PUBLIC ${KITSUNE_GLOBAL_INCLUDE_DIRECTORIES})
