cmake_minimum_required(VERSION 3.22)
project(Kitsune)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(KITSUNE_DEFAULT_BUILD_TYPE "Release")
set(KITSUNE_CONFIG_TYPES "Debug" "RelWithDbgInfo" "Release")

option(KITSUNE_BUILD_STATIC "Build all Kitsune libraries as static libs." OFF)
option(KITSUNE_BUILD_EXAMPLES "Build Kitsune's example executables." ON)

if (CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Detected multi-configuration CMake generator.")
    set(CMAKE_CONFIGURATION_TYPES ${KITSUNE_CONFIG_TYPES} CACHE STRING "" FORCE)
else()
    message(STATUS "Detected single-configuration CMake generator.")

    if(NOT CMAKE_BUILD_TYPE)
        message(STATUS "CMAKE_BUILD_TYPE has not been set, setting build type to ${KITSUNE_DEFAULT_BUILD_TYPE}.")
        set(CMAKE_BUILD_TYPE "${KITSUNE_DEFAULT_BUILD_TYPE}" CACHE
            STRING "" FORCE)
    endif()

    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS ${KITSUNE_CONFIG_TYPES})
endif()

include("${CMAKE_CURRENT_SOURCE_DIR}/CMake/KitsuneGlobals.cmake")
include("${CMAKE_CURRENT_SOURCE_DIR}/CMake/KitsuneFunctions.cmake")

add_subdirectory("Source/Runtime")

if (KITSUNE_BUILD_EXAMPLES)
    add_subdirectory("Examples")
endif()
