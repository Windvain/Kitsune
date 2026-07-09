#include "Display/WindowManager.h"

#include "Foundation/Logging/Logger.h"
#include "Display/Null/NullWindowManager.h"

#if defined(KITSUNE_OS_WINDOWS)
    #include "Display/Windows/WindowsWindowManager.h"
#else
    #error Could not find an implementation for the window manager.
#endif

namespace Kitsune
{
    WindowManager* WindowManager::s_Instance = nullptr;

    WindowManager* WindowManager::Initialize(StringView serverName)
    {
        KITSUNE_INFO_FORMAT("Initializing the {0} window manager.", serverName);

        // Linux-based distros can use either X11 or Wayland.
        // Other operating systems just use the built-in display server.
#if !defined(KITSUNE_OS_LINUX)
        KITSUNE_UNUSED(serverName);
#endif

        if (serverName == "Null")
            s_Instance = Memory::New<NullWindowManager>();
        else if (serverName == "Windows")
#if defined(KITSUNE_OS_WINDOWS)
            s_Instance = Memory::New<WindowsWindowManager>();
#endif

        return s_Instance;
    }

    void WindowManager::Shutdown()
    {
        KITSUNE_INFO("Shutting down the window manager.");
        Memory::Delete(s_Instance);
    }
}
