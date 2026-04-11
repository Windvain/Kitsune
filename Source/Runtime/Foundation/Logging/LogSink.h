#pragma once

#include "Foundation/Logging/LogPayload.h"

namespace Kitsune
{
    // An abstract class representing a sink/output, which is used
    // by the `Logger` class for output.
    class LogSink
    {
    public:
        virtual ~LogSink() = default;

    public:
        virtual void Log(const LogPayload& payload) = 0;

        inline virtual void Flush()
        {
        }
    };
}
