#pragma once

#include "Foundation/Logging/ILoggerSink.h"

namespace Kitsune
{
    class ConsoleLoggerSink : public ILoggerSink
    {
    public:
        ConsoleLoggerSink() { /* ... */ }
        virtual ~ConsoleLoggerSink() { /* ... */ }

    public:
        KITSUNE_API_ void Log(const LogMessage& message) override;
        KITSUNE_API_ void Flush() override;
    };
}
