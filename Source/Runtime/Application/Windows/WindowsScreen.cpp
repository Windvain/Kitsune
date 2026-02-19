#include "Application/Windows/WindowsScreen.h"
#include <ShellScalingApi.h>

#include "Foundation/Logging/GlobalLog.h"
#include "Foundation/Diagnostics/SystemException.h"

namespace Kitsune
{
    WindowsScreen::WindowsScreen(const WideStringView deviceName)
        : m_DeviceName(deviceName), m_MonitorHandle(GetMonitorHandle(deviceName))
    {
    }

    Vector2<Uint32> WindowsScreen::GetSize() const
    {
        DEVMODEW deviceMode;
        if (!GetDeviceMode(&deviceMode))
        {
            KITSUNE_ENGINE_ERROR_FORMAT_("Failed to get size of screen {0}.", this);
            return Vector2<Uint32>();
        }

        return { deviceMode.dmPelsWidth, deviceMode.dmPelsHeight };
    }

    Vector2<Int32> WindowsScreen::GetPosition() const
    {
        DEVMODEW deviceMode;
        if (!GetDeviceMode(&deviceMode))
        {
            KITSUNE_ENGINE_ERROR_FORMAT_("Failed to get position of screen {0}.", this);
            return Vector2<Int32>();
        }

        return { deviceMode.dmPosition.x, deviceMode.dmPosition.y };
    }

    Uint32 WindowsScreen::GetRefreshRate() const
    {
        DEVMODEW deviceMode;
        if (!GetDeviceMode(&deviceMode))
        {
            KITSUNE_ENGINE_ERROR_FORMAT_("Failed to get refresh rate of screen {0}.", this);
            return 0;
        }

        DWORD refreshRate = deviceMode.dmDisplayFrequency;

        // When you call the EnumDisplaySettings function, the dmDisplayFrequency
        // member may return with the value 0 or 1. These values represent the
        // display hardware's default refresh rate.
        //
        // https://learn.microsoft.com/en-us/windows/win32/api/wingdi/ns-wingdi-devmodea#members
        if ((refreshRate == 0) || (refreshRate == 1))
        {
            // 60Hz seems like a safe option here, I'm not writing WMI
            // code specifically for this.
            return 60;
        }

        return deviceMode.dmDisplayFrequency;
    }

    Uint32 WindowsScreen::GetDotsPerInch() const
    {
        HMODULE shcore = ::LoadLibraryW(L"Shcore.dll");
        if (shcore == nullptr)
            return USER_DEFAULT_SCREEN_DPI;

        using GetDpiForMonitorFunction = HRESULT (*)(HMONITOR, MONITOR_DPI_TYPE,
                                                     UINT*, UINT*);

        auto getDpiForMonitor = (GetDpiForMonitorFunction)(void*)(
            ::GetProcAddress(shcore, "GetDpiForMonitor"));

        if (getDpiForMonitor == nullptr)
        {
            ::FreeLibrary(shcore);
            return USER_DEFAULT_SCREEN_DPI;
        }

        // The values of *dpiX and *dpiY are identical.
        // You only need to record one of the values to determine the DPI and
        // respond appropriately.
        //
        // https://learn.microsoft.com/en-us/windows/win32/api/shellscalingapi/nf-shellscalingapi-getdpiformonitor
        UINT dpiX, _dpiY;
        if (getDpiForMonitor(m_MonitorHandle, MDT_EFFECTIVE_DPI, &dpiX, &_dpiY) != S_OK)
        {
            KITSUNE_ENGINE_ERROR_FORMAT_("Failed to get DPI of screen {0}.", this);
            dpiX = USER_DEFAULT_SCREEN_DPI;
        }

        ::FreeLibrary(shcore);
        return dpiX;
    }

    ScreenOrientation WindowsScreen::GetOrientation() const
    {
        DEVMODEW deviceMode;
        if (!GetDeviceMode(&deviceMode))
        {
            KITSUNE_ENGINE_ERROR_FORMAT_("Failed to get orientation of screen {0}.", this);
            return ScreenOrientation::Default;
        }

        switch (deviceMode.dmDisplayOrientation)
        {
        case DMDO_90:  return ScreenOrientation::Rotated90;
        case DMDO_180: return ScreenOrientation::Rotated180;
        case DMDO_270: return ScreenOrientation::Rotated270;
        case DMDO_DEFAULT:
            return ScreenOrientation::Default;
        }

        // Get compiler to shut up.
        KITSUNE_UNREACHABLE();
    }

