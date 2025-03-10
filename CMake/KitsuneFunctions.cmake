function(kitsune_add_library)
    set(PARSE_OPTIONS FORCE_STATIC)
    set(PARSE_SINGLE_VALUE_ARGS TARGET)
    set(PARSE_MULTI_VALUE_ARGS SOURCES DEPENDENCIES)

    cmake_parse_arguments(
        LIBRARY_ARGS
        "${PARSE_OPTIONS}"
        "${PARSE_SINGLE_VALUE_ARGS}"
        "${PARSE_MULTI_VALUE_ARGS}"
        ${ARGN})

    if (NOT LIBRARY_ARGS_TARGET)
        message(FATAL_ERROR "A target name has not been specified.")
    endif()

    project(${LIBRARY_ARGS_TARGET})

    if (LIBRARY_ARGS_FORCE_STATIC OR KITSUNE_BUILD_STATIC)
        add_library(${LIBRARY_ARGS_TARGET} STATIC ${LIBRARY_ARGS_SOURCES})
        target_compile_definitions(${LIBRARY_ARGS_TARGET} PUBLIC "KITSUNE_IS_MONOLITHIC=1")
    else()
        add_library(${LIBRARY_ARGS_TARGET} SHARED ${LIBRARY_ARGS_SOURCES})
        target_compile_definitions(${LIBRARY_ARGS_TARGET}
            PRIVATE "KITSUNE_EXPORTS=1"
            PUBLIC "KITSUNE_IS_SHARED=1"
        )
    endif()

    target_include_directories(${LIBRARY_ARGS_TARGET} PRIVATE "${KITSUNE_ROOT_DIR}/Source/Runtime")
    target_compile_definitions(${LIBRARY_ARGS_TARGET} PRIVATE ${KITSUNE_GLOBAL_COMMON_DEFINITIONS})

    target_compile_options(${LIBRARY_ARGS_TARGET} PRIVATE ${KITSUNE_GLOBAL_COMMON_COMPILE_FLAGS})
    target_link_options(${LIBRARY_ARGS_TARGET} PRIVATE ${KITSUNE_GLOBAL_COMMON_LINKER_FLAGS})
    target_link_libraries(${LIBRARY_ARGS_TARGET} PRIVATE ${LIBRARY_ARGS_DEPENDENCIES})
endfunction()

function(kitsune_add_test)
    set(PARSE_OPTIONS)
    set(PARSE_SINGLE_VALUE_ARGS TARGET)
    set(PARSE_MULTI_VALUE_ARGS SOURCES DEPENDENCIES)

    cmake_parse_arguments(
        TEST_EXE_ARGS
        "${PARSE_OPTIONS}"
        "${PARSE_SINGLE_VALUE_ARGS}"
        "${PARSE_MULTI_VALUE_ARGS}"
        ${ARGN})

    if (NOT TEST_EXE_ARGS_TARGET)
        message(FATAL_ERROR "A target name has not been specified.")
    endif()

    project(${TEST_EXE_ARGS_TARGET})

    if (WIN32)
        add_executable(${TEST_EXE_ARGS_TARGET} WIN32 ${TEST_EXE_ARGS_SOURCES})
    else()
        add_executable(${TEST_EXE_ARGS_TARGET} ${TEST_EXE_ARGS_SOURCES})
    endif()

    add_test(NAME ${TEST_EXE_ARGS_TARGET} COMMAND ${TEST_EXE_ARGS_TARGET})
    target_include_directories(${TEST_EXE_ARGS_TARGET} PRIVATE
        "${KITSUNE_ROOT_DIR}/Source/Runtime"
        "${KITSUNE_ROOT_DIR}/Source/External/googletest/googletest/include"
        "${KITSUNE_ROOT_DIR}/Source/External/googletest/googlemock/include"
    )

    target_link_libraries(${TEST_EXE_ARGS_TARGET} PRIVATE
        GTest::gtest
        GTest::gmock
        KitsuneLaunch
        KitsuneApplicationCore

        ${TARGET_EXE_ARGS_DEPENDENCIES})
endfunction()

function(kitsune_add_platform_sources)
    set(PARSE_OPTIONS)
    set(PARSE_SINGLE_VALUE_ARGS TARGET)
    set(PARSE_MULTI_VALUE_ARGS WINDOWS LINUX GCC CLANG MSVC)

    cmake_parse_arguments(
        TARGET_ARGS
        "${PARSE_OPTIONS}"
        "${PARSE_SINGLE_VALUE_ARGS}"
        "${PARSE_MULTI_VALUE_ARGS}"
        ${ARGN})

    # Check for missing arguments.
    if (NOT TARGET_ARGS_TARGET)
        message(FATAL_ERROR "Target has not been specified.")
    endif()

    # Add platform-specific files.
    if (TARGET_ARGS_WINDOWS AND WIN32)
        target_sources(${TARGET_ARGS_TARGET} PRIVATE ${TARGET_ARGS_WINDOWS})
    endif()

    if (TARGET_ARGS_LINUX AND ("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux"))
        target_sources(${TARGET_ARGS_TARGET} PRIVATE ${TARGET_ARGS_LINUX})
    endif()

    if (TARGET_ARGS_GCC AND ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU"))
        target_sources(${TARGET_ARGS_TARGET} PRIVATE ${TARGET_ARGS_GCC})
    endif()

    if (TARGET_ARGS_CLANG AND ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang"))
        target_sources(${TARGET_ARGS_TARGET} PRIVATE ${TARGET_ARGS_CLANG})
    endif()

    if (TARGET_ARGS_MSVC AND MSVC)
        target_sources(${TARGET_ARGS_TARGET} PRIVATE ${TARGET_ARGS_MSVC})
    endif()
endfunction()

function(kitsune_add_platform_dependencies)
    set(PARSE_OPTIONS)
    set(PARSE_SINGLE_VALUE_ARGS TARGET)
    set(PARSE_MULTI_VALUE_ARGS WINDOWS LINUX GCC CLANG MSVC)

    cmake_parse_arguments(
        TARGET_ARGS
        "${PARSE_OPTIONS}"
        "${PARSE_SINGLE_VALUE_ARGS}"
        "${PARSE_MULTI_VALUE_ARGS}"
        ${ARGN})

    # Check for missing arguments.
    if (NOT TARGET_ARGS_TARGET)
        message(FATAL_ERROR "Target has not been specified.")
    endif()

    # Add platform-specific files.
    if (TARGET_ARGS_WINDOWS AND WIN32)
        target_link_libraries(${TARGET_ARGS_TARGET} PRIVATE ${TARGET_ARGS_WINDOWS})
    endif()

    if (TARGET_ARGS_LINUX AND ("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux"))
        target_link_libraries(${TARGET_ARGS_TARGET} PRIVATE ${TARGET_ARGS_LINUX})
    endif()

    if (TARGET_ARGS_GCC AND ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU"))
        target_link_libraries(${TARGET_ARGS_TARGET} PRIVATE ${TARGET_ARGS_GCC})
    endif()

    if (TARGET_ARGS_CLANG AND ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang"))
        target_link_libraries(${TARGET_ARGS_TARGET} PRIVATE ${TARGET_ARGS_CLANG})
    endif()

    if (TARGET_ARGS_MSVC AND MSVC)
        target_link_libraries(${TARGET_ARGS_TARGET} PRIVATE ${TARGET_ARGS_MSVC})
    endif()
endfunction()
