#include "ApplicationCore/Application.h"

#include <Windows.h>

namespace Kitsune
{
    void Application::ProcessExitRequest(bool forced, int exitCode)
    {
        if (forced)
            ::TerminateProcess(GetCurrentProcess(), static_cast<UINT>(exitCode));
        else
            ::PostQuitMessage(exitCode);
    }
}
