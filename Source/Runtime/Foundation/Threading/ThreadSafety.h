#pragma once

namespace Kitsune
{
    enum class ThreadSafety
    {
        NotThreadSafe,      //< Safe to use in single-threaded environments.
        ThreadSafe          //< Safe to use in multithreaded environments, but comes
                            //  at a small cost of using atomic operations.
    };
}
