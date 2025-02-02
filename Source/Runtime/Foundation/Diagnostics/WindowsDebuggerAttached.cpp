#include "Foundation/Diagnostics/IsDebuggerAttached.h"
#include <Windows.h>

namespace Kitsune
{
    bool IsDebuggerAttached()
    {
        return (::IsDebuggerPresent() != 0);
    }
}
