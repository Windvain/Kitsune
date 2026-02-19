#pragma once

#include <Windows.h>

#include "Application/Screen.h"
#include "Foundation/String/String.h"

namespace Kitsune
{
    class WindowsScreen : public Screen
    {
    public:
        WindowsScreen(const WideStringView deviceName);
        ~WindowsScreen() = default;

    public:
        [[nodiscard]] Vector2<Uint32> GetSize() const;
        [[nodiscard]] Vector2<Int32> GetPosition() const;

        [[nodiscard]] Uint32 GetRefreshRate() const;
        [[nodiscard]] Uint32 GetDotsPerInch() const;

        [[nodiscard]]
        ScreenOrientation GetOrientation() const;

    public:
        void SetSize(const Vector2<Uint32>& size);
        void SetOrientation(ScreenOrientation orientation);

    public:
        inline WideStringView GetDeviceName() const
        {
            return m_DeviceName;
        }

    private:
        struct MonitorEnumProcData
        {
            HMONITOR MonitorHandle;
            WideStringView DeviceName;
        };

        static BOOL CALLBACK MonitorEnumProcedure(HMONITOR monitor, HDC device,
                                                  LPRECT rect, LPARAM lparam);

        HMONITOR GetMonitorHandle() const;

        bool GetDeviceMode(DEVMODEW* deviceMode) const;
        bool SetDeviceMode(DEVMODEW* deviceMode);

    private:
        WideString m_DeviceName;
    };
}
