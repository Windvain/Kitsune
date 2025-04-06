#pragma once

#include "Foundation/Logging/ILogSink.h"
#include "Foundation/Logging/ConsoleStream.h"

#include "Foundation/Threading/Mutex.h"
#include "Foundation/Threading/LockGuard.h"

namespace Kitsune
{
    class AnsiColorSink : public ILogSink
    {
    public:
        inline AnsiColorSink(ConsoleOutputStream& outStream,
                             ConsoleOutputStream& errStream)
            : m_StdoutStream(outStream), m_StderrStream(errStream)
        {
        }

    public:
        KITSUNE_API_ void Log(const LogMessage& message) override;
        KITSUNE_API_ void Flush() override;

    private:
        KITSUNE_FORCEINLINE
        static const char* PickAnsiColor(LogSeverity severity)
        {
            return s_AnsiColorCode[static_cast<Index>(severity)];
        }

    private:
        // The bold colours appear *lighter* than their non-bolded counterparts (?)
        static constexpr const char* s_AnsiColorCode[] = {
            "\x1B[0m",      // Trace
            "\x1B[36m",     // Info
            "\x1B[33m",     // Warning
            "\x1B[31;1m",   // Error
            "\x1B[31m",     // Fatal
        };

    private:
        ConsoleOutputStream& m_StdoutStream;
        ConsoleOutputStream& m_StderrStream;

        Mutex m_SinkLock;
    };
}
