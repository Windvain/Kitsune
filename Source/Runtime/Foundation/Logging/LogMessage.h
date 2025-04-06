#pragma once

#include "Foundation/String/StringView.h"
#include "Foundation/Diagnostics/SourceLocation.h"

namespace Kitsune
{
    enum class LogSeverity
    {
        Trace, Info,
        Warning, Error, Fatal
    };

    class LogMessage
    {
    public:
        LogMessage() = default;
        LogMessage(const StringView message, const StringView loggerName,
                   SourceLocation loc, LogSeverity severity)
            : Message(message), LoggerName(loggerName),
              Location(Move(loc)), Severity(severity)
        {
        }

    public:
        StringView Message;
        StringView LoggerName;

        SourceLocation Location;
        LogSeverity Severity;
    };
}
