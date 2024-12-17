kitsune_add_library(
    TARGET KitsuneLaunch
    FORCE_STATIC

    SOURCES
    "EngineLoop.cpp"
    "EngineLoop.h"
    "EngineMain.cpp"

    DEPENDENCIES
    KitsuneApplicationCore
)

kitsune_add_platform_sources(
    TARGET KitsuneLaunch
    WINDOWS
    "Windows/WindowsEntry.cpp"
)
