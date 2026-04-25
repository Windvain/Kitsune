#pragma once

#include "Foundation/Common/Types.h"
#include "Foundation/Utilities/NonCopyable.h"

namespace Kitsune
{
    // A synchronization object used for syncing the GPU <-> CPU.
    class Fence : public NonCopyable
    {
    public:
        virtual ~Fence() = default;

        virtual void Wait(Uint64 timeout) = 0;
        virtual void Reset() = 0;
    };
}
