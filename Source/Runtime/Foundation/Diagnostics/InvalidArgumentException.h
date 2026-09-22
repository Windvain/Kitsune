#pragma once

#include "Foundation/String/Format.h"
#include "Foundation/Diagnostics/Exception.h"

namespace Kitsune
{
    // Thrown when one or more invalid arguments were passed into a function.
    class InvalidArgumentException : public Exception
    {
    public:
        inline InvalidArgumentException()
            : InvalidArgumentException("An invalid argument(s) was passed.")
        {
        }

        inline explicit InvalidArgumentException(const char* description)
            : Exception("InvalidArgumentException", description)
        {
        }

        inline explicit InvalidArgumentException(const String& description)
            : InvalidArgumentException(description.Raw())
        {
        }

        template<typename... Args>
        inline explicit InvalidArgumentException(const char* format, Args&&... args)
            : InvalidArgumentException(Format(format, Forward<Args>(args)...))
        {
        }
    };
}
