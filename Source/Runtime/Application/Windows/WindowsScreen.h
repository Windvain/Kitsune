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
        [[nodiscard]] String GetName() const override;

    public:
        [[nodiscard]] Vector2<Uint32> GetSize() const override;
        [[nodiscard]] Vector2<Int32> GetPosition() const override;

        [[nodiscard]] Uint32 GetRefreshRate() const override;
        [[nodiscard]] Uint32 GetDotsPerInch() const override;

        [[nodiscard]] ScreenOrientation GetOrientation() const override;

    public:
        void SetSize(const Vector2<Uint32>& size) override;
        void SetOrientation(ScreenOrientation orientation) override;

    public:
        inline WideStringView GetDeviceName() const
        {
            return m_DeviceName;
        }

    private:
        HMONITOR GetMonitorHandle() const;

        bool GetDeviceMode(DEVMODEW* deviceMode) const;
        bool SetDeviceMode(DEVMODEW* deviceMode);

    private:
        struct MonitorEnumProcData
        {
            HMONITOR MonitorHandle;
            WideStringView DeviceName;
        };

        static BOOL CALLBACK MonitorEnumProcedure(HMONITOR monitor, HDC device, LPRECT rect,
                                                  LPARAM lparam);

    private:
        WideString m_DeviceName;
    };
}
