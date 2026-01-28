#pragma once

#include "Foundation/Logging/LogPayload.h"

namespace Kitsune
{
    class Logger
    {
    public:
        virtual ~Logger() { /* ... */ }

    public:
        virtual void Log(const LogPayload& payload) = 0;
    };
}
