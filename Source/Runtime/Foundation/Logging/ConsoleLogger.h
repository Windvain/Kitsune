#pragma once

#include "Foundation/Logging/Logger.h"
#include "Foundation/IO/ConsoleOutputStream.h"

namespace Kitsune
{
    class ConsoleLogger : public Logger
    {
    public:
        void Log(const LogPayload& payload) override;

    public:
        static constexpr const char* TraceColor = "\x1B[0m";
        static constexpr const char* InfoColor = "\x1B[36m";
        static constexpr const char* WarningColor = "\x1B[33m";

        // The bold colors appear *lighter* than their non-bold counterparts (?)
        static constexpr const char* ErrorColor = "\x1B[31;1m";
        static constexpr const char* FatalColor = "\x1B[31m";

    private:
        static String MakeTimeHeader();
        static String MakeSeverityHeader(const LogSeverity severity);

        static String MakeLoggerNameHeader(const StringView loggerName);

    private:
        ConsoleOutputStream m_Stream;
    };
}
