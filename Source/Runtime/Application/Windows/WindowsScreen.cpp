#include "Application/Windows/WindowsScreen.h"
#include <ShellScalingApi.h>

#include "Foundation/Logging/GlobalLog.h"
#include "Foundation/String/TranscodePresets.h"

namespace Kitsune
{
    WindowsScreen::WindowsScreen(WideStringView deviceName)
        : m_DeviceName(deviceName)
    {
    }

    String WindowsScreen::GetName() const
    {
        return Utf16ToUtf8<wchar_t, char>(m_DeviceName);
    }

    Vector2<Uint32> WindowsScreen::GetSize() const
    {
        DEVMODEW deviceMode;
        if (!GetDeviceMode_(&deviceMode))
        {
            KITSUNE_ENGINE_ERROR_FORMAT(
                DisplayManager,
                "Failed to get the size of \"{0}\". Returning (0, 0) as a "
                "fallback value.",
                GetName());

            return { 0, 0 };
        }

        return { deviceMode.dmPelsWidth, deviceMode.dmPelsHeight };
    }

    Vector2<Int32> WindowsScreen::GetPosition() const
    {
        DEVMODEW deviceMode;
        if (!GetDeviceMode_(&deviceMode))
        {
            KITSUNE_ENGINE_ERROR_FORMAT(
                DisplayManager,
                "Failed to get the virtual position of \"{0}\". Returning "
                "(0, 0) as a fallback value.",
                GetName());

            return { 0, 0 };
        }

        return { deviceMode.dmPosition.x, deviceMode.dmPosition.y };
    }

    Uint32 WindowsScreen::GetRefreshRate() const
    {
        DEVMODEW deviceMode;
        if (!GetDeviceMode_(&deviceMode))
        {
            KITSUNE_ENGINE_ERROR_FORMAT(
                DisplayManager,
                "Failed to get the refresh rate of \"{0}\". Returning 60Hz "
                "as a fallback.",
                GetName());

            return 60;
        }

        // A dmDisplayFrequency value of 0 or 1 represent the display hardware's
        // default refresh rate.
        DWORD refreshRate = deviceMode.dmDisplayFrequency;
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
        // The values of *dpiX and *dpiY are identical.
        UINT dpiX, dpiY_;
        HRESULT result = ::GetDpiForMonitor(
            GetMonitorHandle_(), MDT_EFFECTIVE_DPI, &dpiX, &dpiY_);

        if (FAILED(result))
        {
            KITSUNE_ENGINE_ERROR_FORMAT(
                DisplayManager,
                "Failed to get the DPI of \"{0}\". Using USER_DEFAULT_SCREEN_DPI "
                "(96 DPI, 100% scaling) as a fallback value.",
                GetName());

            dpiX = USER_DEFAULT_SCREEN_DPI;
        }

        return dpiX;
    }

    ScreenOrientation WindowsScreen::GetOrientation() const
    {
        DEVMODEW deviceMode;
        if (!GetDeviceMode_(&deviceMode))
        {
            KITSUNE_ENGINE_ERROR_FORMAT(
                DisplayManager,
                "Failed to get orientation of \"{0}\". Returning "
                "ScreenOrientation::Default.",
                GetName());

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

        if (!SetDeviceMode_(&deviceMode))
        {
            KITSUNE_ENGINE_ERROR_FORMAT(
                DisplayManager,
                "Failed to set the size of \"{0}\" to ({1}x{2}).",
                GetName(),
                size.X, size.Y);
        }
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
        default:
            KITSUNE_UNREACHABLE();
        }

        DEVMODEW deviceMode;
        deviceMode.dmSize = sizeof(deviceMode);
        deviceMode.dmDriverExtra = 0;

        deviceMode.dmDisplayOrientation = windowsOrientation;
        deviceMode.dmFields = DM_DISPLAYORIENTATION;

        if (!SetDeviceMode_(&deviceMode))
        {
            KITSUNE_ENGINE_ERROR_FORMAT(
                DisplayManager,
                "Failed to set the orientation of \"{0}\".",
                GetName());
        }
    }

    bool WindowsScreen::GetDeviceMode_(DEVMODEW* deviceMode) const
    {
        ZeroMemory(deviceMode, sizeof(DEVMODEW));
        deviceMode->dmSize = sizeof(DEVMODEW);
        deviceMode->dmDriverExtra = 0;

        return ::EnumDisplaySettingsExW(
            m_DeviceName.Raw(), ENUM_CURRENT_SETTINGS, deviceMode, 0);
    }

    bool WindowsScreen::SetDeviceMode_(DEVMODEW* deviceMode)
    {
        LONG result = ::ChangeDisplaySettingsExW(
            m_DeviceName.Raw(), deviceMode, nullptr, CDS_RESET, nullptr);

        return ((result == DISP_CHANGE_SUCCESSFUL) ||
                (result == DISP_CHANGE_RESTART));
    }

    HMONITOR WindowsScreen::GetMonitorHandle_() const
    {
        // There is currently no way of getting an HMONITOR from a
        // DISPLAY_DEVICE directly, so we have to enumerate through all
        // the connected monitors and check for a monitor with the
        // correct name.
        //
        // GetMonitorHandle_() is not cached here because all monitor handles are
        // invalidated as soon as a new display device is connected/disconnected.
        MonitorEnumProcData_ data;
        data.DeviceName = m_DeviceName;

        BOOL result = ::EnumDisplayMonitors(
            nullptr, nullptr, &MonitorEnumProcedure_,
            reinterpret_cast<LPARAM>(&data));

        if (!result || data.Handle == nullptr)
        {
            KITSUNE_ENGINE_ERROR_FORMAT(
                DisplayManager,
                "Failed to obtain a handle to a monitor from its device "
                "name. The monitor {0} might have been disconnected.",
                this);

            return nullptr;
        }

        return data.Handle;
    }

    BOOL WindowsScreen::MonitorEnumProcedure_(
        HMONITOR monitor, HDC device, LPRECT rect, LPARAM lparam)
    {
        KITSUNE_UNUSED(device);
        KITSUNE_UNUSED(rect);

        auto* data = reinterpret_cast<MonitorEnumProcData_*>(lparam);
        MONITORINFOEXW monitorInfo;

        monitorInfo.cbSize = sizeof(MONITORINFOEXW);
        if (::GetMonitorInfoW(monitor, &monitorInfo) == 0)
            return TRUE;

        if (monitorInfo.szDevice == data->DeviceName)
        {
            data->Handle = monitor;
            return FALSE;
        }

        return TRUE;
    }
}
