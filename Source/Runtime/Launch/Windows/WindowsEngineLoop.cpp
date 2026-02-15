#include "Launch/EngineLoop.h"

#include <Windows.h>
#include "Foundation/Logging/GlobalLog.h"

// Clang doesn't recognize that ::TerminateProcess() will just quit the program
// and marks ForceExit()'s [[noreturn]] as invalid.
KITSUNE_PUSH_COMPILER_WARNINGS()
KITSUNE_IGNORE_CLANG_WARNING(-Winvalid-noreturn)

namespace Kitsune
{
    void EngineLoop::Exit(int exitCode)
    {
        m_ExitCode = exitCode;
        m_ExitRequested = true;

        ::PostQuitMessage(exitCode);
    }

    void EngineLoop::ForceExit(int exitCode)
    {
        KITSUNE_ENGINE_WARN_("ForceExit() was called, forcing the engine to terminate.");

        Exit(exitCode);
        ::TerminateProcess(::GetCurrentProcess(), exitCode);
    }
}

KITSUNE_POP_COMPILER_WARNINGS()
