#pragma once

#include "Foundation/Diagnostics/Exception.h"

namespace Kitsune
{
    class OutOfRangeException : public Exception
    {
    public:
        inline OutOfRangeException()
            : Exception("OutOfRangeException", "Requested an out of range index")
        {
        }
    };
}
