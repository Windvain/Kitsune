#pragma once

namespace Kitsune
{
    enum class MemoryOrder
    {
        Relaxed = 0,
        Consume = 1,
        Acquire = 2,
        Release = 3,
        AcqRel = 4,
        SeqCst = 5
    };
}
