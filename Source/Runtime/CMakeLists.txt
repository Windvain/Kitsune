add_library(KitsuneRuntime SHARED
    "Core/Application.cpp"
    "Core/Application.h"
    "Core/CommandLineArguments.h"

    "Display/DisplayManager.cpp"
    "Display/DisplayManager.h"
    "Display/WindowManager.cpp"
    "Display/WindowManager.h"

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

    "Foundation/Concepts/AnyOf.h"
    "Foundation/Concepts/Character.h"
    "Foundation/Concepts/Comparable.h"
    "Foundation/Concepts/Container.h"
    "Foundation/Concepts/Invocable.h"
    "Foundation/Concepts/Swappable.h"

    "Foundation/Containers/Array.h"
    "Foundation/Containers/Pair.h"
    "Foundation/Containers/RBTree.h"
    "Foundation/Containers/TreeMap.h"
    "Foundation/Containers/TreeSet.h"

    "Foundation/Diagnostics/Assert.cpp"
    "Foundation/Diagnostics/Assert.h"
    "Foundation/Diagnostics/Backtrace.h"
    "Foundation/Diagnostics/BacktraceFrame.h"
    "Foundation/Diagnostics/Exception.cpp"
    "Foundation/Diagnostics/Exception.h"
    "Foundation/Diagnostics/InvalidArgumentException.h"
    "Foundation/Diagnostics/LogicException.h"
    "Foundation/Diagnostics/MessageBox.h"
    "Foundation/Diagnostics/OutOfRangeException.h"
    "Foundation/Diagnostics/SourceLocation.h"
    "Foundation/Diagnostics/SystemException.h"

    "Foundation/Filesystem/CurrentDirectory.h"
    "Foundation/Filesystem/ExecutablePath.h"
    "Foundation/Filesystem/Path.h"
    "Foundation/Filesystem/PathView.h"
    "Foundation/Filesystem/ToAbsolute.h"

    "Foundation/Iterators/BackInsertIterator.h"
    "Foundation/Iterators/Iterator.h"
    "Foundation/Iterators/IteratorTraits.h"
    "Foundation/Iterators/ReverseIterator.h"
    "Foundation/Iterators/ToAddress.h"

    "Foundation/Logging/ConsoleLogSink.cpp"
    "Foundation/Logging/ConsoleLogSink.h"
    "Foundation/Logging/Logger.cpp"
    "Foundation/Logging/Logger.h"
    "Foundation/Logging/LogPayload.h"
    "Foundation/Logging/LogSink.h"

    "Foundation/Maths/Clamp.h"
    "Foundation/Maths/Maximum.h"
    "Foundation/Maths/Minimum.h"
    "Foundation/Maths/Rect2.h"
    "Foundation/Maths/Vector.h"
    "Foundation/Maths/Vector2.h"
    "Foundation/Maths/Vector3.h"
    "Foundation/Maths/Vector4.h"

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

    "Foundation/Streams/ConsoleWriter.h"
    "Foundation/Streams/FileStream.h"
    "Foundation/Streams/Stream.h"
    "Foundation/Streams/WriterIterator.h"

    "Foundation/String/Decode.h"
    "Foundation/String/Encode.h"
    "Foundation/String/Format.h"
    "Foundation/String/FormatArguments.h"
    "Foundation/String/FormatException.h"
    "Foundation/String/FormatScanner.h"
    "Foundation/String/Formatter.h"
    "Foundation/String/String.h"
    "Foundation/String/StringView.h"
    "Foundation/String/TextEncoding.h"
    "Foundation/String/Transcode.h"
    "Foundation/String/TranscodePresets.h"
    "Foundation/String/UTF16Encoding.h"
    "Foundation/String/UTF32Encoding.h"
    "Foundation/String/UTF8Encoding.h"
    "Foundation/String/Valid.h"

    "Foundation/Templates/Exchange.h"
    "Foundation/Templates/Forward.h"
    "Foundation/Templates/Move.h"
    "Foundation/Templates/Swap.h"

    "Foundation/Threading/Atomic.h"
    "Foundation/Threading/LockGuard.h"
    "Foundation/Threading/MemoryOrder.h"
    "Foundation/Threading/Mutex.h"
    "Foundation/Threading/ThreadSafety.h"

    "Foundation/Utilities/BadCallException.h"
    "Foundation/Utilities/Comparators.h"
    "Foundation/Utilities/EnumFlags.h"
    "Foundation/Utilities/Functor.h"
    "Foundation/Utilities/NonCopyable.h"
    "Foundation/Utilities/SystemInformation.cpp"
    "Foundation/Utilities/SystemInformation.h"
)

add_library(Kitsune::Runtime ALIAS KitsuneRuntime)

# CPUID.h should only be included on x86_64 targets. Contains the use of the CPUID
# instruction, which is specific to the x86 architecture.
if (${KITSUNE_TARGET_ARCHITECTURE} STREQUAL "x86_64")
    target_sources(KitsuneRuntime PRIVATE "Foundation/Utilities/CPUID.h")
endif()

if (KITSUNE_ENABLE_VULKAN_BACKEND)
    find_package(Vulkan REQUIRED)

    target_compile_definitions(KitsuneRuntime PRIVATE "KITSUNE_ENABLE_VULKAN_BACKEND=1")
    target_link_libraries(KitsuneRuntime PRIVATE Vulkan::Vulkan)

    target_sources(KitsuneRuntime PRIVATE)
endif()

