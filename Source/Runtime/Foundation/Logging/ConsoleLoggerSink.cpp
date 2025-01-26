#include "Foundation/Logging/ConsoleLoggerSink.h"
#include <cstdio>

#include "Foundation/String/String.h"
#include "Foundation/String/Format.h"

namespace Kitsune
{
    void ConsoleLoggerSink::Log(const LogMessage& message)
    {
        // The bold colours appear *lighter* than their non-bolded counterparts (?)
        constexpr const char* ANSIColorCode[] = {
            "\x1B[0m",      // Trace
            "\x1B[36m",     // Info
            "\x1B[33m",     // Warning
            "\x1B[31;1m",   // Error
            "\x1B[31m",     // Fatal
        };

        String output = Format("{0}{1}\x1B[0m\n",
            ANSIColorCode[static_cast<Usize>(message.Severity)],
            message.Message);

        std::FILE* stream = stdout;
        if (message.Severity >= LogSeverity::Error)
            stream = stderr;

        std::fputs(output.Data(), stream);
    }

    void ConsoleLoggerSink::Flush()
    {
        std::fflush(stdout);
        std::fflush(stderr);
    }
}
