#pragma once

#include "Foundation/Diagnostics/Exception.h"

namespace Kitsune
{
    // Thrown when one or more invalid arguments were passed into a function.
    class InvalidArgumentException : public Exception
    {
    public:
        InvalidArgumentException(const char* description = "Invalid argument passed")
            : Exception("InvalidArgumentException", description)
        {
        }
    };
}
