option(
    KITSUNE_BUILD_EXAMPLE_EXECUTABLES
    "Builds the example executables located in the Examples directory."
    TRUE
)

option(
    KITSUNE_BUILD_TEST_EXECUTABLES
    "Builds the test executables."
    TRUE
)

option(
    KITSUNE_OVERRIDE_BACKTRACE_SUPPORT
    "Enables backtrace support even when the build type disables it."
    FALSE
)

option(
    KITSUNE_ENABLE_TERMINAL_FOR_DEBUGGING
    "Always show the terminal even in Production builds."
    FALSE
)

option(
    KITSUNE_BUILD_VULKAN_BACKEND
    "Builds the Vulkan rendering backend."
    TRUE
)

option(
    KITSUNE_ENABLE_SIMD_OPTIMIZATIONS
    "Builds the engine with SIMD optimizations enabled on Matrix/Vector code."
    TRUE
)
