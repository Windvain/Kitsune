#include "Foundation/Logging/GlobalLog.h"
#include "Launch/EngineLoop.h"

namespace Kitsune
{
    void Log(StringView loggerName, LogSeverity severity,
             const SourceLocation& location, StringView message)
    {
        auto* engineLoop = EngineLoop::GetInstance();
        if (engineLoop == nullptr)
            return;

        LogPayload logPayload(message, loggerName, location, severity);
        for (ScopedPtr<Logger>& logger : engineLoop->GetLoggers())
            logger->Log(logPayload);
    }
}
