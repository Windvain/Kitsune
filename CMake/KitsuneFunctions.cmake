# Required arguments:
# NAME, SOURCES
#
# Usage:
# kitsune_add_library(NAME LibName SOURCES "path/to/file.cpp")
#
# Additional arguments: FORCE_STATIC, LINKS
function(kitsune_add_library)
    set(PARSE_OPTIONS FORCE_STATIC)
    set(PARSE_SINGLE_VALUE_ARGS NAME)
    set(PARSE_MULTI_VALUE_ARGS SOURCES LINKS)

    cmake_parse_arguments(
        LIBRARY_ARGS
        "${PARSE_OPTIONS}"
        "${PARSE_SINGLE_VALUE_ARGS}"
        "${PARSE_MULTI_VALUE_ARGS}"
        ${ARGN})

    # Preliminary checks for library arguments.
    if (NOT LIBRARY_ARGS_NAME)
        message(
            FATAL_ERROR
            "A target name has not been specified, please specify one with the NAME option.")
    endif()

    if (NOT LIBRARY_ARGS_SOURCES)
        message(
            FATAL_ERROR
            "No source files have been specified for ${LIBRARY_ARGS_NAME}.")
    endif()

    # Create the library with the specified sources.
    project(${LIBRARY_ARGS_NAME})
    if (LIBRARY_ARGS_FORCE_STATIC OR KITSUNE_BUILD_STATIC)
        add_library(${LIBRARY_ARGS_NAME} STATIC ${LIBRARY_ARGS_SOURCES})

        if (KITSUNE_BUILD_STATIC)
            target_compile_definitions(${LIBRARY_ARGS_NAME} PUBLIC "KITSUNE_IS_MONOLITHIC=1")
        endif()
    else()
        add_library(${LIBRARY_ARGS_NAME} SHARED ${LIBRARY_ARGS_SOURCES})
        target_compile_definitions(${LIBRARY_ARGS_NAME} PRIVATE "KITSUNE_EXPORTS=1")
    endif()

    # Set global compiler flags, linker flags, and include directories.
    target_compile_options(${LIBRARY_ARGS_NAME} PRIVATE ${KITSUNE_GLOBAL_COMPILER_FLAGS})
    target_compile_definitions(${LIBRARY_ARGS_NAME} PRIVATE ${KITSUNE_GLOBAL_COMPILER_DEFINITIONS})

    target_link_options(${LIBRARY_ARGS_NAME} PRIVATE ${KITSUNE_GLOBAL_LINKER_FLAGS})
    target_link_libraries(${LIBRARY_ARGS_NAME} PRIVATE ${LIBRARY_ARGS_LINKS})

    target_include_directories(${LIBRARY_ARGS_NAME} PRIVATE ${KITSUNE_GLOBAL_INCLUDE_DIRECTORIES})
endfunction()

# Required arguments:
# NAME, SOURCES
#
# Usage:
# kitsune_add_test(NAME TestName SOURCES "path/to/file.cpp")
#
# Additional arguments: LINKS
function(kitsune_add_test)
    set(PARSE_OPTIONS)
    set(PARSE_SINGLE_VALUE_ARGS NAME)
    set(PARSE_MULTI_VALUE_ARGS SOURCES LINKS)

    cmake_parse_arguments(
        TEST_ARGS
        "${PARSE_OPTIONS}"
        "${PARSE_SINGLE_VALUE_ARGS}"
        "${PARSE_MULTI_VALUE_ARGS}"
        ${ARGN})

    # Preliminary checks for test excecutable arguments.
    if (NOT TEST_ARGS_NAME)
        message(
            FATAL_ERROR
            "A target name has not been specified, please specify a name for the test executable with the NAME argument.")
    endif()

    if (NOT TEST_ARGS_SOURCES)
        message(
            FATAL_ERROR
            "No sources were specified for ${TEST_ARGS_NAME}.")
    endif()

    # Create the test executable.
    if (WIN32)
        set(TEST_TYPE WIN32)
    else()
        set(TEST_TYPE)
    endif()

    add_executable(${TEST_ARGS_NAME} ${TEST_TYPE} ${TEST_ARGS_SOURCES})
    add_test(NAME ${TEST_ARGS_NAME} COMMAND ${TEST_ARGS_NAME})

    # Set include directories, compiler flags, and link libraries.
    target_include_directories(${TEST_ARGS_NAME} PRIVATE
        ${KITSUNE_GLOBAL_INCLUDE_DIRECTORIES}
        "${KITSUNE_ROOT_DIR}/Source/External/googletest/googletest/include"
        "${KITSUNE_ROOT_DIR}/Source/External/googletest/googlemock/include"
    )

    if (MSVC)
        # MSVC corrupts UTF-8 strings if /utf-8 is not specified.
        # It first encodes the string as UTF-8, then encodes it again in Windows-1252.
        #
        # Thanks, Microsoft.
        # https://stackoverflow.com/questions/59046071/c-u8-literal-unexpected-encoding-on-windows
        target_compile_options(${TEST_ARGS_NAME} PRIVATE "/utf-8")
    endif()

    # Use global flags here, or else it might cause build type incompatibility issues between
    # libraries and executables. (e.g. /MDd and /MD are incompatible)
    target_compile_options(${TEST_ARGS_NAME} PRIVATE ${KITSUNE_GLOBAL_COMPILER_FLAGS})
    target_link_options(${TEST_ARGS_NAME} PRIVATE ${KITSUNE_GLOBAL_LINKER_FLAGS})

    target_link_libraries(${TEST_ARGS_NAME} PRIVATE
        GTest::gtest
        GTest::gmock
        KitsuneLaunch
        KitsuneApplicationCore

        ${TARGET_EXE_ARGS_DEPENDENCIES}
    )
