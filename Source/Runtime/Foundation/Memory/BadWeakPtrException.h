#pragma once

#include "Foundation/Diagnostics/IException.h"

namespace Kitsune
{
    class BadWeakPtrException : public IException
    {
    public:
        BadWeakPtrException() = default;

    public:
        const char* GetName() const noexcept override { return "BadWeakPtrException"; }
        const char* GetDescription() const noexcept override
        {
            return "Requested SharedPtr<T> to manage a deleted object";
        }
    };
}
