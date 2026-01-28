#pragma once

#include "Foundation/Diagnostics/Exception.h"

namespace Kitsune
{
    class BadCallException : public Exception
    {
    public:
        inline BadCallException()
            : Exception("BadCallException", "Tried to invoke an empty Function<T>.")
        {
        }
    };
}
