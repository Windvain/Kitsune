# When targeting Windows platforms, there are two identifiers used for x86_64 targets.
# A GNU ABI target will define "x86_64", while an MSVC ABI will define "x64".
if ("x64" IN_LIST CMAKE_CXX_COMPILER_ARCHITECTURE_ID OR "x86_64" IN_LIST CMAKE_CXX_COMPILER_ARCHITECTURE_ID)
    set(KITSUNE_TARGET_ARCHITECTURE "x86_64")
endif()

function(kitsune_set_base_settings name)
    if ("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
        target_compile_definitions(${name} PRIVATE
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
        target_compile_options(${name} PRIVATE
            "/Zc:wchar_t"                 # Treat wchar_t as a native type.
            "/Zc:preprocessor"            # Use a C++ standard conforming preprocessor.

            "/utf-8"                      # Specifies the source and execution charset as UTF-8.
            "/MP"                         # Compile the engine source with multiple processes.

            "/W4"                         # Enable all reasonable warnings.
            "/WX"                         # Turn all warnings into errors.
            "/permissive"                 # Enable standards conformance.

            "/wd4244"                     # Disable "conversion from "type1" to "type2", possible loss of data".

            "/external:anglebrackets"     # Treat files included with angle brackets as external files.
            "/external:W0"                # Ignore external file warnings & errors.

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
        target_compile_options(${name} PRIVATE
            "-Wall"                          # Enable all warnings about construction.
            "-Wextra"                        # Enable warnings that aren"t enabled by -Wall.
            "-Werror"                        # Turn all warnings into errors.

            "-pedantic"                      # Enable warnings about compiler extensions.
            "-Wshadow"                       # Warn when a local declaration shadows another.

            # /-g: Generate debug info.
            # /O0: Disable optimizations.
            "$<$<CONFIG:Debug>:-g;-O0>"

            # -g: Generate debug info.
            # -O2: Enable level 2 optimizations.
            "$<$<CONFIG:Optimized>:-g;-O2>"

            # /Ox: Optimize level 3 optimizations.
            "$<$<CONFIG:Production>:-O3>"
        )
    endif()
endfunction()
