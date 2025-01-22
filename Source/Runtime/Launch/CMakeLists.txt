kitsune_add_library(
    TARGET KitsuneLaunch
    FORCE_STATIC

    SOURCES
    "EngineMain.cpp"

    DEPENDENCIES
    KitsuneApplicationCore
    KitsuneFoundation
)

kitsune_add_platform_sources(
    TARGET KitsuneLaunch
    WINDOWS
    "Windows/WindowsEntry.cpp"
)