endfunction()

# Required arguments:
# NAME, [WINDOWS/LINUX/GCC/CLANG/MSVC]
#
# Usage:
# kitsune_add_platform_sources(NAME TestName WINDOWS "windows/file.cpp")
#
# Additional arguments: WINDOWS, LINUX, GCC, CLANG, MSVC
function(kitsune_add_platform_sources)
    set(PARSE_OPTIONS)
    set(PARSE_SINGLE_VALUE_ARGS NAME)
    set(PARSE_MULTI_VALUE_ARGS WINDOWS LINUX GCC CLANG MSVC)

    cmake_parse_arguments(
        TARGET_ARGS
        "${PARSE_OPTIONS}"
        "${PARSE_SINGLE_VALUE_ARGS}"
        "${PARSE_MULTI_VALUE_ARGS}"
        ${ARGN})

    # Check for missing arguments.
    if (NOT TARGET_ARGS_NAME)
        message(
            FATAL_ERROR
            "A target name has not been specified, please specify one with the NAME argument.")
    endif()

    # Add platform-specific files.
    set(TARGET_PLATFORM_SOURCES)
    if (TARGET_ARGS_WINDOWS AND WIN32)
        list(APPEND TARGET_PLATFORM_SOURCES ${TARGET_ARGS_WINDOWS})
    endif()

    if (TARGET_ARGS_LINUX AND ("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux"))
        list(APPEND TARGET_PLATFORM_SOURCES ${TARGET_ARGS_LINUX})
    endif()

    if (TARGET_ARGS_GCC AND ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU"))
        list(APPEND TARGET_PLATFORM_SOURCES ${TARGET_ARGS_GCC})
    endif()

    if (TARGET_ARGS_CLANG AND ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang"))
        list(APPEND TARGET_PLATFORM_SOURCES ${TARGET_ARGS_CLANG})
    endif()

    if (TARGET_ARGS_MSVC AND MSVC)
        list(APPEND TARGET_PLATFORM_SOURCES ${TARGET_ARGS_MSVC})
    endif()

    if (NOT TARGET_PLATFORM_SOURCES)
        message(
            FATAL_ERROR
            "No platform-specific sources have been given. Please specify at least one.")
    endif()

    target_sources(${TARGET_ARGS_NAME} PRIVATE ${TARGET_PLATFORM_SOURCES})
endfunction()

# Required arguments:
# NAME, [WINDOWS/LINUX/GCC/CLANG/MSVC]
#
# Usage:
# kitsune_add_platform_sources(NAME TestName WINDOWS "windows/file.cpp")
#
# Additional arguments: WINDOWS, LINUX, GCC, CLANG, MSVC
function(kitsune_add_platform_links)
    set(PARSE_OPTIONS)
    set(PARSE_SINGLE_VALUE_ARGS NAME)
    set(PARSE_MULTI_VALUE_ARGS WINDOWS LINUX GCC CLANG MSVC)

    cmake_parse_arguments(
        TARGET_ARGS
        "${PARSE_OPTIONS}"
        "${PARSE_SINGLE_VALUE_ARGS}"
        "${PARSE_MULTI_VALUE_ARGS}"
        ${ARGN})

    # Check for missing arguments.
    if (NOT TARGET_ARGS_NAME)
        message(FATAL_ERROR "A target has not been specified. Please specify one with the NAME argument.")
    endif()

    # Add platform-specific links.
    set(TARGET_PLATFORM_LINKS)
    if (TARGET_ARGS_WINDOWS AND WIN32)
        list(APPEND TARGET_PLATFORM_LINKS ${TARGET_ARGS_WINDOWS})
    endif()

    if (TARGET_ARGS_LINUX AND ("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux"))
        list(APPEND TARGET_PLATFORM_LINKS ${TARGET_ARGS_LINUX})
    endif()

    if (TARGET_ARGS_GCC AND ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU"))
        list(APPEND TARGET_PLATFORM_LINKS ${TARGET_ARGS_GCC})
    endif()

    if (TARGET_ARGS_CLANG AND ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang"))
        list(APPEND TARGET_PLATFORM_LINKS ${TARGET_ARGS_CLANG})
    endif()

    if (TARGET_ARGS_MSVC AND MSVC)
        list(APPEND TARGET_PLATFORM_LINKS ${TARGET_ARGS_MSVC})
    endif()

    if (NOT TARGET_PLATFORM_LINKS)
        message(
            FATAL_ERROR
            "No libraries were specified. Please specify at least one library with the platform's name as its argument.")
    endif()

    target_link_libraries(${TARGET_ARGS_NAME} PRIVATE ${TARGET_PLATFORM_LINKS})
endfunction()
