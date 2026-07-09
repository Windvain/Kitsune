#include "Display/DisplayManager.h"

#include "Foundation/Logging/Logger.h"
#include "Display/Null/NullDisplayManager.h"

#if defined(KITSUNE_OS_WINDOWS)
    #include "Display/Windows/WindowsDisplayManager.h"
#else
    #error Could not find an implementation for the display manager.
#endif

namespace Kitsune
{
    DisplayManager* DisplayManager::s_Instance = nullptr;

    DisplayManager* DisplayManager::Initialize(
        const DisplayManagerConfigurations& configs)
    {
        KITSUNE_INFO_FORMAT(
            "Initializing the {0} display manager.",
            configs.DisplayServer);

        if (configs.DisplayServer == "Null")
        {
            s_Instance = Memory::New<NullDisplayManager>(
                configs.NullDisplay.Size,
                configs.NullDisplay.RefreshRate,
                configs.NullDisplay.Orientation);
        }
#if defined(KITSUNE_OS_WINDOWS)
        else if (configs.DisplayServer == "Windows")
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
