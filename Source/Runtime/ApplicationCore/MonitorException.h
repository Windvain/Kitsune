#pragma once

#include "Foundation/Diagnostics/IException.h"

namespace Kitsune
{
    class MonitorException : public IException
    {
    public:
        inline MonitorException(const char* desc = "Unknown monitor error")
            : IException("MonitorException", desc)
        {
        }
    };
}
