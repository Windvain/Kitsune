#include "Foundation/Logging/AnsiColorSink.h"

#include "Foundation/String/Format.h"
#include "Foundation/Diagnostics/Assert.h"
#include "Foundation/Logging/WriteStreamIterator.h"

namespace Kitsune
{
    void AnsiColorSink::Log(const LogMessage& message)
    {
        LockGuard guard(m_SinkLock);
        KITSUNE_ASSERT(m_Stream != nullptr, "Tried to log to a nullptr sink.");

        String header;
        String locInfo;

        const SourceLocation& location = message.Location;

        if (!message.LoggerName.IsEmpty())
            header = Format("[{0}]: ", message.LoggerName);

        if (location != SourceLocation())
        {
            locInfo = Format(" [In function {0}, {1}:{2}]",
                             location.FunctionName(), location.FileName(),
                             location.Line());
        }

        StringView fmt = "{0}{1}{2}{3}\x1B[0m\n";
        FormatTo(WriteStreamIterator<char>(*m_Stream), DefaultFormatScanner(fmt), fmt,
                 ConvertToAnsiColor(message.Severity), header,
                 message.Message, locInfo);
    }

    void AnsiColorSink::Flush()
    {
        LockGuard guard(m_SinkLock);
        m_Stream->Flush();
    }

    const char* AnsiColorSink::ConvertToAnsiColor(LogSeverity severity)
    {
        switch (severity)
        {
        case LogSeverity::Trace:   return TraceColor;
        case LogSeverity::Info:    return InfoColor;
        case LogSeverity::Warning: return WarningColor;
        case LogSeverity::Error:   return ErrorColor;
        case LogSeverity::Fatal:   return FatalColor;
        };
    }
}
