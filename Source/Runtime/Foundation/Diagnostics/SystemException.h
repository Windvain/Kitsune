#pragma once

#include "Foundation/Diagnostics/Exception.h"

namespace Kitsune
{
    class SystemException : public Exception
    {
    public:
        inline SystemException(const char* desc = "Unknown system error")
            : Exception("SystemException", desc)
        {
        }
    };
}
