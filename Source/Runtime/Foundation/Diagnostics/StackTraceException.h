#pragma once

#include "Foundation/Diagnostics/IException.h"

namespace Kitsune
{
    class StackTraceException : public IException
    {
    public:
        inline StackTraceException(const char* desc = "Unknown stack trace exception")
            : IException("StackTraceException", desc)
        {
        }
    };
}
