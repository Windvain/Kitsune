#include "Foundation/Logging/ConsoleLogger.h"

#include "Foundation/String/Format.h"
#include "Foundation/Logging/WriteStreamIterator.h"

namespace Kitsune
{
    void ConsoleLogger::Log(const LogPayload& payload)
    {
        String header;
        String locationInfo;

        const SourceLocation & location = payload.Location;

        if (!payload.LoggerName.IsEmpty())
            header = Format("[{0}]: ", payload.LoggerName);

        if (payload.Location != SourceLocation())
        {
            locationInfo = Format(" [In function {0}, {1}:{2}]",
                                  location.FunctionName(), location.FileName(),
                                  location.Line());
        }

        StringView format = "{0}{1}{2}{3}\x1B[0m\n";
        FormatTo(WriteStreamIterator<char>(m_Stream), format,
                 ConvertToAnsiColor(payload.Severity), header, payload.Message,
                 locationInfo);
    }
}
