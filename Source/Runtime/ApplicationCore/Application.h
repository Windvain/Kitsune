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

        KITSUNE_API_ void Exit(int exitCode);
        KITSUNE_API_ void ForceExit(int exitCode);

    public:
        virtual void OnStart() { /* ... */ }
        virtual void OnUpdate() { /* ... */ }
        virtual void OnExit() { /* ... */ }

    private:
        void ProcessExitRequest(bool forced, int exitCode);

    private:
        friend class EngineLoop;
        static Application* s_ApplicationInst;

    private:
        bool m_ExitRequested = false;
        int m_ExitCode = 0;

        CommandLineArgs m_CommandLineArgs;
    };
}

// Should be defined in client code.
extern Kitsune::Application* CreateApplication(const Kitsune::CommandLineArgs& cmdLineArgs);
