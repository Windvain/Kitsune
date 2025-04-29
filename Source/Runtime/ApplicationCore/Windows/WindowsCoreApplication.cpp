#include "ApplicationCore/CoreApplication.h"

#include <cwchar>
#include <Windows.h>

#include "Foundation/String/Format.h"
#include "Foundation/Algorithms/ForEach.h"
#include "Foundation/Diagnostics/Assert.h"
#include "Foundation/Windows/StringConversions.h"

#include "ApplicationCore/Null/NullWindow.h"
#include "ApplicationCore/Null/NullMonitor.h"

#include "ApplicationCore/Windows/WindowsWindow.h"
#include "ApplicationCore/Windows/WindowsMonitor.h"

namespace Kitsune
{
    CoreApplication::CoreApplication(const ApplicationSpecs& specs)
        : m_ApplicationSpecs(specs)
    {
        /* Enumerate through all of the connected monitors */
        // Headless builds shouldn't have a monitor, just use one that no-ops..
        if (m_ApplicationSpecs.Headless)
        {
            VideoMode videoMode = VideoMode(32, { 1920, 1080 }, 60);
            m_Monitors.PushBack(MakeScoped<NullMonitor>(videoMode, true));

            return;
        }

        // TODO: This enumerates through all of the monitors on startup. Monitors connected after
        //       the application has started running will not be registered.
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

                m_Monitors.PushBack(MakeScoped<WindowsMonitor>(adapterDevice, monitorDevice));
            }
        }
    }

    CoreApplication::~CoreApplication()
    {
    }

    void CoreApplication::PlatformExit(int exitCode)
    {
        m_ExitRequested = true;
        m_ExitCode = exitCode;

        ::PostQuitMessage(exitCode);
    }

    void CoreApplication::PlatformForceExit(int exitCode)
    {
        PlatformExit(exitCode);
        ::TerminateProcess(::GetCurrentProcess(), static_cast<UINT>(exitCode));
    }

    void CoreApplication::MakeWindow(const WindowProperties& props)
    {
        KITSUNE_ASSERT(m_PrimaryWindow == nullptr, "A window has already been created.");
        VerifyWindowProperties(props);

        ScopedPtr<IWindow> window;

        if (m_ApplicationSpecs.Headless)
            window = MakeScoped<NullWindow>(props);
        else
        {
            WideString wideTitle = Details::WindowsConvertToUtf16(props.Title);
            window = MakeScoped<WindowsWindow>(props.Size.x, props.Size.y,
                                               props.Position.x, props.Position.y,
                                               wideTitle.Raw(), props.State, props.Flags);
        }

        m_PrimaryWindow = Move(window);
    }

    void CoreApplication::PlatformUpdate()
    {
        MSG message;
        if (::PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE) != 0)
        {
            ::TranslateMessage(&message);
            ::DispatchMessageW(&message);
        }
    }
}
