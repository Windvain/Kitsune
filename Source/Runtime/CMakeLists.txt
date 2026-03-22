set(FILES
    "Application/Null/NullDisplayManager.h"
    "Application/Null/NullScreen.h"
    "Application/Null/NullWindow.h"

    "Application/Application.cpp"
    "Application/Application.h"
    "Application/CommandLineArguments.h"
    "Application/DisplayManager.cpp"
    "Application/DisplayManager.h"
    "Application/Screen.h"
    "Application/Window.h"

    "Foundation/Algorithms/Advance.h"
    "Foundation/Algorithms/Contains.h"
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

    "Foundation/Common/Macros.h"
    "Foundation/Common/Policies.cpp"
    "Foundation/Common/Predefined.h"
    "Foundation/Common/Types.h"

    "Foundation/Concepts/Character.h"
    "Foundation/Concepts/Comparable.h"
    "Foundation/Concepts/Container.h"
    "Foundation/Concepts/Invocable.h"
    "Foundation/Concepts/Swappable.h"

    "Foundation/Containers/Array.h"
    "Foundation/Containers/Pair.h"
    "Foundation/Containers/Set.h"

    "Foundation/Diagnostics/Assert.cpp"
    "Foundation/Diagnostics/Assert.h"
    "Foundation/Diagnostics/Backtrace.h"
    "Foundation/Diagnostics/BacktraceFrame.h"
    "Foundation/Diagnostics/Exception.cpp"
    "Foundation/Diagnostics/Exception.h"
    "Foundation/Diagnostics/InvalidArgumentException.h"
    "Foundation/Diagnostics/IsDebuggerAttached.h"
    "Foundation/Diagnostics/MessageBox.h"
    "Foundation/Diagnostics/OutOfRangeException.h"
    "Foundation/Diagnostics/SourceLocation.h"
    "Foundation/Diagnostics/SystemException.h"

    "Foundation/IO/ConsoleOutputStream.cpp"
    "Foundation/IO/ConsoleOutputStream.h"
    "Foundation/IO/Stream.h"
    "Foundation/IO/WriteStreamIterator.h"

    "Foundation/Iterators/BackInsertIterator.h"
    "Foundation/Iterators/Iterator.h"
    "Foundation/Iterators/IteratorTraits.h"
    "Foundation/Iterators/ReverseIterator.h"
    "Foundation/Iterators/ToAddress.h"

    "Foundation/Logging/ConsoleLogger.cpp"
    "Foundation/Logging/ConsoleLogger.h"
    "Foundation/Logging/GlobalLog.cpp"
    "Foundation/Logging/GlobalLog.h"
    "Foundation/Logging/Logger.h"
    "Foundation/Logging/LogPayload.h"

    "Foundation/Maths/Vector2.h"
    "Foundation/Maths/VectorBase.h"

    "Foundation/Memory/AddressOf.h"
    "Foundation/Memory/Allocator.h"
    "Foundation/Memory/BadAllocException.h"
    "Foundation/Memory/BadWeakPtrException.h"
    "Foundation/Memory/CMallocApi.cpp"
    "Foundation/Memory/CMallocApi.h"
    "Foundation/Memory/Deleter.h"
    "Foundation/Memory/GlobalAllocator.h"
    "Foundation/Memory/Memory.cpp"
    "Foundation/Memory/Memory.h"
    "Foundation/Memory/MemoryApi.h"
    "Foundation/Memory/ScopedPtr.h"
    "Foundation/Memory/SharedPtr.h"

    "Foundation/String/Decode.h"
    "Foundation/String/Encode.h"
    "Foundation/String/Encoding.h"
    "Foundation/String/Format.h"
    "Foundation/String/FormatArguments.h"
    "Foundation/String/FormatException.h"
    "Foundation/String/FormatScanner.h"
    "Foundation/String/Formatter.h"
    "Foundation/String/String.h"
    "Foundation/String/StringView.h"
    "Foundation/String/Transcode.h"
    "Foundation/String/TranscodePresets.h"
    "Foundation/String/Utf8Encoding.h"
    "Foundation/String/Utf16Encoding.h"
    "Foundation/String/Utf32Encoding.h"
    "Foundation/String/Valid.h"

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
    "Foundation/Utilities/Comparators.h"
    "Foundation/Utilities/CpuId.h"
    "Foundation/Utilities/EnumFlags.h"
    "Foundation/Utilities/Functor.h"
    "Foundation/Utilities/NonCopyable.h"
    "Foundation/Utilities/SystemInformation.cpp"
    "Foundation/Utilities/SystemInformation.h"

    "Launch/EngineLoop.cpp"
    "Launch/EngineLoop.h"
    "Launch/UniversalMain.cpp"
)

