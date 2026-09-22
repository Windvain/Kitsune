#include "Display/DisplayManager.h"
#include "Display/Null/NullDisplayManager.h"

#include "Foundation/Logging/Logger.h"
#include "Foundation/Diagnostics/InvalidArgumentException.h"

#if defined(KITSUNE_OS_WINDOWS)
    #include "Foundation/String/TranscodePresets.h"
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
        KITSUNE_ENGINE_INFO_FORMAT(
            Display,
            "Initializing the \"{0}\" display manager.", configs.DisplayServer);

        if (configs.DisplayServer == "Null")
        {
            s_Instance = Memory::New<NullDisplayManager>(
                configs.VirtualDisplay.Size,
                configs.VirtualDisplay.RefreshRate,
                configs.VirtualDisplay.Scaling);
        }
#if defined(KITSUNE_OS_WINDOWS)
        else if (configs.DisplayServer == "Windows")
        {
            WideString className = UTF8ToUTF16<char, wchar_t>(configs.WindowClassName);
            s_Instance = Memory::New<WindowsDisplayManager>(className);
        }
#endif
        else
        {
            throw InvalidArgumentException(
                "The specified display server \"{0}\" was not compiled with the "
                "engine.",
                configs.DisplayServer);
        }

        return s_Instance;
    }

    void DisplayManager::Shutdown()
    {
        KITSUNE_INFO("Shutting down the display manager.");
        Memory::Delete(s_Instance);
    }
}
