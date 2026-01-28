option(
    BUILD_EXAMPLE_EXECUTABLES
    "Builds the example executables located in the Examples directory."
    TRUE
)

option(
    BUILD_TEST_EXECUTABLES
    "Builds the test executables."
    TRUE
)

option(
    OVERRIDE_BACKTRACE_SUPPORT
    "Enables backtrace support even when the build type disables it."
    FALSE
)