set(COMPILER_OPTIONS "")
set(COMPILER_DEFS "")
set(LINKED_LIBRARIES "")

# Can't make these any shorter, because.. CMake...
macro(append_if_backtrace_supported var value)
    # if ((config != Production) || (KITSUNE_OVERRIDE_BACKTRACE_SUPPORT))
    #     list(APPEND ${var} ${value})
    list(APPEND ${var} "$<$<OR:$<NOT:$<CONFIG:Production>>,$<BOOL:${KITSUNE_OVERRIDE_BACKTRACE_SUPPORT}>>:${value}>")
endmacro()

macro(append_if_debug_terminal_enabled var value)
    # if ((config != Production) || (KITSUNE_SHOW_TERMINAL))
    #     list(APPEND ${var} ${value})
    list(APPEND ${var} "$<$<OR:$<NOT:$<CONFIG:Production>>,$<BOOL:${KITSUNE_ENABLE_TERMINAL_FOR_DEBUGGING}>>:${value}>")
endmacro()

if ("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
    list(APPEND FILES
        "Application/Windows/WindowsDisplayManager.cpp"
        "Application/Windows/WindowsDisplayManager.h"
        "Application/Windows/WindowsScreen.cpp"
        "Application/Windows/WindowsScreen.h"
        "Application/Windows/WindowsWindow.cpp"
        "Application/Windows/WindowsWindow.h"

        "Foundation/Windows/WindowsConsoleStream.cpp"
        "Foundation/Windows/WindowsDebuggerAttached.cpp"
        "Foundation/Windows/WindowsMemory.cpp"
        "Foundation/Windows/WindowsMessageBox.cpp"
        "Foundation/Windows/WindowsMutex.cpp"

        "Launch/Windows/WindowsEngineLoop.cpp"
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
        "Foundation/Msvc/MsvcBacktrace.cpp"
        "Foundation/Msvc/MsvcInterlocked.inl"
        "Foundation/Msvc/MsvcSystemInformation.cpp"
    )

    append_if_backtrace_supported(LINKED_LIBRARIES "dbghelp.lib")
    list(APPEND LINKED_LIBRARIES
        # Used for querying WMI which is used in getting information about the system.
        "wbemuuid.lib"
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

        "/external:anglebrackets"     # Treat files included with angle brackets as external files.
        "/external:W0"                # Ignore external file warnings & errors.
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
        "Foundation/Clang/ClangBacktrace.cpp"
        "Foundation/Clang/ClangInterlocked.inl"

        # This file is for compilers OTHER than MSVC.
        "Foundation/Windows/WindowsSystemInformation.cpp"
    )

    find_library(LIBBACKTRACE_LIBRARY
        NAMES libbacktrace backtrace
        OPTIONAL
    )

    if (NOT LIBBACKTRACE_LIBRARY)
        message(STATUS "libbacktrace FOUND: FALSE")
        message(WARNING "Dependency libbacktrace required for obtaining backtraces was not found. Backtraces will be disabled.")
    else()
        message(STATUS "libbacktrace FOUND: TRUE")

        append_if_backtrace_supported(LINKED_LIBRARIES "${LIBBACKTRACE_LIBRARY}")
        append_if_backtrace_supported(COMPILER_DEFS "KITSUNE_SUPPORTS_BACKTRACES=1")
    endif()

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

append_if_debug_terminal_enabled(COMPILER_DEFS "KITSUNE_TERMINAL_ENABLED_FOR_DEBUGGING=1")
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

target_include_directories(KitsuneEngine PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(KitsuneEngine PRIVATE
    ${LINKED_LIBRARIES}
    "SHcore.lib"                    # DPI support, e.g. GetDpiForMonitor.
)
