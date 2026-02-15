#pragma once

#include "Foundation/Diagnostics/Exception.h"

namespace Kitsune
{
    // Thrown when an attempt to access an out of range index was made.
    // (read from/written to)
    class OutOfRangeException : public Exception
    {
    public:
        inline OutOfRangeException()
            : Exception("OutOfRangeException", "Requested an out of range index")
        {
        }
    };
}
