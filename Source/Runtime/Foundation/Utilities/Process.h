#pragma once

#include "Foundation/Common/Macros.h"

namespace Kitsune
{
    class Process
    {
    public:
        KITSUNE_API_ static void Exit(int exitCode);

        [[noreturn]]
        KITSUNE_API_ static void ForceExit(int exitCode);

    public:
        KITSUNE_API_ static bool IsExitRequested();
        KITSUNE_API_ static int GetExitCode();

    private:
        KITSUNE_API_ static int s_ExitCode;
        KITSUNE_API_ static bool s_ExitRequested;
    };
}
