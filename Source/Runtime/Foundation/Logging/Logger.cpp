#include "Foundation/Logging/Logger.h"

#include "Foundation/Diagnostics/Assert.h"
#include "Foundation/Diagnostics/SingletonException.h"

namespace Kitsune
{
    Logger* Logger::s_Instance = nullptr;

    Logger::Logger()
    {
        if (s_Instance != nullptr)
            throw SingletonException();

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
