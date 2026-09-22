#pragma once

#include "Foundation/String/Format.h"
#include "Foundation/Diagnostics/Exception.h"

namespace Kitsune
{
    // Thrown when a general operating system level error happens.
    class SystemException : public Exception
    {
    public:
        inline SystemException()
            : SystemException("An unknown system error occurred.")
        {
        }

        inline explicit SystemException(const char* description)
            : Exception("SystemException", description)
        {
        }

        inline explicit SystemException(const String& description)
            : SystemException(description.Raw())
        {
        }

        template<typename... Args>
        inline explicit SystemException(const char* format, Args&&... args)
            : SystemException(Format(format, Forward<Args>(args)...))
        {
        }
    };
}
