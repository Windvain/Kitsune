#pragma once

#include <Windows.h>
#include "Application/DisplayManager.h"

namespace Kitsune
{
    class WindowsDisplayManager : public DisplayManager
    {
    public:
        WindowsDisplayManager();
        ~WindowsDisplayManager();

    public:
        void Update() override;

        Array<SharedPtr<Screen>> GetScreens() const override;
        SharedPtr<Screen> GetPrimaryScreen() const override;

    private:
        template<Invocable<const DISPLAY_DEVICEW&> Func>
        inline static void EnumerateMonitors(Func function)
        {
            DISPLAY_DEVICEW adapterDevice;
            adapterDevice.cb = sizeof(adapterDevice);

            for (DWORD adapterIndex = 0; /* ... */; ++adapterIndex)
            {
                if (!::EnumDisplayDevicesW(nullptr, adapterIndex, &adapterDevice, 0))
                    break;

                if (!(adapterDevice.StateFlags & DISPLAY_DEVICE_ACTIVE))
                    continue;

                function(adapterDevice);
            }
        }
    };
}
