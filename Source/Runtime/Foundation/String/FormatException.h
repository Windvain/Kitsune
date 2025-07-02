#pragma once

#include "Foundation/Diagnostics/IException.h"

namespace Kitsune
{
    class FormatException : public IException
    {
    public:
        inline FormatException(const char* desc = "Unknown format error")
            : IException("FormatException", desc)
        {
        }
    };
}
