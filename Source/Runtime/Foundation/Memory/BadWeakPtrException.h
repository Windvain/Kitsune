#pragma once

#include "Foundation/Diagnostics/IException.h"

namespace Kitsune
{
    class BadWeakPtrException : public IException
    {
    public:
        inline BadWeakPtrException()
            : IException("BadWeakPtrException",
                         "Requested SharedPtr<T> to manage a deleted object")
        {
        }
    };
}
