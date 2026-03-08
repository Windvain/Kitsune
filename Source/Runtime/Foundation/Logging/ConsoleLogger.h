#pragma once

#include "Foundation/Logging/Logger.h"
#include "Foundation/IO/ConsoleOutputStream.h"

namespace Kitsune
{
    // A logger which facilitates output to the console.
    class ConsoleLogger : public Logger
    {
    public:
        void Log(const LogPayload& payload) override;

    public:
        static constexpr const char* TraceColor = "\x1B[0;0m";
        static constexpr const char* InfoColor = "\x1B[36;0m";
        static constexpr const char* WarningColor = "\x1B[33;0m";

        // The bold colors appear *lighter* than their non-bold counterparts (?)
        static constexpr const char* ErrorColor = "\x1B[31;1m";
        static constexpr const char* FatalColor = "\x1B[31;0m";

    private:
        static String MakeTimeHeader_();
        static String MakeSeverityHeader_(const LogSeverity severity);

        static String MakeLoggerNameHeader_(const StringView loggerName);

    private:
        ConsoleOutputStream m_Stream;
    };
}
