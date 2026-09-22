#pragma once

#include "Foundation/String/Format.h"
#include "Foundation/Diagnostics/Exception.h"

namespace Kitsune
{
    // Thrown when one or more invalid arguments were passed into a function.
    class IOException : public Exception
    {
    public:
        inline IOException()
            : IOException("An unknown I/O error occurred.")
        {
        }

        inline explicit IOException(const char* description)
            : Exception("IOException", description)
        {
        }

        inline explicit IOException(const String& description)
            : IOException(description.Raw())
        {
        }

        template<typename... Args>
        inline explicit IOException(const char* format, Args&&... args)
            : IOException(Format(format, Forward<Args>(args)...))
        {
        }
    };
}
