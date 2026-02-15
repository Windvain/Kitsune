#pragma once

namespace Kitsune
{
    // Returns true if a debugger is attached when this function was called, else
    // returns false.
    [[nodiscard]]
    bool IsDebuggerAttached();
}
