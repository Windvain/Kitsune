#include "Application/DisplayManager.h"

#include "Foundation/Common/Predefined.h"
#include "Foundation/Logging/GlobalLog.h"
#include "Foundation/Diagnostics/LogicException.h"

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
        {
            throw LogicException(
                "An instance of DisplayManager has already been created.");
        }

        if (specs.Headless)
        {
            KITSUNE_ENGINE_INFO(
                DisplayManager,
                "DisplayManagerSpecifications::Headless was set to true, "
                "the engine will construct a NULL display manager.");

            auto screen = MakeScoped<NullScreen>(
                Vector2<Uint32>(1920, 1080),        // Size in pixels
                Vector2<Int32>(),                   // Virtual position
                60,                                 // Refresh rate in Hertz
                96,                                 // DPI
                ScreenOrientation::Default          // Screen orientation
            );

            s_Instance = Memory::New<NullDisplayManager>(Move(screen), specs);
        }
        else
        {
#if defined(KITSUNE_OS_WINDOWS)
            s_Instance = Memory::New<WindowsDisplayManager>(specs, L"Kitsune Window");
#else
            KITSUNE_ENGINE_ERROR(
                DisplayManager,
                "Could not find an implementation for DisplayManager. Using the "
                "NULL implementation instead.");

            s_Instance = Memory::New<NullDisplayManager>();
#endif
        }

        return s_Instance;
    }

    void DisplayManager::Shutdown()
    {
        if (s_Instance == nullptr)
            return;

        Memory::Delete(s_Instance);
        s_Instance = nullptr;
    }
}
