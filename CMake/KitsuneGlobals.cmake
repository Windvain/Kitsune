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

macro(combine_build_specific_vars list)
    foreach (CONFIG_TYPE IN LISTS KITSUNE_CONFIG_TYPES)
        string(REPLACE "COMMON" ${CONFIG_TYPE} ${list}_${CONFIG_TYPE}_CONFIG "${list}")
    endforeach()

    list(APPEND KITSUNE_GLOBAL_COMMON_COMPILE_FLAGS
        # As of CMake <= 3.31.2, generator expressions can't be used when expanding variables.
        $<$<CONFIG:Debug>:${${${list}_DEBUG_CONFIG}}>
        $<$<CONFIG:RelWithDbgInfo>:${${${list}_RELWITHDBGINFO_CONFIG}}>
        $<$<CONFIG:Release>:${${${list}_RELEASE_CONFIG}}>
    )
endmacro()

# Bring in all build type-specific variables into the common variable.
list(APPEND KITSUNE_GLOBAL_COMMON_DEFINITIONS
    "KITSUNE_BUILD_$<UPPER_CASE:$<CONFIG>>=1"               # KITSUNE_BUILD_[$<CONFIG>]
)

combine_build_specific_vars(KITSUNE_GLOBAL_COMMON_DEFINITIONS)
combine_build_specific_vars(KITSUNE_GLOBAL_COMMON_COMPILE_FLAGS)
combine_build_specific_vars(KITSUNE_GLOBAL_COMMON_LINKER_FLAGS)
