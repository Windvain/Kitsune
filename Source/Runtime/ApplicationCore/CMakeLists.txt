kitsune_add_library(
    TARGET KitsuneApplicationCore
    SOURCES
    "Application.cpp"
    "Application.h"
    "CommandLineArguments.h"
    "Environment.cpp"
    "Environment.h"
    "IPlatformApplication.h"

    DEPENDENCIES
    KitsuneFoundation
)

kitsune_add_platform_sources(
    TARGET KitsuneApplicationCore
    WINDOWS
    "Windows/WindowsApplication.cpp"
    "Windows/WindowsApplication.h"
)
