#pragma once

namespace Kitsune
{
    // Inherited by a class to make that class non-copyable.
    class NonCopyable
    {
    public:
        NonCopyable() = default;

        NonCopyable(const NonCopyable&) = delete;
        NonCopyable& operator=(const NonCopyable&) = delete;
    };
}
