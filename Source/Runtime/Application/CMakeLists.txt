kitsune_add_library(
    TARGET KitsuneApplication
    SOURCES
    "Application.cpp"
    "Application.h"

    DEPENDENCIES
    KitsuneApplicationCore
    KitsuneFoundation
)

kitsune_add_platform_sources(
    TARGET KitsuneApplication
    WINDOWS "Windows/WindowsApplication.cpp"
)
