#include "Application/Windows/WindowsDisplayManager.h"

#include "Foundation/Memory/Memory.h"
#include "Application/Windows/WindowsScreen.h"

namespace Kitsune
{
    WindowsDisplayManager::WindowsDisplayManager()
    {
    }

    WindowsDisplayManager::~WindowsDisplayManager()
    {
    }

    void WindowsDisplayManager::Update()
    {
    }

    Array<SharedPtr<Screen>> WindowsDisplayManager::GetScreens() const
    {
        Array<SharedPtr<Screen>> screens;
        EnumerateMonitors([&screens](const DISPLAY_DEVICEW& device)
        {
            screens.PushBack(MakeShared<WindowsScreen>(device.DeviceName));
        });

        return screens;
    }

    SharedPtr<Screen> WindowsDisplayManager::GetPrimaryScreen() const
    {
        SharedPtr<Screen> primaryScreen;
        EnumerateMonitors([&primaryScreen](const DISPLAY_DEVICEW& device)
        {
            for (DWORD index = 0; /* ... */; ++index)
            {
                DISPLAY_DEVICEW monitorDevice;
                monitorDevice.cb = sizeof(monitorDevice);

                if (!::EnumDisplayDevicesW(device.DeviceName, index, &monitorDevice, 0))
                {
                    break;
                }

                if (monitorDevice.StateFlags & (DISPLAY_DEVICE_PRIMARY_DEVICE |
                                                DISPLAY_DEVICE_ACTIVE))
                {
                    primaryScreen = MakeShared<WindowsScreen>(device.DeviceName);
                    break;
                }
            }
        });

        return primaryScreen;
    }

    DisplayManager* DisplayManager::Initialize(const DisplayManagerSpecifications& specs)
    {
        KITSUNE_UNUSED(specs);

        if (s_Instance != nullptr)
            return s_Instance;

        s_Instance = Memory::New<WindowsDisplayManager>();
        return s_Instance;
    }

    void DisplayManager::Shutdown()
    {
        Memory::Delete(s_Instance);
        s_Instance = nullptr;
    }
}
