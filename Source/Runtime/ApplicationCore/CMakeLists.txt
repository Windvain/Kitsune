kitsune_add_library(
    TARGET KitsuneApplicationCore
    SOURCES
    "Null/NullWindow.h"

    "Application.cpp"
    "Application.h"
    "BadWindowCreationException.h"
    "CommandLineArguments.h"
    "Environment.cpp"
    "Environment.h"
    "IPlatformApplication.h"
    "IWindow.h"

    DEPENDENCIES
    KitsuneFoundation
)

kitsune_add_platform_sources(
    TARGET KitsuneApplicationCore
    WINDOWS
    "Windows/WindowsApplication.cpp"
    "Windows/WindowsApplication.h"
    "Windows/WindowsWindow.cpp"
    "Windows/WindowsWindow.h"
)
