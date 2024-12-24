#pragma once

namespace Kitsune
{
    enum class ThreadSafety
    {
        // Safe to use in single-threaded environments.
        NotThreadSafe,

        // Safe to use in multithreaded environments, but comes
        // at a small cost of using atomic operations.
        ThreadSafe
    };
}
