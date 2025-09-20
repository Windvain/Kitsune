#pragma once

#include "Foundation/Common/Macros.h"

namespace Kitsune
{
    class Process
    {
    public:
        static void Exit(int exitCode);

        [[noreturn]]
        static void ForceExit(int exitCode);

    public:
        static bool IsExitRequested();
        static int GetExitCode();

    private:
        static int s_ExitCode;
        static bool s_ExitRequested;
    };
}
