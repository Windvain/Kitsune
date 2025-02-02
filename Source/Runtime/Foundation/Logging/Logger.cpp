#include "Foundation/Logging/Logger.h"
#include "Foundation/Algorithms/ForEach.h"

namespace Kitsune
{
    void Logger::Log(LogSeverity severity, const StringView message)
    {
        if (!IsLogged(severity)) return;

        LogMessage logMessage;
        logMessage.Severity = severity;
        logMessage.Message = message;
        logMessage.LoggerName = m_Name;

        Algorithms::ForEach(m_Sinks.GetBegin(), m_Sinks.GetEnd(),
            [&](const SharedPtr<ILoggerSink>& loggerSink)
            {
                KITSUNE_ASSERT(loggerSink != nullptr, "Tried to log to a sink that doesn't exist.");
                loggerSink->Log(logMessage);

                if (IsFlushed(severity))
                    loggerSink->Flush();
            });
    }

    void Logger::Flush()
    {
        Algorithms::ForEach(m_Sinks.GetBegin(), m_Sinks.GetEnd(),
            [](const SharedPtr<ILoggerSink>& loggerSink)
            {
                KITSUNE_ASSERT(loggerSink != nullptr, "Tried to flush a sink that doesn't exist.");
                loggerSink->Flush();
            });
    }
}
