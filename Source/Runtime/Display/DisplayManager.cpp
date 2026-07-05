#include "Display/DisplayManager.h"
#include "Foundation/Logging/Logger.h"

#if defined(KITSUNE_OS_WINDOWS)
    #include "Display/Windows/WindowsDisplayManager.h"
#else
    #error Could not find an implementation for the display manager.
#endif

namespace Kitsune
{
    DisplayManager* DisplayManager::s_Instance = nullptr;

    DisplayManager* DisplayManager::Initialize(StringView serverName)
    {
        KITSUNE_INFO_FORMAT("Initializing the {0} display manager.", serverName);

        // Linux-based distros can use either X11 or Wayland.
        // Other operating systems just use the built-in display server.
#if !defined(KITSUNE_OS_LINUX)
        KITSUNE_UNUSED(serverName);
#endif

#if defined(KITSUNE_OS_WINDOWS)
        s_Instance = Memory::New<WindowsDisplayManager>();
#endif

        return s_Instance;
    }

    void DisplayManager::Shutdown()
    {
        KITSUNE_INFO("Shutting down the display manager.");
        Memory::Delete(s_Instance);
    }
}
