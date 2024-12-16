# Common versions of build type-specific variables.
set(KITSUNE_GLOBAL_COMMON_COMPILE_FLAGS)
set(KITSUNE_GLOBAL_COMMON_LINKER_FLAGS)
set(KITSUNE_GLOBAL_COMMON_DEFINITIONS)

# Set global variables for each build type.
foreach(CONFIG_TYPE IN LISTS KITSUNE_CONFIG_TYPES)
    string(TOUPPER ${CONFIG_TYPE} CONFIG_TYPE)

    set(KITSUNE_GLOBAL_${CONFIG_TYPE}_COMPILE_FLAGS)
    set(KITSUNE_GLOBAL_${CONFIG_TYPE}_LINKER_FLAGS)
    set(KITSUNE_GLOBAL_${CONFIG_TYPE}_DEFINITIONS)
endforeach()

include("${CMAKE_CURRENT_LIST_DIR}/Platforms/${CMAKE_SYSTEM_NAME}Config.cmake")

# Bring in all build type-specific variables into the common variable.
list(APPEND KITSUNE_GLOBAL_COMMON_DEFINITIONS
    "KITSUNE_BUILD_$<UPPER_CASE:$<CONFIG>>=1"               # KITSUNE_BUILD_[$<CONFIG>]

    # As of CMake <= 3.31.2, generator expressions can't be used when expanding variables.
    $<$<CONFIG:Debug>:${KITSUNE_GLOBAL_DEBUG_DEFINITIONS}>
    $<$<CONFIG:RelWithDbgInfo>:${KITSUNE_GLOBAL_RELWITHDBGINFO_DEFINITIONS}>
    $<$<CONFIG:Release>:${KITSUNE_GLOBAL_RELEASE_DEFINITIONS}>
)

list(APPEND KITSUNE_GLOBAL_COMMON_COMPILE_FLAGS
    # As of CMake <= 3.31.2, generator expressions can't be used when expanding variables.
    $<$<CONFIG:Debug>:${KITSUNE_GLOBAL_DEBUG_COMPILE_FLAGS}>
    $<$<CONFIG:RelWithDbgInfo>:${KITSUNE_GLOBAL_RELWITHDBGINFO_COMPILE_FLAGS}>
    $<$<CONFIG:Release>:${KITSUNE_GLOBAL_RELEASE_COMPILE_FLAGS}>
)

list(APPEND KITSUNE_GLOBAL_COMMON_LINKER_FLAGS
    # As of CMake <= 3.31.2, generator expressions can't be used when expanding variables.
    $<$<CONFIG:Debug>:${KITSUNE_GLOBAL_DEBUG_LINKER_FLAGS}>
    $<$<CONFIG:RelWithDbgInfo>:${KITSUNE_GLOBAL_RELWITHDBGINFO_LINKER_FLAGS}>
    $<$<CONFIG:Release>:${KITSUNE_GLOBAL_RELEASE_LINKER_FLAGS}>
)
