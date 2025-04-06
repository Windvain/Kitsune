#pragma once

#include "Foundation/Common/Macros.h"
#include "Foundation/Filesystem/Path.h"

#include "ApplicationCore/CommandLineArguments.h"

namespace Kitsune
{
    class Environment
    {
    public:
        KITSUNE_API_ static void Initialize(int argc, char** argv);
        KITSUNE_API_ static void Shutdown();

    public:
        KITSUNE_API_ static CommandLineArguments GetCommandLineArguments();

        KITSUNE_API_ static Path GetCurrentWorkingDirectory();
        KITSUNE_API_ static void SetCurrentWorkingDirectory(const Path& path);

    public:
        KITSUNE_API_ static Path GetExecutablePath();
    };
}
