#pragma once

#include "Foundation/Diagnostics/Exception.h"

namespace Kitsune
{
    class LogicException : public Exception
    {
    public:
        LogicException(const char* description = "Unknown logic error occurred.")
            : Exception("LogicException", description)
        {
        }
    };
}
