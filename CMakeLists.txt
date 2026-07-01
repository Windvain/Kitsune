cmake_minimum_required(VERSION 3.22)
project(
    Kitsune
    LANGUAGES C CXX

    VERSION 0.0.1
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (MSVC)
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
endif()

if (NOT CMAKE_CONFIGURATION_TYPES)
    message(FATAL_ERROR "${CMAKE_GENERATOR} is not supported, only multi-config generators are supported.")
else()
    set(CMAKE_CONFIGURATION_TYPES "Debug" "Optimized" "Production" CACHE STRING "" FORCE)

    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

    # Hack: Some multi-configuration generators, notably Visual Studio, will
    #       will generate seperate build directories for each build type.
    foreach (CONFIG_TYPE ${CMAKE_CONFIGURATION_TYPES})
        set(TOUPPER ${CONFIG_TYPE} CONFIG_TYPE)

        set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CONFIG_TYPE} "${CMAKE_BINARY_DIR}/lib")
        set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CONFIG_TYPE} "${CMAKE_BINARY_DIR}/lib")
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONFIG_TYPE} "${CMAKE_BINARY_DIR}/bin")
    endforeach()
endif()

message(STATUS "Source directory: ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "Binary directory: ${CMAKE_BINARY_DIR}")
message(STATUS "")
message(STATUS "Generator: ${CMAKE_GENERATOR}")

if (KITSUNE_BUILD_TEST_EXECUTABLES)
    enable_testing()
endif()

include("${CMAKE_CURRENT_SOURCE_DIR}/CMake/KitsuneOptions.cmake")
include("${CMAKE_CURRENT_SOURCE_DIR}/CMake/KitsuneUtilities.cmake")

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/External")

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/Source/Runtime")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/Source/Launcher")

if (KITSUNE_BUILD_TEST_EXECUTABLES)
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/Tests")
endif()

if (KITSUNE_BUILD_EXAMPLE_EXECUTABLES)
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/Examples")
endif()
