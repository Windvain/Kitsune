kitsune_add_library(
    NAME KitsuneApplication
    SOURCES
    "Application.cpp"
    "Application.h"

    LINKS
    KitsuneApplicationCore
    KitsuneFoundation
)

kitsune_add_platform_sources(
    NAME KitsuneApplication
    WINDOWS "Windows/WindowsApplication.cpp"
)
