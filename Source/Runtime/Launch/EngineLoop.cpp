#include "Launch/EngineLoop.h"

#include "ApplicationCore/Application.h"
#include "Foundation/Memory/Memory.h"

namespace Kitsune
{
    EngineLoop::EngineLoop(int argc, char** argv)
    {
        /* Subsystem Initialization */
        Memory::InitializeExplicit();

        CommandLineArgs cmdLineArgs = { argc, argv };

        m_Application = CreateApplication(cmdLineArgs);
        m_Application->m_CommandLineArgs = cmdLineArgs;

        m_Application->OnStart();
    }

    EngineLoop::~EngineLoop()
    {
        m_Application->OnExit();
        Memory::Delete(m_Application);

        /* Subsystem Shutdown */
        Memory::Shutdown();
    }

    void EngineLoop::Tick()
    {
        m_Application->OnUpdate();
    }

    bool EngineLoop::IsExitRequested() const
    {
        return m_Application->IsExitRequested();
    }

    int EngineLoop::GetExitCode() const
    {
        return m_Application->m_ExitCode;
    }
}
