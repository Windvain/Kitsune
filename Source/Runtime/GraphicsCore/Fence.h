#pragma once

#include "Foundation/Common/Types.h"

#include "Foundation/Utilities/EnumFlags.h"
#include "Foundation/Utilities/NonCopyable.h"

namespace Kitsune
{
    // Flags for fence creation.
    enum class FenceFlag
    {
        None = 0,
        Signaled = 1 << 0
    };

    KITSUNE_OVERLOAD_FLAGS_OPERATORS(FenceFlag);

    // A synchronization object used for syncing the GPU and the CPU.
    class Fence : public NonCopyable
    {
    public:
        virtual ~Fence() = default;

        virtual void Wait(Uint64 timeout) = 0;
        virtual void Reset() = 0;
    };
}
