#include "ApplicationCore/Windows/WindowsMonitor.h"
#include "Foundation/Windows/StringConversions.h"

#include "ApplicationCore/BadMonitorCreationException.h"

namespace Kitsune
{
    WindowsMonitor::WindowsMonitor(LPDISPLAY_DEVICEW adapterDevice,
                                   LPDISPLAY_DEVICEW monitorDevice)
    {
        m_MonitorString = Internal::WindowsConvertToUtf8(monitorDevice->DeviceString);
        m_AdapterName = adapterDevice->DeviceName;
    }

    VideoMode WindowsMonitor::GetCurrentVideoMode() const
    {
        DEVMODEW devMode;
        devMode.dmSize = sizeof(devMode);
        devMode.dmDriverExtra = 0;

        if (::EnumDisplaySettingsW(m_AdapterName.Data(), ENUM_CURRENT_SETTINGS, &devMode) == 0)
            throw BadMonitorCreationException();

        return VideoMode(devMode.dmBitsPerPel,
                         Vector2<Uint32>(devMode.dmPelsWidth, devMode.dmPelsHeight),
                         devMode.dmDisplayFrequency);
    }
}
