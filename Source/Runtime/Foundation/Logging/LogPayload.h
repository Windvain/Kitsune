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
        inline LogPayload(StringView message, StringView topic,
                          SourceLocation location, LogSeverity severity)
            : Message(message),
              Topic(topic),
              Location(Move(location)),
              Severity(severity)
        {
        }

    public:
        String Message;
        String Topic;

        /* TODO: Maybe add time? */

        SourceLocation Location;
        LogSeverity Severity;
    };
}
