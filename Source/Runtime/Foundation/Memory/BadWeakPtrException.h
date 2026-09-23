#pragma once

#include "Foundation/Diagnostics/Exception.h"

namespace Kitsune
{
    // Thrown when a request to SharedPtr<T> to manage a deleted WeakPtr<T>.
    class BadWeakPtrException : public Exception
    {
    public:
        inline BadWeakPtrException()
            : Exception(
                "BadWeakPtrException",
                "Requested SharedPtr<T> to manage a deleted object.")
        {
        }
    };
}
