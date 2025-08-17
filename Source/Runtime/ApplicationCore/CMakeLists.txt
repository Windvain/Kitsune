kitsune_add_library(
    NAME KitsuneApplicationCore
    SOURCES
    "Null/NullMonitor.h"
    "Null/NullWindow.cpp"
    "Null/NullWindow.h"

    "CommandLineArguments.h"
    "Environment.h"
    "IMonitor.h"
    "IWindow.h"
    "MonitorException.h"
    "VideoMode.h"
    "WindowException.h"

    LINKS
    KitsuneFoundation
)

kitsune_add_platform_sources(
    NAME KitsuneApplicationCore
    WINDOWS
    "Windows/WindowsEnvironment.cpp"
    "Windows/WindowsMonitor.cpp"
    "Windows/WindowsMonitor.h"
    "Windows/WindowsWindow.cpp"
    "Windows/WindowsWindow.h"
)
