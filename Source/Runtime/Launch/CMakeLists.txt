kitsune_add_library(
    TARGET KitsuneLaunch
    FORCE_STATIC
)

kitsune_add_platform_sources(
    TARGET KitsuneLaunch
    WINDOWS
    "Windows/WindowsEntry.cpp"
)
