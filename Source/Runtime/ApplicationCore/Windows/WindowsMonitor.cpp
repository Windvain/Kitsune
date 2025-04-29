#include "ApplicationCore/Windows/WindowsMonitor.h"
#include "ApplicationCore/MonitorException.h"

#include <cwchar>
#include "Foundation/Windows/StringConversions.h"

namespace Kitsune
{
    struct MonitorEnumProcData
    {
        HMONITOR MonitorHandle;
        const wchar_t* Name;
    };

    WindowsMonitor::WindowsMonitor(const DISPLAY_DEVICEW& adapter, const DISPLAY_DEVICEW& monitor)
        : m_Device(adapter), m_Name(Details::WindowsConvertToUtf8(monitor.DeviceString))
    {
        MonitorEnumProcData data{ .MonitorHandle = nullptr, .Name = adapter.DeviceName };
        ::EnumDisplayMonitors(nullptr, nullptr, &MonitorEnumProcedure, reinterpret_cast<LPARAM>(&data));

        m_Monitor = data.MonitorHandle;
    }

    Vector2<Int32> WindowsMonitor::GetVirtualPosition() const
    {
        DEVMODEW devMode = GetDeviceMode();
        return { devMode.dmPosition.x, devMode.dmPosition.y };
    }

    VideoMode WindowsMonitor::GetVideoMode() const
    {
        DEVMODEW devMode = GetDeviceMode();
        Vector2<Uint32> size = { devMode.dmPelsWidth, devMode.dmPelsHeight };

        return VideoMode(devMode.dmBitsPerPel, size, devMode.dmDisplayFrequency);
    }

    void WindowsMonitor::SetVideoMode(const VideoMode& videoMode)
    {
        DEVMODEW devMode;
        devMode.dmSize = sizeof(DEVMODEW);
        devMode.dmDriverExtra = 0;

        devMode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT |
            DM_DISPLAYFREQUENCY;

        devMode.dmBitsPerPel = videoMode.BitsPerPixel;
        devMode.dmPelsWidth = videoMode.Resolution.x;
        devMode.dmPelsHeight = videoMode.Resolution.y;
        devMode.dmDisplayFrequency = videoMode.RefreshRate;

        LONG result = ::ChangeDisplaySettingsW(&devMode, 0);
        if ((result != DISP_CHANGE_SUCCESSFUL) && (result != DISP_CHANGE_RESTART))
            throw MonitorException(GetResultDescription(result));
    }

    bool WindowsMonitor::IsPrimaryMonitor() const
    {
        MONITORINFO monitorInfo;
        monitorInfo.cbSize = sizeof(MONITORINFO);

        if (::GetMonitorInfoW(m_Monitor, &monitorInfo) == 0)
            return false;

        return ((monitorInfo.dwFlags & MONITORINFOF_PRIMARY) == MONITORINFOF_PRIMARY);
    }

    DEVMODEW WindowsMonitor::GetDeviceMode() const
    {
        DEVMODEW devMode;
        devMode.dmSize = sizeof(DEVMODEW);
        devMode.dmDriverExtra = 0;

        if (::EnumDisplaySettingsExW(m_Device.DeviceName, ENUM_CURRENT_SETTINGS, &devMode, 0) == 0)
            throw MonitorException("Failed to retrieve monitor settings.");

        return devMode;
    }

    const char* WindowsMonitor::GetResultDescription(LONG result) const
    {
        switch (result)
        {
        case DISP_CHANGE_BADDUALVIEW: return "Failed to change display settings because the "
                                             "system is DualView capable.";
        case DISP_CHANGE_BADFLAGS:    return "Invalid flags were passed in.";
        case DISP_CHANGE_BADMODE:     return "The graphics mode is not supported.";
        case DISP_CHANGE_BADPARAM:    return "An invalid parameter was passed in.";
        case DISP_CHANGE_FAILED:      return "Failed to change display settings.";
        case DISP_CHANGE_NOTUPDATED:  return "Couldn't write settings to the registry.";
        default:
            KITSUNE_UNREACHABLE();
        };
    }

    BOOL WindowsMonitor::MonitorEnumProcedure(HMONITOR monitor, HDC /* device */, LPRECT /* rect */, LPARAM untypedData)
    {
        auto& data = *reinterpret_cast<MonitorEnumProcData*>(untypedData);
        MONITORINFOEXW monitorInfo;

        monitorInfo.cbSize = sizeof(MONITORINFOEXW);
        if (::GetMonitorInfoW(monitor, &monitorInfo) == 0)
            return TRUE;

        if (std::wcscmp(monitorInfo.szDevice, data.Name) == 0)
        {
            data.MonitorHandle = monitor;
            return FALSE;
        }

        return TRUE;
    }
}
