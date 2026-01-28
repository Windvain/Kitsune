#pragma once

#include "Foundation/Diagnostics/Exception.h"

namespace Kitsune
{
    class FormatException : public Exception
    {
    public:
        inline FormatException(const char* desc = "Unknown format error")
            : Exception("FormatException", desc)
        {
        }
    };
}
