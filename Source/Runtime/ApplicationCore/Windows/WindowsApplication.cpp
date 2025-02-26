#include "ApplicationCore/Windows/WindowsApplication.h"
#include "ApplicationCore/Windows/WindowsMonitor.h"

#include "ApplicationCore/BadMonitorCreationException.h"

namespace Kitsune
{
    WindowsApplication::WindowsApplication()
    {
    }

    WindowsApplication::~WindowsApplication()
    {
    }

    void WindowsApplication::Exit(int exitCode)
    {
        m_ExitRequested = true;
        m_ExitCode = exitCode;

        ::PostQuitMessage(exitCode);
    }

    void WindowsApplication::ForceExit(int exitCode)
    {
        Exit(exitCode);
        ::TerminateProcess(::GetCurrentProcess(), static_cast<UINT>(exitCode));
    }

    void WindowsApplication::PollEvents()
    {
        MSG message;
        if (::PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE) != 0)
        {
            ::TranslateMessage(&message);
            ::DispatchMessageW(&message);
        }
    }

    SharedPtr<IMonitor> WindowsApplication::AllocatePrimaryMonitor()
    {
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

                if (!(adapterDevice.StateFlags & DISPLAY_DEVICE_ACTIVE))
                    continue;

                return MakeShared<WindowsMonitor>(&adapterDevice, &monitorDevice);
            }
        }

        throw BadMonitorCreationException("No active primary display was found");
    }

    ScopedPtr<IPlatformApplication> IPlatformApplication::CreateApplicationImpl()
    {
        return MakeScoped<WindowsApplication>();
    }
}
