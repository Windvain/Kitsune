#pragma once

#include "ApplicationCore/VideoMode.h"

#include "Foundation/String/String.h"
#include "Foundation/Containers/Array.h"

namespace Kitsune
{
    // Unlike IWindow, we hide the construction APIs behind IPlatformApplication.
    // Makes sure no one tries to construct a monitor class.
    class IMonitor
    {
    public:
        virtual ~IMonitor() { /* ... */ }

    public:
        virtual String GetMonitorName() const = 0;
        virtual VideoMode GetCurrentVideoMode() const = 0;
    };
}
