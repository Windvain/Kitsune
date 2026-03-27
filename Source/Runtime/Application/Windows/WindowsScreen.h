#pragma once

#include <Windows.h>

#include "Application/Screen.h"
#include "Foundation/String/String.h"

namespace Kitsune
{
    class WindowsScreen : public Screen
    {
    public:
        WindowsScreen(WideStringView deviceName);
        ~WindowsScreen() override = default;

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
        [[nodiscard]]
        inline WideStringView GetDeviceName() const
        {
            return m_DeviceName;
        }

    private:
        [[nodiscard]] HMONITOR GetMonitorHandle_() const;

        bool GetDeviceMode_(DEVMODEW* deviceMode) const;
        bool SetDeviceMode_(DEVMODEW* deviceMode);

    private:
        struct MonitorEnumProcData_
        {
            HMONITOR Handle = nullptr;
            WideStringView DeviceName;
        };

        static BOOL CALLBACK MonitorEnumProcedure_(
            HMONITOR monitor, HDC device, LPRECT rect,
            LPARAM lparam);

    private:
        WideString m_DeviceName;
    };
}
