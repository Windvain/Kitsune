#include "ApplicationCore/Application.h"
#include <Windows.h>

namespace Kitsune
{
    void Application::Exit(int exitCode)
    {
        m_ExitRequested = true;
        ::PostQuitMessage(exitCode);
    }

    void Application::ForceExit(int exitCode)
    {
        Exit(exitCode);
        ::TerminateProcess(::GetCurrentProcess(), static_cast<UINT>(exitCode));
    }
}
