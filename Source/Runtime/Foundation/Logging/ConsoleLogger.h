#pragma once

#include "Foundation/Logging/ILogger.h"
#include "Foundation/Logging/ConsoleOutputStream.h"

namespace Kitsune
{
    class ConsoleLogger : public ILogger
    {
    public:
        void Log(const LogPayload& payload) override;

    public:
        static constexpr const char* TraceColor = "\x1B[0m";
        static constexpr const char* InfoColor = "\x1B[36m";
        static constexpr const char* WarningColor = "\x1B[33m";

        // The bold colours appear *lighter* than their non-bolded counterparts (?)
        static constexpr const char* ErrorColor = "\x1B[31;1m";
        static constexpr const char* FatalColor = "\x1B[31m";

    private:
        inline static const char* ConvertToAnsiColor(LogSeverity severity)
        {
            switch (severity)
            {
            case LogSeverity::Trace:   return TraceColor;
            case LogSeverity::Info:    return InfoColor;
            case LogSeverity::Warning: return WarningColor;
            case LogSeverity::Error:   return ErrorColor;
            case LogSeverity::Fatal:   return FatalColor;
            default:
                KITSUNE_UNREACHABLE();
            };
        }

    private:
        ConsoleOutputStream m_Stream;
    };
}
