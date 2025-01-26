#pragma once

#include "Foundation/String/StringView.h"
#include "Foundation/Logging/LogMessage.h"

namespace Kitsune
{
    class ILoggerSink
    {
    public:
        virtual ~ILoggerSink() { /* ... */ }
        virtual void Log(const LogMessage& message) = 0;

    public:
        virtual void Flush() { /* ... */ }
    };
}
