#pragma once

#include "Foundation/String/String.h"
#include "Foundation/Diagnostics/SourceLocation.h"

namespace Kitsune
{
    enum class LogSeverity
    {
        Trace, Info,
        Warning, Error, Fatal
    };

    class LogPayload
    {
    public:
        LogPayload() = default;
        LogPayload(const StringView message, const StringView loggerName,
                   SourceLocation loc, LogSeverity severity)
            : Message(message), LoggerName(loggerName),
              Location(Move(loc)), Severity(severity)
        {
        }

    public:
        String Message;
        String LoggerName;

        SourceLocation Location;
        LogSeverity Severity;
    };
}
