#pragma once

#include "Foundation/Diagnostics/IException.h"

namespace Kitsune
{
    class BadWeakPtrException : public IException
    {
    public:
        BadWeakPtrException() = default;

    public:
        const char* GetName() const override { return "BadWeakPtrException"; }
        const char* GetDescription() const override
        {
            return "Requested SharedPtr<T> to manage a deleted object";
        }
    };
}
