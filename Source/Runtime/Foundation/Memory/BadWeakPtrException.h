#pragma once

#include "Foundation/Diagnostics/Exception.h"

namespace Kitsune
{
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
