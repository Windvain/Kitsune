set(FILES
    "Application/CommandLineArguments.h"
    "Application/IApplication.cpp"
    "Application/IApplication.h"
    "Application/IDisplayManager.cpp"
    "Application/IDisplayManager.h"
    "Application/IScreen.h"
    "Application/IWindow.h"
    "Application/SystemInformation.h"

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

set(COMPILER_OPTIONS "")
set(COMPILER_DEFS "")
set(LINKED_LIBRARIES "")

if ("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
    list(APPEND FILES
        "Application/Windows/WindowsDisplayManager.cpp"
        "Application/Windows/WindowsDisplayManager.h"
        "Application/Windows/WindowsScreen.cpp"
        "Application/Windows/WindowsScreen.h"
        "Application/Windows/WindowsSystemInformation.cpp"
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

    list(APPEND COMPILER_DEFS
        # Enable the usage of "wide" functions in the Win32 API.
        "UNICODE=1"
        "_UNICODE=1"

        # Undefines MIN() and MAX() macros from the <Windows.h> header.
        "NOMINMAX=1"

        # Disable warnings from MSVC about using unsafe C functions.
        "_CRT_SECURE_NO_WARNINGS=1"
    )
endif()

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    list(APPEND FILES
        "Foundation/Msvc/MsvcInterlocked.inl"
        "Foundation/Msvc/MsvcStackFrame.h"
        "Foundation/Msvc/MsvcStackTrace.cpp"

        "KitsuneEngine.natvis"
    )

    list(APPEND LINKED_LIBRARIES
        # Used for querying WMI which is used in getting information about the system.
        "wbemuuid.lib"

        # Used in getting the backtrace of our program.
        "dbghelp.lib"
    )

    append_if_backtrace_supported(COMPILER_DEFS "KITSUNE_SUPPORTS_BACKTRACES=1")
    list(APPEND COMPILER_OPTIONS
        "/Zc:wchar_t"                 # Treat wchar_t as a native type.
        "/Zc:preprocessor"            # Use a C++ standard conforming preprocessor.

        "/utf-8"                      # Specifies the source and execution charset as UTF-8.
        "/MP"                         # Compile the engine source with multiple processes.

        "/W4"                         # Enable all reasonable warnings.
        "/WX"                         # Turn all warnings into errors.
        "/permissive"                 # Enable standards conformance.

        "/wd4244"                     # Disable "conversion from "type1" to "type2"  possible loss of data".
    )

    list(APPEND COMPILER_OPTIONS
        # /Zi: Create a seperate PDB file with symbols.
        # /MDd: Link to the debug runtime library.
        # /Od: Disable optimizations.
        "$<$<CONFIG:Debug>:/Zi;/MDd;/Od>"

        # /Zi: Create a seperate PDB file with symbols.
        # /MD: Link to the release runtime library.
        # /O2: Enable level 2 optimizations.
        "$<$<CONFIG:Optimized>:/Zi;/MD;/O2>"

        # /MD: Link to the release runtime library.
        # /Ox: Optimize for speed.
        "$<$<CONFIG:Production>:/MD;/Ox>"
    )
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    list(APPEND FILES
        "Foundation/Clang/ClangInterlocked.inl"
        "Foundation/Clang/ClangStackTrace.cpp"

        "Foundation/GeneralPlatform/GeneralStackFrame.h"
    )

    find_library(LIBBACKTRACE_LIBRARY
        NAMES libbacktrace backtrace
        OPTIONAL
    )

    list(APPEND LINKED_LIBRARIES "${LIBBACKTRACE_LIBRARY}")
    list(APPEND COMPILER_OPTIONS
        "-Wall"                          # Enable all warnings about construction.
        "-Wextra"                        # Enable warnings that aren"t enabled by -Wall.
        "-Werror"                        # Turn all warnings into errors.

        "-pedantic"                      # Enable warnings about compiler extensions.
        "-Wshadow"                       # Warn when a local declaration shadows another.
    )

    list(APPEND COMPILER_OPTIONS
        # /-g: Generate debug info.
        # /O0: Disable optimizations.
        "$<$<CONFIG:Debug>:-g;-O0>"

        # -g: Generate debug info.
        # -O2: Enable level 2 optimizations.
        "$<$<CONFIG:Optimized>:-g;-O2>"

        # /Ox: Optimize level 3 optimizations.
        "$<$<CONFIG:Production>:-O3>"
    )
else()
    message(FATAL_ERROR
        "${CMAKE_CXX_COMPILER_ID} is unsupported for compiling Kitsune. Please use one of the listed supported compilers."
    )
endif()

list(APPEND COMPILER_DEFS
    "KITSUNE_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}"
    "KITSUNE_VERSION_MINOR=${PROJECT_VERSION_MINOR}"
    "KITSUNE_VERSION_PATCH=${PROJECT_VERSION_PATCH}"

    "KITSUNE_BUILD_$<UPPER_CASE:$<CONFIG>>=1"
)

add_library(KitsuneEngine STATIC ${FILES})
set_target_properties(KitsuneEngine PROPERTIES FOLDER "Engine")

target_compile_options(KitsuneEngine PRIVATE ${COMPILER_OPTIONS})
target_compile_definitions(KitsuneEngine PRIVATE ${COMPILER_DEFS})

target_link_libraries(KitsuneEngine PRIVATE ${LINKED_LIBRARIES})
target_include_directories(KitsuneEngine PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
