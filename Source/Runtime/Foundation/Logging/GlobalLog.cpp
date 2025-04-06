#include "Foundation/Logging/GlobalLog.h"
#include "Foundation/Templates/Exchange.h"

namespace Kitsune
{
    namespace { Logger* g_GlobalLogger = nullptr; }

    Logger* SetGlobalLogger(Logger* logger)
    {
        return Exchange(g_GlobalLogger, logger);
    }

    Logger* GetGlobalLogger()
    {
        return g_GlobalLogger;
    }
}
