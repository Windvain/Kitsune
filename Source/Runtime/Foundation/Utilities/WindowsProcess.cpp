#include "Foundation/Utilities/Process.h"
#include <Windows.h>

KITSUNE_PUSH_COMPILER_WARNINGS()
KITSUNE_IGNORE_CLANG_WARNING(-Winvalid-noreturn)

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

KITSUNE_POP_COMPILER_WARNINGS()
