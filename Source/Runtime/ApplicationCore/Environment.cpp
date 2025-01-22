#include "ApplicationCore/Environment.h"
#include "Foundation/Diagnostics/Assert.h"

namespace Kitsune
{
    bool Environment::s_Initialized = false;
    CommandLineArguments Environment::s_CmdLineArgs;

    void Environment::Initialize(int argc, char** argv)
    {
        KITSUNE_ASSERT(!s_Initialized, "Environment static class has already been initialized.");

        s_CmdLineArgs = CommandLineArguments(argc, argv);
        s_Initialized = true;
    }

    void Environment::Shutdown()
    {
        s_Initialized = false;
    }
}
