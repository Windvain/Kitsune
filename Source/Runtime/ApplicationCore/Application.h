#pragma once

#include "Foundation/String/String.h"
#include "ApplicationCore/CommandLineArguments.h"

namespace Kitsune
{
    struct ApplicationSpecs
    {
        String Name;
    };

    class Application
    {
    public:
        KITSUNE_API_ Application(const ApplicationSpecs& specs);
        KITSUNE_API_ virtual ~Application();

    public:
        virtual void Update() { /* ... */ }

    public:
        inline bool IsExitRequested() const { return m_ExitRequested; }

        KITSUNE_API_ void Exit(int exitCode);
        KITSUNE_API_ void ForceExit(int exitCode);

    private:
        static Application* s_Instance;

        ApplicationSpecs m_ApplicationSpecs;
        bool m_ExitRequested = false;
    };

    // Should be defined in client code.
    extern Kitsune::Application* CreateApplication(const Kitsune::CommandLineArguments&);
}
