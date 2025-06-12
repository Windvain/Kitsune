#include "Foundation/Utilities/Process.h"
#include <Windows.h>

namespace Kitsune
{
    void Process::Exit(int exitCode)
    {
        s_ExitCode = exitCode;
        s_ExitRequested = true;

        ::PostQuitMessage(exitCode);
    }

    void Process::ForceExit(int exitCode)
    {
        Exit(exitCode);
        ::TerminateProcess(::GetCurrentProcess(), static_cast<UINT>(exitCode));
    }
}
