#pragma once

#include "Foundation/Diagnostics/Exception.h"

namespace Kitsune
{
    class InvalidArgumentException : public Exception
    {
    public:
        InvalidArgumentException(const char* desc = "Invalid argument passed")
            : Exception("InvalidArgumentException", desc)
        {
        }
    };
}
