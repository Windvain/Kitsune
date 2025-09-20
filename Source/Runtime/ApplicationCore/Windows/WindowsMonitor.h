#pragma once

#include <Windows.h>
#include "Foundation/Common/Macros.h"

#include "ApplicationCore/IMonitor.h"

namespace Kitsune
{
    class WindowsMonitor : public IMonitor
    {
    public:
        WindowsMonitor(const DISPLAY_DEVICEW& adapter, const DISPLAY_DEVICEW& monitor);

    public:
        Vector2<Int32> GetVirtualPosition() const override;
        inline String GetName() const override { return m_Name; }

        VideoMode GetVideoMode() const override;
        void SetVideoMode(const VideoMode& videoMode) override;

        bool IsPrimaryMonitor() const override;

    private:
        DEVMODEW GetDeviceMode() const;
        const char* GetResultDescription(LONG result) const;

        static BOOL MonitorEnumProcedure(HMONITOR monitor, HDC device, LPRECT rect, LPARAM data);

    private:
        DISPLAY_DEVICEW m_Device;
        String m_Name;

        HMONITOR m_Monitor;
    };
}
