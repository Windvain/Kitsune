# When targeting Windows platforms, there are two identifiers used for x86_64 targets.
# A GNU ABI target will define "x86_64", while an MSVC ABI will define "x64".
if ("x64" IN_LIST CMAKE_CXX_COMPILER_ARCHITECTURE_ID OR "x86_64" IN_LIST CMAKE_CXX_COMPILER_ARCHITECTURE_ID)
    set(KITSUNE_TARGET_ARCHITECTURE "x86_64")
endif()
