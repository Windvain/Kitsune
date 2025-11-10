#pragma once

#include <Windows.h>
#include "Application/IScreen.h"

namespace Kitsune
{
    class WindowsScreen : public IScreen
    {
    public:
        WindowsScreen(const DISPLAY_DEVICEW& adapterDevice, const DISPLAY_DEVICEW& monitorDevice);
        ~WindowsScreen() = default;

    public:
        [[nodiscard]] Vector2<Uint32> GetSize() const override;
        [[nodiscard]] Vector2<Int32> GetPosition() const override;

        [[nodiscard]] Uint32 GetDotsPerInch() const override;
        [[nodiscard]] ScreenOrientation GetOrientation() const override;

        [[nodiscard]] float GetRefreshRate() const override;

    public:
        void SetOrientation(ScreenOrientation orientation) override;

    public:
        const wchar_t* GetDeviceId() const { return m_MonitorDevice.DeviceID; }

    private:
        DEVMODEW GetDeviceMode() const;
        static BOOL MonitorEnumProcedure(HMONITOR monitor, HDC device, LPRECT rect, LPARAM lparam);

        // Used for getting data from the monitor enumeration procedure. Look at
        // the WindowsScreen() constructor for more information.
        struct MonitorEnumProcData
        {
            HMONITOR MonitorHandle;
            const wchar_t* Name;
        };

    private:
        DISPLAY_DEVICEW m_AdapterDevice;
        DISPLAY_DEVICEW m_MonitorDevice;

        HMONITOR m_MonitorHandle;
    };
}
