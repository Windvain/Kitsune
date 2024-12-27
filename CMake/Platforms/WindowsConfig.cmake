list(APPEND KITSUNE_GLOBAL_COMMON_DEFINITIONS
    # Enable the usage of UTF-16 in the Win32 API.
    UNICODE
    _UNICODE

    # Disable MIN() and MAX() macros from <Windows.h>.
    NOMINMAX

    # Disable warnings from compilers about using XXX_s (safe functions).
    _CRT_SECURE_NO_WARNINGS
)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    list(APPEND KITSUNE_GLOBAL_COMMON_COMPILE_FLAGS
        "-Wall"                         # Enable all warnings about construction.
        "-Wextra"                       # Enable warnings that aren't enabled by -Wall.
        "-Werror"                       # Turn all warnings into errors.

        "-pedantic"                     # Enable warnings about compiler extensions.
        "-Wshadow"                      # Warn when a local declaration shadows another.

        "-fno-ms-compatibility"         # Disable MSVC compatibility.
        "-fms-compatibility-version=0"  # Remove definition of _MSC_VER.
    )

    list(APPEND KITSUNE_GLOBAL_DEBUG_COMPILE_FLAGS
        "-g"              # Generate debug info.
        "-O0"             # Disable optimizations.
    )

    list(APPEND KITSUNE_GLOBAL_RELWITHDBGINFO_COMPILE_FLAGS
        "-g"              # Generate debug info.
        "-O2"             # Enable level 2 optimizations.
    )

    list(APPEND KITSUNE_GLOBAL_RELEASE_COMPILE_FLAGS
        "-O3"             # Enable level 3 optimizations.
    )

elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    list(APPEND KITSUNE_GLOBAL_COMMON_COMPILE_FLAGS
        "/Zc:wchar_t"                 # Treat wchar_t as a native type.
        "/utf-8"                      # Specifies the source and execution charset as UTF-8.
        "/MP"                         # Compile the engine source with multiple processes.

        "/W4"                         # Enable all reasonable warnings.
        "/WX"                         # Turn all warnings into errors.
        "/permissive"                 # Enable standards conformance.
    )

    list(APPEND KITSUNE_GLOBAL_DEBUG_COMPILE_FLAGS
        "/Zi"             # Create a seperate PDB file with symbols.
        "/MDd"            # Link to the multithreaded debug runtime library.
        "/Od"             # Disable optimizations.
    )

    list(APPEND KITSUNE_GLOBAL_RELWITHDBGINFO_COMPILE_FLAGS
        "/Zi"             # Create a seperate PDB file with symbols.
        "/MDd"            # Link to the multithreaded debug runtime library.
        "/O2"             # Enable level 2 optimizations.
    )

    list(APPEND KITSUNE_GLOBAL_RELEASE_COMPILE_FLAGS
        "/MD"             # Link to the multithreaded release runtime library.
        "/Ox"             # Optimize for speed.
    )

    # Linker flags -----------------------------------------------------------------
    list(APPEND KITSUNE_GLOBAL_DEBUG_LINKER_FLAGS "/DEBUG")
    list(APPEND KITSUNE_GLOBAL_RELWITHDBGINFO_LINKER_FLAGS "/DEBUG")
else()
    message(FATAL_ERROR "Compiler unsupported on the current platform.")
endif()
