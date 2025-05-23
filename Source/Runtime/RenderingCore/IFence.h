#pragma once

#include "Foundation/Common/Types.h"

namespace Kitsune
{
    class IFence
    {
    public:
        virtual ~IFence() { /* ... */ }

    public:
        virtual Uint64 GetSignaledValue() const = 0;

        virtual void Signal() = 0;
        virtual void Wait() = 0;
    };
}
