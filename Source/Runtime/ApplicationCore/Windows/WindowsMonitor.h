#pragma once

#include <Windows.h>
#include "ApplicationCore/IMonitor.h"

namespace Kitsune
{
    class WindowsMonitor : public IMonitor
    {
    public:
        KITSUNE_API_ WindowsMonitor(LPDISPLAY_DEVICEW adapterDevice,
                                    LPDISPLAY_DEVICEW monitorDevice);

    public:
        String GetMonitorName() const override { return m_MonitorString; }
        KITSUNE_API_ VideoMode GetCurrentVideoMode() const override;

    private:
        String m_MonitorString;
        WideString m_AdapterName;
    };
}
