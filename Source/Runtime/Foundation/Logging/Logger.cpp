#include "Foundation/Logging/Logger.h"

#include "Foundation/Diagnostics/Assert.h"
#include "Foundation/Diagnostics/LogicException.h"

namespace Kitsune
{
    Logger* Logger::s_Instance = nullptr;

    Logger::Logger()
    {
        if (s_Instance != nullptr)
        {
            throw LogicException(
                "The Logger class has already been initialized. Please "
                "do not instantiate another instance of the logger.");
        }

        s_Instance = this;
    }

    Logger::~Logger()
    {
        KITSUNE_ASSERT(
            s_Instance != nullptr,
            "The s_Instance pointer of the Logger class was not set.");

        s_Instance = nullptr;
    }
}
