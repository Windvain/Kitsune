#pragma once

#include "Foundation/String/String.h"
#include "Foundation/Diagnostics/SourceLocation.h"

namespace Kitsune
{
    // The severity of the log message/payload.
    enum class LogSeverity
    {
        Trace,
        Info,
        Warning,
        Error,
        Fatal
    };

    // Contains all information that is needed by a logger about a log request.
    class LogPayload
    {
    public:
        LogPayload() = default;
        inline LogPayload(const StringView message, const StringView loggerName,
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
