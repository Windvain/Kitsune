#pragma once

#include "Foundation/Logging/LogSink.h"
#include "Foundation/Streams/ConsoleWriter.h"

namespace Kitsune
{
    // A log sink which outputs log payloads to the console.
    class ConsoleLogSink : public LogSink
    {
    public:
        void Log(const LogPayload& payload) override;
        void Flush() override;

    private:
        static constexpr const char* s_TraceSeverityColor = "\x1B[0m";
        static constexpr const char* s_InfoSeverityColor = "\x1B[36m";
        static constexpr const char* s_WarningSeverityColor = "\x1B[33m";

        // The bold colors appear *lighter* than their non-bold counterparts (?)
        static constexpr const char* s_ErrorSeverityColor = "\x1B[31;1m";
        static constexpr const char* s_FatalSeverityColor = "\x1B[31m";

    private:
        ConsoleWriter m_Writer{ ConsoleWriterType::StdOut };
    };
}
