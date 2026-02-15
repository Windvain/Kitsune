#include "Foundation/Logging/GlobalLog.h"
#include "Launch/EngineLoop.h"

namespace Kitsune
{
    void Log(const StringView loggerName, const LogSeverity severity,
             const SourceLocation location, const StringView message)
    {
        auto* engineLoop = EngineLoop::GetInstance();
        LogPayload logPayload(message, loggerName, location, severity);

        for (ScopedPtr<Logger>& logger : engineLoop->GetLoggers())
            logger->Log(logPayload);
    }
}
