#include "Application/Application.h"

#include <Windows.h>
#include "Foundation/String/UnicodeConversion.h"

#include "ApplicationCore/Windows/WindowsWindow.h"
#include "ApplicationCore/Windows/WindowsMonitor.h"

namespace Kitsune
{
    void Application::PlatformUpdate()
    {
        MSG message;
        if (::PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE) != 0)
        {
            ::TranslateMessage(&message);
            ::DispatchMessageW(&message);
        }
    }

    SharedPtr<IWindow> Application::MakePlatformWindow(const WindowProperties& props)
    {
        VerifyWindowProperties(props);

        WideString wideTitle;
        Unicode::Convert(props.Title.GetBegin(), props.Title.GetEnd(),
                         BackInsertIterator<WideString>(wideTitle));

        return MakeShared<WindowsWindow>(props.Size.x, props.Size.y,
                                         props.Position.x, props.Position.y,
                                         wideTitle.Raw(), props.State, props.Flags);
    }

    Array<SharedPtr<IMonitor>> Application::RetrieveMonitors()
    {
        Array<SharedPtr<IMonitor>> monitors;
        for (DWORD adapterIndex = 0; /* ... */; ++adapterIndex)
        {
            DISPLAY_DEVICEW adapterDevice;
            adapterDevice.cb = sizeof(adapterDevice);

            if (::EnumDisplayDevicesW(nullptr, adapterIndex, &adapterDevice, 0) == 0)
                break;

            if (!(adapterDevice.StateFlags & (DISPLAY_DEVICE_ACTIVE | DISPLAY_DEVICE_PRIMARY_DEVICE)))
                continue;

            for (DWORD monitorIndex = 0; /* ... */; ++monitorIndex)
            {
                DISPLAY_DEVICEW monitorDevice;
                monitorDevice.cb = sizeof(monitorDevice);

                if (::EnumDisplayDevices(adapterDevice.DeviceName, monitorIndex, &monitorDevice, 0) == 0)
                    break;

                if (!(monitorDevice.StateFlags & DISPLAY_DEVICE_ACTIVE))
                    continue;

                monitors.PushBack(MakeScoped<WindowsMonitor>(adapterDevice, monitorDevice));
            }
        }

        return monitors;
    }
}
