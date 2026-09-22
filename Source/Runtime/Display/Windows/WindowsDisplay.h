#pragma once

#include <Windows.h>
#include <ShellScalingApi.h>

#include "Display/Display.h"

#include "Foundation/String/String.h"
#include "Foundation/String/TranscodePresets.h"

namespace Kitsune
{
    class WindowsDisplay : public Display
    {
    public:
        inline explicit WindowsDisplay(WideStringView deviceName)
            : m_DeviceName(deviceName)
        {
        }

    public:
        [[nodiscard]]
        inline WideStringView GetDeviceName() const
        {
            return m_DeviceName;
        }

        [[nodiscard]]
        inline String GetDeviceString() const
        {
            return UTF16ToUTF8<wchar_t, char>(m_DeviceName);
        }

    public:
        [[nodiscard]]
        inline Vector2<Uint32> GetSize() const override
        {
            DEVMODEW deviceMode = GetDeviceMode();
            return { deviceMode.dmPelsWidth, deviceMode.dmPelsHeight };
        }

        [[nodiscard]]
        inline Vector2<Int32> GetPosition() const override
        {
            DEVMODEW deviceMode = GetDeviceMode();
            return { deviceMode.dmPosition.x, deviceMode.dmPosition.y };
        }

        [[nodiscard]]
        inline Uint32 GetRefreshRate() const override
        {
            DEVMODEW deviceMode = GetDeviceMode();
            DWORD refreshRate = deviceMode.dmDisplayFrequency;

            if ((refreshRate == 0) || (refreshRate == 1))
            {
                // 60Hz seems like a safe option here, I'm not writing WMI
                // code specifically for this.
                refreshRate = 60;
            }

            return refreshRate;
        }

        [[nodiscard]]
        inline float GetScaling() const override
        {
            HMONITOR monitor = GetMonitorHandle();
            if (monitor == nullptr)
                return 1.0f;

            UINT dpiX, dpiY;
            if (FAILED(::GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY)))
                dpiX = USER_DEFAULT_SCREEN_DPI;

            return (float(dpiX) / USER_DEFAULT_SCREEN_DPI);
        }

    public:
        [[nodiscard]]
        inline DisplayOrientation GetOrientation() const override
        {
            DEVMODEW deviceMode = GetDeviceMode();
            switch (deviceMode.dmDisplayOrientation)
            {
            case DMDO_90:
                return DisplayOrientation::Rotated90;
            case DMDO_180:
                return DisplayOrientation::Rotated180;
            case DMDO_270:
                return DisplayOrientation::Rotated270;
            case DMDO_DEFAULT:
                return DisplayOrientation::Default;
            }

            KITSUNE_UNREACHABLE();
        }

        KITSUNE_API void SetOrientation(DisplayOrientation orientation) override;

    private:
        [[nodiscard]] DEVMODEW GetDeviceMode() const;
        [[nodiscard]] HMONITOR GetMonitorHandle() const;

    private:
        WideString m_DeviceName;
    };
}
