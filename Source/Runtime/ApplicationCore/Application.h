#pragma once

#include "Foundation/Common/Macros.h"
#include "Launch/EngineLoop.h"

namespace Kitsune
{
    struct ApplicationSpecs
    {
    };

    struct CommandLineArgs
    {
        int Count;
        char** Args;
    };

    class Application
    {
    public:
        KITSUNE_API_ Application(const ApplicationSpecs& specs);
        KITSUNE_API_ virtual ~Application();

    public:
        bool IsExitRequested() const { return m_ExitRequested; }

    private:
        friend class EngineLoop;

    private:
        bool m_ExitRequested;
        CommandLineArgs m_CommandLineArgs;

        static Application* s_ApplicationInst;
    };

}

// Should be defined in client code.
extern Kitsune::Application* CreateApplication(const Kitsune::CommandLineArgs& cmdLineArgs);
