#pragma once

#include "Foundation/String/Format.h"
#include "Foundation/Diagnostics/Exception.h"

namespace Kitsune
{
    // Thrown when one or more invalid arguments were passed into a function.
    class InvalidArgumentException : public Exception
    {
    public:
        inline InvalidArgumentException(const char* description =
            "Invalid argument passed.")
            : Exception("InvalidArgumentException", description)
        {
        }

        inline InvalidArgumentException(const String& description)
            : InvalidArgumentException(description.Raw())
        {
        }

        template<typename... Args>
        inline InvalidArgumentException(const char* format, Args&&... args)
            : InvalidArgumentException(Format(format, Forward<Args>(args)...))
        {
        }
    };
}
