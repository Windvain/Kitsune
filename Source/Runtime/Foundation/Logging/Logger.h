#pragma once

#include "Foundation/Logging/LogPayload.h"

namespace Kitsune
{
    // An abstract class which allows for logging to a specific output or sink.
    class Logger
    {
    public:
        virtual ~Logger() = default;

    public:
        virtual void Log(const LogPayload& payload) = 0;
    };
}
