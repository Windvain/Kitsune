kitsune_add_library(
    NAME KitsuneLaunch
    FORCE_STATIC

    SOURCES
    "EngineLoop.cpp"
    "EngineLoop.h"
    "EngineMain.cpp"

    LINKS
    KitsuneApplication
    KitsuneApplicationCore
    KitsuneFoundation
)

kitsune_add_platform_sources(
    NAME KitsuneLaunch
    WINDOWS
    "Windows/WindowsEntry.cpp"
)
