#pragma once

#include "Foundation/Diagnostics/Exception.h"

namespace Kitsune
{
    // Thrown when an attempt was made to invoke an empty Function<T>.
    class BadCallException : public Exception
    {
    public:
        inline BadCallException()
            : Exception("BadCallException",
                        "Tried to invoke an empty Function<T>.")
        {
        }
    };
}
