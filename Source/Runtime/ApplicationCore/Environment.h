#pragma once

#include "Foundation/Common/Macros.h"
#include "Foundation/Filesystem/Path.h"

#include "ApplicationCore/CommandLineArguments.h"

namespace Kitsune
{
    class Environment
    {
    public:
        static void Initialize(int argc, char** argv);
        static void Shutdown();

    public:
        static CommandLineArguments GetCommandLineArguments();

        static Filesystem::Path GetCurrentWorkingDirectory();
        static void SetCurrentWorkingDirectory(const Filesystem::Path& path);

    public:
        static Filesystem::Path GetExecutablePath();
    };
}
