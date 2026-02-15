#pragma once

#include "Foundation/Diagnostics/Exception.h"

namespace Kitsune
{
    // Thrown when an error has happened during the formatting of a string.
    class FormatException : public Exception
    {
    public:
        inline FormatException(const char* desc = "Unknown format error")
            : Exception("FormatException", desc)
        {
        }
    };
}
