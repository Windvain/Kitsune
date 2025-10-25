#include "Foundation/Logging/GlobalLog.h"
#include "Launch/DefaultEngineLoop.h"

namespace Kitsune::Details
{
    void LogGlobal(const StringView loggerName, LogSeverity severity, SourceLocation loc, const StringView str)
    {
        auto* engineLoop = DefaultEngineLoop::GetInstance();
        for (SharedPtr<ILogger>& logger : engineLoop->GetLoggers())
        {
            logger->Log(LogPayload(str, loggerName, loc, severity));
        }
    }
}
