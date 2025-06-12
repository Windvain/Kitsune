#pragma once

#include "Application/Application.h"

#include "Foundation/Memory/ScopedPtr.h"
#include "Foundation/Memory/SharedPtr.h"

#include "Foundation/Logging/Logger.h"
#include "Foundation/Logging/ConsoleStream.h"

namespace Kitsune
{
    class EngineLoop
    {
    public:
        EngineLoop(int argc, char** argv);
        ~EngineLoop();

    public:
        void PreInitialize();
        void Initialize(int argc, char** argv);

        void Run();

        int PreShutdown();
        void Shutdown();

    public:
        inline Application* GetApplicationInstance() const
        {
            return m_Application;
        }

    private:
        bool m_IsPreInitialized = false;
        bool m_IsFullyInitialized = false;

        SharedPtr<Logger> m_GlobalLogger;
        Application* m_Application;
    };
}
