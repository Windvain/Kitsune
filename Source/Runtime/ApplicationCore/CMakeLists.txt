kitsune_add_library(
    TARGET KitsuneApplicationCore
    SOURCES
    "Application.cpp"
    "Application.h"

    DEPENDENCIES
    KitsuneFoundation
)

kitsune_add_platform_sources(
    TARGET KitsuneApplicationCore
    WINDOWS
    "Windows/WindowsApplication.cpp"
)
