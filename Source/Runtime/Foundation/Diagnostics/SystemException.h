#pragma once

#include "Foundation/String/Format.h"
#include "Foundation/Diagnostics/Exception.h"

namespace Kitsune
{
    // Thrown when a general operating system level error happens.
    class SystemException : public Exception
    {
    public:
        inline SystemException(const char* description = "Unknown system error.")
            : Exception("SystemException", description)
        {
        }

        inline SystemException(const String& description)
            : SystemException(description.Raw())
        {
        }

        template<typename... Args>
        inline SystemException(const char* format, Args&&... args)
            : SystemException(Format(format, Forward<Args>(args)...))
        {
        }
    };
}
