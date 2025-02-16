#pragma once

#include "Foundation/String/String.h"
#include "Foundation/Memory/ScopedPtr.h"

#include "ApplicationCore/CommandLineArguments.h"
#include "ApplicationCore/IPlatformApplication.h"

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
        KITSUNE_API_ void Update();

    public:
        inline bool IsExitRequested() const { return m_PlatformImpl->IsExitRequested(); }

        inline void Exit(int exitCode)      { return m_PlatformImpl->Exit(exitCode); }
        inline void ForceExit(int exitCode) { return m_PlatformImpl->ForceExit(exitCode); }

        inline int GetExitCode()            { return m_PlatformImpl->GetExitCode(); }

    public:
        virtual void OnUpdate() { /* ... */ }

    private:
        static Application* s_Instance;

    private:
        ApplicationSpecs m_ApplicationSpecs;
        ScopedPtr<IPlatformApplication> m_PlatformImpl;
    };

    // Should be defined in client code.
    extern Kitsune::Application* CreateApplication(const Kitsune::CommandLineArguments&);
}
