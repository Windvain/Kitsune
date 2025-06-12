#include "Foundation/Utilities/Process.h"

namespace Kitsune
{
    int Process::s_ExitCode = 0;
    bool Process::s_ExitRequested = false;

    bool Process::IsExitRequested()
    {
        return s_ExitRequested;
    }

    int Process::GetExitCode()
    {
        return s_ExitCode;
    }
}
