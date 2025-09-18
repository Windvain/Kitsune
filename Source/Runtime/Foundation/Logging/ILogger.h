#pragma once

#include "Foundation/Logging/LogPayload.h"

namespace Kitsune
{
    class ILogger
    {
    public:
        virtual ~ILogger() { /* ... */ }

    public:
        virtual void Log(const LogPayload& payload) = 0;
    };
}
