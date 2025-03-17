kitsune_add_library(
    TARGET KitsuneApplicationCore
    SOURCES
    "Null/NullWindow.cpp"
    "Null/NullWindow.h"

    "Application.cpp"
    "Application.h"
    "BadMonitorCreationException.h"
    "BadWindowCreationException.h"
    "CommandLineArguments.h"
    "Environment.cpp"
    "Environment.h"
    "IMonitor.h"
    "IPlatformApplication.h"
    "IWindow.h"
    "VideoMode.h"

    DEPENDENCIES
    KitsuneFoundation
)

kitsune_add_platform_sources(
    TARGET KitsuneApplicationCore
    WINDOWS
    "Windows/WindowsApplication.cpp"
    "Windows/WindowsApplication.h"
    "Windows/WindowsMonitor.cpp"
    "Windows/WindowsMonitor.h"
    "Windows/WindowsWindow.cpp"
    "Windows/WindowsWindow.h"
)