set_target_properties(KitsuneRuntime PROPERTIES FOLDER "Engine")
target_include_directories(KitsuneRuntime PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

if (KITSUNE_ENABLE_SIMD_OPTIMIZATIONS)
    target_compile_definitions(KitsuneRuntime PUBLIC "KITSUNE_ENABLE_SIMD_OPTIMIZATIONS=1")
endif()

target_compile_definitions(KitsuneRuntime
    PUBLIC
    "KITSUNE_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}"
    "KITSUNE_VERSION_MINOR=${PROJECT_VERSION_MINOR}"
    "KITSUNE_VERSION_PATCH=${PROJECT_VERSION_PATCH}"

    "KITSUNE_BUILD_$<UPPER_CASE:$<CONFIG>>=1"

    PRIVATE
    "KITSUNE_EXPORTS=1"
)

if ("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
    target_sources(KitsuneRuntime PRIVATE
        "Display/Windows/WindowsDisplayManager.cpp"
        "Display/Windows/WindowsDisplayManager.h"
        "Display/Windows/WindowsWindowManager.cpp"
        "Display/Windows/WindowsWindowManager.h"

        "Foundation/Windows/WindowsConsoleWriter.cpp"
        "Foundation/Windows/WindowsCurrentDirectory.cpp"
        "Foundation/Windows/WindowsExecutablePath.cpp"
        "Foundation/Windows/WindowsFileStream.cpp"
        "Foundation/Windows/WindowsMemory.cpp"
        "Foundation/Windows/WindowsMessageBox.cpp"
        "Foundation/Windows/WindowsMutex.cpp"
        "Foundation/Windows/WindowsPath.cpp"
        "Foundation/Windows/WindowsPathView.cpp"
        "Foundation/Windows/WindowsToAbsolute.cpp"
    )

    # Include WindowsSystemInformation.cpp for compilers other than MSVC, b.c. the
    # implementation of SystemInformation uses the WMI API, which MinGW-Clang does not
    # support as of writing this. (May 2026, Clang 20.1.8)
    if (NOT "${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
        target_sources(KitsuneRuntime PRIVATE "Foundation/Windows/WindowsSystemInformation.cpp")
    endif()

    # Simulate the symbol visibility of DLLs. (Hidden until declared exported)
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        target_compile_options(KitsuneRuntime PRIVATE
            "-fvisibility=hidden"
            "-fvisibility-inlines-hidden"
        )
    endif()


    target_link_libraries(KitsuneRuntime PRIVATE
        "SHcore.lib"        # Used for DPI support, for example GetDpiForMonitor().
    )

    target_compile_definitions(KitsuneRuntime PRIVATE
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
    target_sources(KitsuneRuntime PRIVATE
        "Foundation/MSVC/MSVCBacktrace.cpp"
        "Foundation/MSVC/MSVCInterlocked.inl"
        "Foundation/MSVC/MSVCSystemInformation.cpp"
    )

    target_compile_definitions(KitsuneRuntime PRIVATE "KITSUNE_ENABLE_BACKTRACES=1")
    target_link_libraries(KitsuneRuntime PRIVATE
        "wbemuuid.lib"      # Used for querying WMI which is used in getting information about the system.
        "dbghelp.lib"       # Used in the backtrace code for retrieving stack traces.
    )

    target_compile_options(KitsuneRuntime PRIVATE
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

    target_compile_options(KitsuneRuntime PRIVATE
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

    # Intrinsics are used in header files, which means that all executables using
    # this library will also require this flag to compile successfully.
    if (KITSUNE_ENABLE_SIMD_OPTIMIZATIONS AND ${KITSUNE_TARGET_ARCHITECTURE} STREQUAL "x86_64")
        target_compile_options(KitsuneRuntime PUBLIC "/arch:AVX2")
    endif()
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    target_sources(KitsuneRuntime PRIVATE
        "Foundation/Clang/ClangAtomicMacros.h"
        "Foundation/Clang/ClangBacktrace.cpp"
    )

    find_library(LIBBACKTRACE_LIBRARY
        NAMES libbacktrace backtrace
        OPTIONAL
    )

    if (LIBBACKTRACE_LIBRARY)
        message(STATUS "libbacktrace FOUND: TRUE")

        target_link_libraries(KitsuneRuntime PRIVATE ${LIBBACKTRACE_LIBRARY})
        target_compile_definitions(KitsuneRuntime PRIVATE "KITSUNE_ENABLE_BACKTRACES=1")
    else()
        message(STATUS "libbacktrace FOUND: FALSE")
        message(WARNING "Dependency libbacktrace required for obtaining backtraces was not found. Backtraces will be disabled.")
    endif()

    target_compile_options(KitsuneRuntime PRIVATE
        "-Wall"                          # Enable all warnings about construction.
        "-Wextra"                        # Enable warnings that aren"t enabled by -Wall.
        "-Werror"                        # Turn all warnings into errors.

        "-pedantic"                      # Enable warnings about compiler extensions.
        "-Wshadow"                       # Warn when a local declaration shadows another.
    )

    target_compile_options(KitsuneRuntime PRIVATE
        # /-g: Generate debug info.
        # /O0: Disable optimizations.
        "$<$<CONFIG:Debug>:-g;-O0>"

        # -g: Generate debug info.
        # -O2: Enable level 2 optimizations.
        "$<$<CONFIG:Optimized>:-g;-O2>"

        # /Ox: Optimize level 3 optimizations.
        "$<$<CONFIG:Production>:-O3>"
    )

    # Intrinsics are used in header files, which means that all executables using
    # this library will also require this flag to compile successfully.
    if (KITSUNE_ENABLE_SIMD_OPTIMIZATIONS AND ${KITSUNE_TARGET_ARCHITECTURE} STREQUAL "x86_64")
        target_compile_options(KitsuneRuntime PUBLIC "-mavx2")
    endif()
else()
    message(FATAL_ERROR
        "${CMAKE_CXX_COMPILER_ID} is unsupported for compiling Kitsune. Please use one of the listed supported compilers."
    )
endif()
