#include "Application/Windows/WindowsScreen.h"
#include <cwchar>

#include <Windows.h>
#include <shellscalingapi.h>

#include "Application/IScreen.h"

#include "Foundation/Maths/Vector2.h"
#include "Foundation/Diagnostics/SystemException.h"

namespace Kitsune
{
    WindowsScreen::WindowsScreen(const DISPLAY_DEVICEW& adapterDevice, const DISPLAY_DEVICEW& monitorDevice)
        : m_AdapterDevice(adapterDevice), m_MonitorDevice(monitorDevice)
    {
        // No way of getting an HMONITOR from a DISPLAY_DEVICE directly, we have to enumerate through
        // all the monitors connected and check for the monitor with the correct name.
        MonitorEnumProcData data{ .MonitorHandle = nullptr, .Name = adapterDevice.DeviceName };
        ::EnumDisplayMonitors(nullptr, nullptr, &MonitorEnumProcedure, reinterpret_cast<LPARAM>(&data));

        m_MonitorHandle = data.MonitorHandle;
    }

    Vector2<Uint32> WindowsScreen::GetSize() const
    {
        DEVMODEW deviceMode = GetDeviceMode();
        return { deviceMode.dmPelsWidth, deviceMode.dmPelsHeight };
    }

    Vector2<Int32> WindowsScreen::GetPosition() const
    {
        DEVMODEW deviceMode = GetDeviceMode();
        return { deviceMode.dmPosition.x, deviceMode.dmPosition.y };
    }

    Uint32 WindowsScreen::GetDotsPerInch() const
    {
        HMODULE shcore = ::LoadLibraryW(L"Shcore.dll");
        if (shcore == nullptr)
            return USER_DEFAULT_SCREEN_DPI;

        using GetDpiForMonitorFunction = HRESULT(*)(HMONITOR, MONITOR_DPI_TYPE, UINT*, UINT*);
        auto getDpiForMonitor = (GetDpiForMonitorFunction)(void*)(::GetProcAddress(shcore, "GetDpiForMonitor"));

        if (getDpiForMonitor == nullptr)
        {
            ::FreeLibrary(shcore);
            return USER_DEFAULT_SCREEN_DPI;
        }

        // The values of *dpiX and *dpiY are identical.
        // You only need to record one of the values to determine the DPI and respond appropriately.
        // https://learn.microsoft.com/en-us/windows/win32/api/shellscalingapi/nf-shellscalingapi-getdpiformonitor
        UINT dpiX, _dpiY;
        if (getDpiForMonitor(m_MonitorHandle, MDT_EFFECTIVE_DPI, &dpiX, &_dpiY) != S_OK)
            dpiX = USER_DEFAULT_SCREEN_DPI;

        ::FreeLibrary(shcore);
        return dpiX;
    }

    ScreenOrientation WindowsScreen::GetOrientation() const
    {
        DEVMODEW deviceMode = GetDeviceMode();
        return (deviceMode.dmPelsWidth < deviceMode.dmPelsHeight) ? ScreenOrientation::Portrait :
                                                                    ScreenOrientation::Landscape;
    }

    float WindowsScreen::GetRefreshRate() const
    {
        DEVMODEW deviceMode = GetDeviceMode();
        DWORD refreshRate = deviceMode.dmDisplayFrequency;

        // "When you call the EnumDisplaySettings function, the dmDisplayFrequency member
        // may return with the value 0 or 1. These values represent the display hardware's default
        // refresh rate."
        // https://learn.microsoft.com/en-us/windows/win32/api/wingdi/ns-wingdi-devmodea#members
        if ((refreshRate == 0) || (refreshRate == 1))
        {
            // Yeah, I'm not writing WMI code just for this.
            // Just assume 60Hz and be done with it.
            return 1.0f / 60;
        }

        return 1.0f / deviceMode.dmDisplayFrequency;
    }

    void WindowsScreen::SetOrientation(ScreenOrientation orientation)
    {
        // Do nothing.
        KITSUNE_UNUSED(orientation);
    }

    DEVMODEW WindowsScreen::GetDeviceMode() const
    {
        DEVMODEW deviceMode;
        deviceMode.dmSize = sizeof(deviceMode);
        deviceMode.dmDriverExtra = 0;

        if (::EnumDisplaySettingsExW(m_AdapterDevice.DeviceName, ENUM_CURRENT_SETTINGS, &deviceMode, 0) == 0)
            throw SystemException("Failed to retrieve the adapter device's display settings.");

        return deviceMode;
    }

    BOOL WindowsScreen::MonitorEnumProcedure(HMONITOR monitor, HDC /* device */, LPRECT /* rect */, LPARAM lparam)
    {
        auto& data = *reinterpret_cast<MonitorEnumProcData*>(lparam);
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
