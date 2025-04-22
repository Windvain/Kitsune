#pragma once

#include <Windows.h>
#include "Foundation/Common/Macros.h"

#include "ApplicationCore/IMonitor.h"

namespace Kitsune
{
    class WindowsMonitor : public IMonitor
    {
    public:
        KITSUNE_API_ WindowsMonitor(const DISPLAY_DEVICEW& adapter, const DISPLAY_DEVICEW& monitor);

    public:
        KITSUNE_API_ Vector2<Int32> GetVirtualPosition() const override;
        inline String GetName() const override { return m_Name; }

        KITSUNE_API_ VideoMode GetVideoMode() const override;
        KITSUNE_API_ void SetVideoMode(const VideoMode& videoMode) override;

        KITSUNE_API_ bool IsPrimaryMonitor() const override;

    private:
        KITSUNE_API_ DEVMODEW GetDeviceMode() const;
        KITSUNE_API_ const char* GetResultDescription(LONG result) const;

        KITSUNE_API_ static BOOL MonitorEnumProcedure(HMONITOR monitor, HDC device, LPRECT rect, LPARAM data);

    private:
        DISPLAY_DEVICEW m_Device;
        String m_Name;

        HMONITOR m_Monitor;
    };
}
