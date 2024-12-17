#include "Launch/EngineLoop.h"

#include "ApplicationCore/Application.h"

namespace Kitsune
{
    EngineLoop::EngineLoop(int argc, char** argv)
    {
        /* Subsystem Initialization */

        CommandLineArgs cmdLineArgs = { argc, argv };

        m_Application = CreateApplication(cmdLineArgs);
        m_Application->m_CommandLineArgs = cmdLineArgs;
    }

    EngineLoop::~EngineLoop()
    {
    }

    void EngineLoop::Tick()
    {
        /* ... */
    }

    bool EngineLoop::IsExitRequested() const
    {
        return m_Application->IsExitRequested();
    }
}
