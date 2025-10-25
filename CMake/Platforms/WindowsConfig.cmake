set(KITSUNE_GLOBAL_COMPILER_DEFINITIONS
    # Enable the usage of UTF-16 in the Win32 API.
    UNICODE
    _UNICODE

    # Disable MIN() and MAX() macros from <Windows.h>.
    NOMINMAX

    # Disable warnings from compilers about using XXX_s (safe functions).
    _CRT_SECURE_NO_WARNINGS
)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(KITSUNE_GLOBAL_COMPILER_FLAGS
        "-Wall"                         # Enable all warnings about construction.
        "-Wextra"                       # Enable warnings that aren't enabled by -Wall.
        "-Werror"                       # Turn all warnings into errors.

        "-pedantic"                     # Enable warnings about compiler extensions.
        "-Wshadow"                      # Warn when a local declaration shadows another.
    )

    set(KITSUNE_DEBUG_COMPILER_FLAGS
        "-g"              # Generate debug info.
        "-O0"             # Disable optimizations.
    )

    set(KITSUNE_OPTIMIZED_COMPILER_FLAGS
        "-g"              # Generate debug info.
        "-O2"             # Enable level 2 optimizations.
    )

    set(KITSUNE_PRODUCTION_COMPILER_FLAGS
        "-O3"             # Enable level 3 optimizations.
    )

elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    # MSVC corrupts UTF-8 strings if /utf-8 is not specified.
    # It first encodes the string as UTF-8, then encodes it again in Windows-1252.
    #
    # Thanks, Microsoft.
    # https://stackoverflow.com/questions/59046071/c-u8-literal-unexpected-encoding-on-windows
    set(KITSUNE_GLOBAL_COMPILER_FLAGS
        "/Zc:wchar_t"                 # Treat wchar_t as a native type.
        "/Zc:preprocessor"            # Use a C++ standard conforming preprocessor.

        "/utf-8"                      # Specifies the source and execution charset as UTF-8.
        "/MP"                         # Compile the engine source with multiple processes.

        "/W4"                         # Enable all reasonable warnings.
        "/WX"                         # Turn all warnings into errors.
        "/permissive"                 # Enable standards conformance.

        "/wd4244"                     # Disable "conversion from 'type1' to 'type2', possible loss of data".
    )

    set(KITSUNE_DEBUG_COMPILER_FLAGS
        "/Zi"             # Create a seperate PDB file with symbols.
        "/MDd"            # Link to the multithreaded debug runtime library.
        "/Od"             # Disable optimizations.
    )

    set(KITSUNE_OPTIMIZED_COMPILER_FLAGS
        "/Zi"             # Create a seperate PDB file with symbols.
        "/MD"             # Link to the multithreaded release runtime library.
        "/O2"             # Enable level 2 optimizations.
    )

    set(KITSUNE_PRODUCTION_COMPILER_FLAGS
        "/MD"             # Link to the multithreaded release runtime library.
        "/Ox"             # Optimize for speed.
    )

    set(KITSUNE_DEBUG_LINKER_FLAGS "/DEBUG")
    set(KITSUNE_OPTIMIZED_LINKER_FLAGS "/DEBUG")

    set(KITSUNE_PRODUCTION_LINKER_FLAGS
        "/INCREMENTAL:NO"       # Disable incremental linking.
        "/OPT:REF"              # Remove data which are never referenced.
    )
else()
    message(
        FATAL_ERROR
        "The compiler currently used is unsupported on Windows. Please switch compilers to continue the build process."
    )
endif()
