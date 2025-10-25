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

    set_target_properties(${TEST_ARGS_NAME} PROPERTIES FOLDER "Tests")

    # Set include directories, compiler flags, and link libraries.
    target_include_directories(${TEST_ARGS_NAME} PRIVATE
        ${KITSUNE_GLOBAL_INCLUDE_DIRECTORIES}
        "${KITSUNE_ROOT_DIR}/Source/External/googletest/googletest/include"
        "${KITSUNE_ROOT_DIR}/Source/External/googletest/googlemock/include"
    )

    # Use global flags here, or else it might cause build type incompatibility issues between
    # libraries and executables. (e.g. /MDd and /MD are incompatible)
    target_compile_options(${TEST_ARGS_NAME} PRIVATE ${KITSUNE_GLOBAL_COMPILER_FLAGS})
    target_link_options(${TEST_ARGS_NAME} PRIVATE ${KITSUNE_GLOBAL_LINKER_FLAGS})

    target_link_libraries(${TEST_ARGS_NAME} PRIVATE
        GTest::gtest
        GTest::gmock

        KitsuneEngine
        ${TARGET_EXE_ARGS_DEPENDENCIES}
    )
endfunction()
