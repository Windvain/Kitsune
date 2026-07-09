#include "Display/Windows/WindowsDisplayManager.h"

#include <ShellScalingApi.h>
#include "Foundation/Logging/Logger.h"

#include "Foundation/Diagnostics/SystemException.h"
#include "Foundation/Diagnostics/InvalidArgumentException.h"

namespace Kitsune
{
    struct MonitorEnumerationData
    {
        _Out_ HMONITOR Handle;
        _In_ WCHAR* DeviceName;
    };

    void WindowsDisplayManager::Update(double delta)
    {
        // HACK:
        // Turns out, neither HMONITOR nor DeviceName/szDevice persist when a monitor is
        // connected or disconnected.
        // Every single handle (HMONITOR) gets invalidated if the display changes.
        // The device names get shuffled around if the display changes.
        // I have no idea how else to approach this problem (maybe with EDIDs?) so this
        // is a half-baked solution y'all.

        KITSUNE_UNUSED(delta);
        DWORD index = 0;

        DISPLAY_DEVICE displayDevice;
        displayDevice.cb = sizeof(displayDevice);

        while (::EnumDisplayDevicesW(nullptr, index++, &displayDevice, 0))
        {
            if (!(displayDevice.StateFlags & DISPLAY_DEVICE_ACTIVE))
                continue;

            auto iter = Algorithms::FindIf(m_Displays.GetBegin(), m_Displays.GetEnd(),
                [&](const ScopedPtr<WindowsDisplay>& display)
                {
                    return (std::wcscmp(
                        display->DeviceName, displayDevice.DeviceName) == 0);
                });

            if (iter == m_Displays.GetEnd())
            {
                ScopedPtr<WindowsDisplay> display = MakeScoped<WindowsDisplay>();
                std::memcpy(
                    display->DeviceName,
                    displayDevice.DeviceName,
                    32 * sizeof(wchar_t));

                KITSUNE_INFO_FORMAT(
                    "A display with ID {0} has been connected.",
                    display.Get());

                m_OldDisplays.PushBack(Move(display));
            }
            else
            {
                if (displayDevice.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE)
                    m_OldDisplays.Insert(m_OldDisplays.GetBegin(), Move(*iter));
                else
                    m_OldDisplays.PushBack(Move(*iter));

                m_Displays.RemoveUnsorted(iter);
            }
        }

        for (ScopedPtr<WindowsDisplay>& disconnected : m_Displays)
        {
            KITSUNE_INFO_FORMAT(
                "A display with ID {0} has been disconnected.",
                disconnected.Get());
        }

        m_Displays.Clear();
        Swap(m_Displays, m_OldDisplays);
    }

    DisplayInformation WindowsDisplayManager::GetDisplayInformation(
        DisplayID displayID) const
    {
        auto* displayPointer = static_cast<WindowsDisplay*>(displayID);
        auto iter = Algorithms::Find(
            m_Displays.GetBegin(), m_Displays.GetEnd(), displayPointer);

        if (iter == m_Displays.GetEnd())
            throw InvalidArgumentException("The display ID given was invalid.");

        DEVMODEW deviceMode;
        ::ZeroMemory(&deviceMode, sizeof(deviceMode));

        deviceMode.dmSize = sizeof(deviceMode);
        if (!::EnumDisplaySettingsExW((*iter)->DeviceName, ENUM_CURRENT_SETTINGS,
                                      &deviceMode, 0))
        {
            throw SystemException("Failed to retrieve the display's settings.");
        }

        DisplayInformation displayInfo;
        displayInfo.Size = { deviceMode.dmPelsWidth, deviceMode.dmPelsHeight };
        displayInfo.Position = { deviceMode.dmPosition.x, deviceMode.dmPosition.y };

        // Index 0 will always contain the main display.
        displayInfo.MainDisplay = (m_Displays[0].Get() == displayPointer);

        // A dmDisplayFrequency value of 0 or 1 represent the display hardware's
        // default refresh rate.
        DWORD refreshRate = deviceMode.dmDisplayFrequency;
        if ((refreshRate != 0) && (refreshRate != 1))
            displayInfo.RefreshRate = refreshRate;
        else
        {
            // 60Hz seems like a safe option here, I'm not writing WMI
            // code specifically for this.
            displayInfo.RefreshRate = 60;
        }

        MonitorEnumerationData data{ /* ... */ };
        data.DeviceName = (*iter)->DeviceName;

        KITSUNE_UNUSED(::EnumDisplayMonitors(
            nullptr,
            nullptr,
            WindowsDisplayManager::MonitorEnumerationProc,
            reinterpret_cast<LPARAM>(&data)));

        // The values of *dpiX and *dpiY are identical, we're going to use dpiX.
        UINT dpiX, dpiY;
        if (FAILED(::GetDpiForMonitor(data.Handle, MDT_EFFECTIVE_DPI, &dpiX, &dpiY)))
            dpiX = USER_DEFAULT_SCREEN_DPI;

        displayInfo.Scaling = (float(dpiX) / 96.0f);
        switch (deviceMode.dmDisplayOrientation)
        {
        case DMDO_90:
            displayInfo.Orientation = DisplayOrientation::Rotated90;
            break;
        case DMDO_180:
            displayInfo.Orientation = DisplayOrientation::Rotated180;
            break;
        case DMDO_270:
            displayInfo.Orientation = DisplayOrientation::Rotated270;
            break;
        case DMDO_DEFAULT:
            displayInfo.Orientation = DisplayOrientation::Default;
        }

        return displayInfo;
    }

    void WindowsDisplayManager::SetDisplayOrientation(
        DisplayID displayID,
        DisplayOrientation orientation)
    {
        KITSUNE_ENGINE_WARN(
            Display,
            "Tried to call SetDisplayOrientation() on a desktop operating system. This "
            "function is meant for applications on mobile.");

        DEVMODEW deviceMode;
        ::ZeroMemory(&deviceMode, sizeof(deviceMode));

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

        auto* displayPointer = static_cast<WindowsDisplay*>(displayID);
        auto iter = Algorithms::Find(
            m_Displays.GetBegin(), m_Displays.GetEnd(), displayPointer);

        if (iter == m_Displays.GetEnd())
            throw InvalidArgumentException("The display ID given was invalid.");

        LONG result = ::ChangeDisplaySettingsExW(
            (*iter)->DeviceName, &deviceMode, nullptr, CDS_RESET, nullptr);

        if (result != DISP_CHANGE_SUCCESSFUL)
            throw SystemException("Failed to change the orientation of the screen.");
    }

    BOOL WindowsDisplayManager::MonitorEnumerationProc(
        HMONITOR monitor, HDC deviceContext, LPRECT rect, LPARAM lparam)
    {
        KITSUNE_UNUSED(deviceContext);
        KITSUNE_UNUSED(rect);

        MONITORINFOEXW monitorInfo;
        monitorInfo.cbSize = sizeof(monitorInfo);

        if (!::GetMonitorInfoW(monitor, &monitorInfo))
            throw SystemException("Failed to get monitor info.");

        auto* data = reinterpret_cast<MonitorEnumerationData*>(lparam);
        if (std::wcscmp(data->DeviceName, monitorInfo.szDevice) == 0)
        {
            data->Handle = monitor;
            return FALSE;
        }

        return TRUE;
    }
}
