#include "Foundation/Logging/AnsiColorSink.h"

#include "Foundation/String/Format.h"
#include "Foundation/Logging/WriteStreamIterator.h"

namespace Kitsune
{
    void AnsiColorSink::Log(const LogMessage& message)
    {
        LockGuard guard(m_SinkLock);
        ConsoleOutputStream& stream =
            (message.Severity < LogSeverity::Error) ? m_StdoutStream :
                                                      m_StderrStream;

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
        FormatTo(WriteStreamIterator<char>(stream), DefaultFormatScanner(fmt), fmt,
                 PickAnsiColor(message.Severity), header, message.Message,
                 locInfo);
    }

    void AnsiColorSink::Flush()
    {
        LockGuard guard(m_SinkLock);

        m_StdoutStream.Flush();
        m_StderrStream.Flush();
    }
}
