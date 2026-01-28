#include "Foundation/Logging/ConsoleLogger.h"
#include <ctime>        // TODO: Replace this with our own functions.

#include "Foundation/String/Format.h"
#include "Foundation/Logging/WriteStreamIterator.h"

namespace Kitsune
{
    void ConsoleLogger::Log(const LogPayload& payload)
    {
        FormatTo(WriteStreamIterator<char>(m_Stream), "{0}{1}{2}: {3}\x1B[0m\n",
                 MakeTimeHeader(), MakeSeverityHeader(payload.Severity),
                 MakeLoggerNameHeader(payload.LoggerName),
                 payload.Message);
    }

    String ConsoleLogger::MakeTimeHeader()
    {
        std::time_t time;
        std::time(&time);

        std::tm* timeInfo = std::localtime(&time);
        if (timeInfo == nullptr)
            return "";

        return Format(
            "\x1B[34m[{0}:{1}:{2}] ",
            timeInfo->tm_hour, timeInfo->tm_min, timeInfo->tm_sec);
    }

    String ConsoleLogger::MakeSeverityHeader(const LogSeverity severity)
    {
        StringView severityString;
        StringView severityColor;

        switch (severity)
        {
        case LogSeverity::Trace:
            severityString = "TRACE";
            severityColor = TraceColor;
            break;

        case LogSeverity::Info:
            severityString = "INFO";
            severityColor = InfoColor;
            break;

        case LogSeverity::Warning:
            severityString = "WARN";
            severityColor = WarningColor;
            break;

        case LogSeverity::Error:
            severityString = "ERROR";
            severityColor = ErrorColor;
            break;

        case LogSeverity::Fatal:
            severityString = "FATAL";
            severityColor = FatalColor;
            break;

        default:
            KITSUNE_UNREACHABLE();
        }

        return Format(
            "{0}[{1}] ",
            severityColor, severityString);
    }

    String ConsoleLogger::MakeLoggerNameHeader(const StringView loggerName)
    {
        if (loggerName.IsEmpty())
            return "";

        return Format("\x1B[32m({0})\x1B[0m", loggerName);
    }
}
