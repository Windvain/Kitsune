#pragma once

#include "Launch/IEngineLoop.h"
#include "Application/IApplication.h"

namespace Kitsune
{
    class DefaultEngineLoop : public IEngineLoop
    {
    public:
        DefaultEngineLoop();
        ~DefaultEngineLoop();

    public:
        bool Initialize(int argc, char** argv) override;
        int Run() override;

        void Shutdown() override;
        void Notify(EngineLoopNotification notification) override;

    public:
        void Exit(int exitCode);
        [[noreturn]] void ForceExit(int exitCode);

        const CommandLineArguments& GetCommandLineArguments() const
        {
            return m_CommandLineArguments;
        }

    public:
        inline static DefaultEngineLoop* GetInstance()
        {
            return s_Instance;
        }

    private:
        void PlatformUpdate();

    private:
        static DefaultEngineLoop* s_Instance;

    private:
        IApplication* m_Application;
        CommandLineArguments m_CommandLineArguments;

        bool m_ExitRequested = false;
        int m_ExitCode;
    };
}
