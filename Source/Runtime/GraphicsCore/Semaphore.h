#pragma once

#include "Foundation/Utilities/NonCopyable.h"

namespace Kitsune
{
    // A synchronization object used for syncing between GPU queues.
    class Semaphore : public NonCopyable
    {
    public:
        virtual ~Semaphore() = default;
    };
}
