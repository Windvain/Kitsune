#include "Foundation/Logging/GlobalLog.h"
// #include "Launch/GameEngine.h"

namespace Kitsune::Details
{
    void LogGlobal(LogSeverity severity, SourceLocation loc, const StringView str)
    {
        KITSUNE_UNUSED(severity); KITSUNE_UNUSED(loc); KITSUNE_UNUSED(str);
        /* GameEngine* engine = GameEngine::GetInstance();
        for (auto& logger : engine->GetLoggers())
            logger->Log(LogPayload(str, "GLOBAL", Move(loc), severity)); */
    }
}
