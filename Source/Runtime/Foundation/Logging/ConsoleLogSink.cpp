#include "Foundation/Logging/ConsoleLogSink.h"
#include <ctime>        // TODO: Replace this with our own functions.

#include "Foundation/String/Format.h"
#include "Foundation/Streams/WriterIterator.h"

namespace Kitsune
{
    void ConsoleLogSink::Log(const LogPayload& payload)
    {
        String locationInfo;
        String topicInfo;

        if (!payload.Topic.IsEmpty())
            topicInfo = Format(" \x1B[32m({0})", payload.Topic);

        if (payload.Location != SourceLocation())
        {
            locationInfo = Format(
                " ({0}:{1})",
                payload.Location.FileName(), payload.Location.Line());
        }

        String timeInfo;
        String severityInfo;

#pragma region Time Information Formatting
        {
            std::time_t time;
            std::time(&time);

            std::tm* localTime = std::localtime(&time);
            if (localTime != nullptr)
            {
                // Time information always comes first, no need
                // for a space character to be in the front.
                timeInfo = Format(
                    "\x1B[34m[{0}:{1}:{2}]",
                    localTime->tm_hour, localTime->tm_min, localTime->tm_sec);
            }
        }
#pragma endregion

#pragma region Severity Information Formatting
        {
            StringView severityString;
            StringView severityColor;

            switch (payload.Severity)
            {
            case LogSeverity::Trace:
                severityString = "TRACE";
                severityColor = s_TraceSeverityColor;
                break;

            case LogSeverity::Info:
                severityString = "INFO";
                severityColor = s_InfoSeverityColor;
                break;

            case LogSeverity::Warning:
                severityString = "WARN";
                severityColor = s_WarningSeverityColor;
                break;

            case LogSeverity::Error:
                severityString = "ERROR";
                severityColor = s_ErrorSeverityColor;
                break;

            case LogSeverity::Fatal:
                severityString = "FATAL";
                severityColor = s_FatalSeverityColor;
                break;

            default:
                KITSUNE_UNREACHABLE();
            }

            severityInfo = Format(" {0}[{1}]", severityColor, severityString);
        }
#pragma endregion

        // [12:34:56] [INFO] (My Topic/Subtopic): Hello, World! (file.cpp:78)
        FormatTo(
            WriterIterator<char>(m_Writer),
            "{0}{1}{2}\x1B[0m: {3}{4}\n",
            timeInfo, severityInfo, topicInfo, payload.Message, locationInfo);
    }

    void ConsoleLogSink::Flush()
    {
        m_Writer.Flush();
    }
}
