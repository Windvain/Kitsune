#include "Application/DisplayManager.h"

#include "Foundation/Common/Predefined.h"
#include "Foundation/Logging/GlobalLog.h"

#include "Application/Null/NullDisplayManager.h"

#if defined(KITSUNE_OS_WINDOWS)
    #include "Application/Windows/WindowsDisplayManager.h"
#endif

namespace Kitsune
{
    DisplayManager* DisplayManager::s_Instance = nullptr;

    DisplayManager* DisplayManager::Initialize(const DisplayManagerSpecifications& specs)
    {
        if (s_Instance != nullptr)
            return s_Instance;

        if (specs.Headless)
        {
            KITSUNE_ENGINE_INFO_("DisplayManagerSpecifications::Headless was set to true, "
                                 "the engine will construct a NULL display manager.");

            s_Instance = Memory::New<NullDisplayManager>();
        }
        else
        {
#if defined(KITSUNE_OS_WINDOWS)
            KITSUNE_ENGINE_INFO_("Constructing the Windows display manager.");
            s_Instance = Memory::New<WindowsDisplayManager>();
#else
            KITSUNE_ENGINE_ERROR_(
                "Could not find an implementation for DisplayManager. Using the "
                "NULL implementation instead.");

            s_Instance = Memory::New<NullDisplayManager>();
#endif
        }

        return s_Instance;
    }

    void DisplayManager::Shutdown()
    {
        KITSUNE_ENGINE_INFO_("The display manager is being shut down.");

        Memory::Delete(s_Instance);
        s_Instance = nullptr;
    }
}
