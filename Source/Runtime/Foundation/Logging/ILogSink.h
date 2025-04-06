#pragma once

#include "Foundation/Logging/LogMessage.h"

namespace Kitsune
{
    class ILogSink
    {
    public:
        virtual ~ILogSink() { /* ... */ }

        virtual void Log(const LogMessage& message) = 0;
        virtual void Flush() { /* ... */ }
    };
}
