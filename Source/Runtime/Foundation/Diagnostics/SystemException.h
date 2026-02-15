#pragma once

#include "Foundation/Diagnostics/Exception.h"

namespace Kitsune
{
    // Thrown when a general operating system level error happens.
    class SystemException : public Exception
    {
    public:
        inline SystemException(const char* description = "Unknown system error")
            : Exception("SystemException", description)
        {
        }
    };
}
