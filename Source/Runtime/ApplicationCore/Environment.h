#pragma once

#include "Foundation/Common/Macros.h"
#include "ApplicationCore/CommandLineArguments.h"

namespace Kitsune
{
    class Environment
    {
    public:
        KITSUNE_API_ static void Initialize(int argc, char** argv);
        KITSUNE_API_ static void Shutdown();

    public:
        static inline CommandLineArguments GetCommandLineArguments() { return s_CmdLineArgs; }

    private:
        KITSUNE_API_ static CommandLineArguments s_CmdLineArgs;
        static bool s_Initialized;
    };
}
