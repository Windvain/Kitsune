#pragma once

#include <ctime>        // TODO: Replace this with our own functions.

#include "Foundation/String/Format.h"
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

    template<>
    class Formatter<LogPayload, char>
    {
    public:
        template<OutputIterator<const char&> Iter>
        inline static Iter Format(
            const LogPayload& payload,
            const FormatContext<Iter>& context)
        {
            bool colored = context.GetFormatSpecifications().Contains('c');

            // [12:34:56] [INFO] (My Topic/Subtopic): Hello, World! (file.cpp:78)
            return FormatTo(
                context.GetOutput(),
                "{0}{1}{2}{3}: {4}{5}\n",
                GetTimeInformation(colored),
                GetSeverityInformation(payload.Severity, colored),
                GetTopicInformation(payload.Topic, colored),
                colored ? s_ResetColor : "",
                payload.Message,
                GetLocationInformation(payload.Location));
        }

    private:
        inline static String GetTimeInformation(bool colored)
        {
            std::time_t time;
            std::time(&time);

            std::tm* localTime = std::localtime(&time);
            if (localTime == nullptr)
                return "";

            // Time information always comes first, no need
            // for a space character to be in the front.
            return Kitsune::Format(
                "{0}[{1}:{2}:{3}]",
                colored ? s_TimeInfoColor : "",
                localTime->tm_hour,
                localTime->tm_min,
                localTime->tm_sec);
        }

        inline static String GetSeverityInformation(LogSeverity severity, bool colored)
        {
            int index = static_cast<int>(severity);
            return Kitsune::Format(
                " {0}[{1}]",
                colored ? s_SeverityColors[index] : "",
                s_SeverityNames[index]);
        }

        inline static String GetLocationInformation(
            const SourceLocation& location)
        {
            if (location == SourceLocation())
                return "";

            return Kitsune::Format(
                " ({0}:{1})",
                location.FileName(), location.Line());
        }

        inline static String GetTopicInformation(StringView topic, bool colored)
        {
            if (topic.IsEmpty())
                return "";

            return Kitsune::Format(
                " {0}({1})",
                colored ? s_TopicInfoColor : "",
                topic);
        }

    private:
        static_assert(LogSeverity::Trace == LogSeverity(0));
        static_assert(LogSeverity::Info == LogSeverity(1));
        static_assert(LogSeverity::Warning == LogSeverity(2));
        static_assert(LogSeverity::Error == LogSeverity(3));
        static_assert(LogSeverity::Fatal == LogSeverity(4));

        static constexpr const char* s_SeverityColors[] = {
            "\x1B[0m",
            "\x1B[36m",
            "\x1B[33m",
            "\x1B[31;1m",   // The bold colors appear *lighter* than non-bold colors.
            "\x1B[31m"
        };

        static constexpr const char* s_SeverityNames[] = {
            "TRACE",
            "INFO",
            "WARN",
            "ERROR",
            "FATAL"
        };

    private:
        static constexpr const char* s_TimeInfoColor = "\x1B[34m";
        static constexpr const char* s_TopicInfoColor = "\x1B[32m";

        static constexpr const char* s_ResetColor = "\x1B[0m";
    };
}
