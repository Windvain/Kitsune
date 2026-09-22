#include "Display/Windows/WindowsDisplay.h"
#include "Foundation/Logging/Logger.h"

namespace Kitsune
{
    void WindowsDisplay::SetOrientation(DisplayOrientation orientation)
    {
        KITSUNE_ENGINE_WARN(
            Display,
            "Tried to call SetDisplayOrientation() on a desktop operating system. This "
            "function is meant for applications on mobile.");

        DEVMODEW deviceMode{ /* ... */ };
        deviceMode.dmSize = sizeof(deviceMode);
        deviceMode.dmFields = DM_DISPLAYORIENTATION;

        switch (orientation)
        {
        case DisplayOrientation::Rotated90:
            deviceMode.dmDisplayOrientation = DMDO_90;
            break;
        case DisplayOrientation::Rotated180:
            deviceMode.dmDisplayOrientation = DMDO_180;
            break;
        case DisplayOrientation::Rotated270:
            deviceMode.dmDisplayOrientation = DMDO_270;
            break;
        case DisplayOrientation::Default:
            deviceMode.dmDisplayOrientation = DMDO_DEFAULT;
            break;
        }

        LONG result = ::ChangeDisplaySettingsExW(
            m_DeviceName.Raw(), &deviceMode, nullptr, CDS_RESET, nullptr);

        if (result != DISP_CHANGE_SUCCESSFUL)
        {
            KITSUNE_ENGINE_ERROR(
                Display,
                "Failed to set the orientation of the specified display.");
        }
    }

    DEVMODEW WindowsDisplay::GetDeviceMode() const
    {
        DEVMODEW deviceMode{ /* ... */ };
        deviceMode.dmSize = sizeof(deviceMode);
        deviceMode.dmDisplayFrequency = 60;

        BOOL result = ::EnumDisplaySettingsExW(
            m_DeviceName.Raw(), ENUM_CURRENT_SETTINGS, &deviceMode, 0);

        if (!result)
        {
            // Do not crash here, because afaik displays are volatile; they can be
            // connected when checking whether the display is valid, but not connected
            // when the display settings are being queried.
            KITSUNE_ENGINE_WARN(
                Display,
                "Failed to retrieve settings for the specified display. The function "
                "had been called will return a bogus value.");
        }

        return deviceMode;
    }

    HMONITOR WindowsDisplay::GetMonitorHandle() const
    {
        struct MonitorEnumData
        {
            WideStringView DeviceName;
            HMONITOR Handle;
        };

        MonitorEnumData data = {
            .DeviceName = m_DeviceName,
            .Handle = nullptr
        };

        const auto enumProcedure =
            [](HMONITOR monitor, HDC, LPRECT, LPARAM lparam) -> BOOL
            {
                MONITORINFOEXW monitorInfo;
                monitorInfo.cbSize = sizeof(monitorInfo);

                if (!::GetMonitorInfoW(monitor, &monitorInfo))
                    return TRUE;

                auto* data = reinterpret_cast<MonitorEnumData*>(lparam);
                if (data->DeviceName == monitorInfo.szDevice)
                {
                    data->Handle = monitor;
                    return FALSE;
                }

                return TRUE;
            };

        ::EnumDisplayMonitors(
            nullptr, nullptr, enumProcedure, reinterpret_cast<LPARAM>(&data));

        return data.Handle;
    }
}