    void WindowsScreen::SetSize(const Vector2<Uint32>& size)
    {
        DEVMODEW deviceMode;
        deviceMode.dmSize = sizeof(deviceMode);
        deviceMode.dmDriverExtra = 0;

        deviceMode.dmPelsWidth = size.X;
        deviceMode.dmPelsHeight = size.Y;
        deviceMode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;

        if (!SetDeviceMode(&deviceMode))
            KITSUNE_ENGINE_ERROR_FORMAT_("Failed to set screen {0} size.", this);
    }

    void WindowsScreen::SetOrientation(ScreenOrientation orientation)
    {
        DWORD windowsOrientation;
        switch (orientation)
        {
        case ScreenOrientation::Default:
            windowsOrientation = DMDO_DEFAULT;
            break;
        case ScreenOrientation::Rotated90:
            windowsOrientation = DMDO_90;
            break;
        case ScreenOrientation::Rotated180:
            windowsOrientation = DMDO_180;
            break;
        case ScreenOrientation::Rotated270:
            windowsOrientation = DMDO_270;
            break;
        }

        DEVMODEW deviceMode;
        deviceMode.dmSize = sizeof(deviceMode);
        deviceMode.dmDriverExtra = 0;

        deviceMode.dmDisplayOrientation = windowsOrientation;
        deviceMode.dmFields = DM_DISPLAYORIENTATION;

        if (!SetDeviceMode(&deviceMode))
            KITSUNE_ENGINE_ERROR_FORMAT_("Failed to set screen {0} orientation.", this);
    }

    bool WindowsScreen::GetDeviceMode(DEVMODEW* deviceMode) const
    {
        ZeroMemory(deviceMode, sizeof(DEVMODEW));
        deviceMode->dmSize = sizeof(DEVMODEW);
        deviceMode->dmDriverExtra = 0;

        return ::EnumDisplaySettingsExW(m_DeviceName.Raw(), ENUM_CURRENT_SETTINGS,
                                        deviceMode, 0);
    }

    bool WindowsScreen::SetDeviceMode(DEVMODEW* deviceMode)
    {
        LONG result = ::ChangeDisplaySettingsExW(m_DeviceName.Raw(), deviceMode,
                                                 nullptr, CDS_RESET, nullptr);

        return ((result == DISP_CHANGE_SUCCESSFUL) || (result == DISP_CHANGE_RESTART));
    }

    HMONITOR WindowsScreen::GetMonitorHandle(const WideStringView deviceName)
    {
        // There is currently no way of getting an HMONITOR from a DISPLAY_DEVICE
        // directly, so we have to enumerate through all the connected monitors
        // and check for a monitor with the correct name.
        MonitorEnumProcData data;
        data.MonitorHandle = nullptr;
        data.DeviceName = deviceName;

        ::EnumDisplayMonitors(nullptr, nullptr, &MonitorEnumProcedure,
                              reinterpret_cast<LPARAM>(&data));

        if (data.MonitorHandle == nullptr)
        {
            throw SystemException("Failed to obtain a handle to a monitor from "
                                  "its device name.");
        }

        return data.MonitorHandle;
    }

    BOOL WindowsScreen::MonitorEnumProcedure(HMONITOR monitor, HDC /* device */,
                                             LPRECT /* rect */, LPARAM lparam)
    {
        auto* data = reinterpret_cast<MonitorEnumProcData*>(lparam);
        MONITORINFOEXW monitorInfo;

        monitorInfo.cbSize = sizeof(MONITORINFOEXW);
        if (::GetMonitorInfoW(monitor, &monitorInfo) == 0)
            return TRUE;

        if (monitorInfo.szDevice == data->DeviceName)
        {
            data->MonitorHandle = monitor;
            return FALSE;
        }

        return TRUE;
    }
}
