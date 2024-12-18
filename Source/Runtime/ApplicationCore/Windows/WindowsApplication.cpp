#include "ApplicationCore/Application.h"

#include <cstring>
#include <Windows.h>

namespace Kitsune
{
    void Application::ProcessExitRequest(bool forced, int exitCode)
    {
        if (forced)
        {
            // TODO: Check whether this code is relevant, I'm worried that
            // without this memcpy, the exit code will be truncated if it was negative.
            UINT unsignedCode;
            std::memcpy(&unsignedCode, &exitCode, sizeof(unsignedCode));

            ::TerminateProcess(GetCurrentProcess(), unsignedCode);
        }
        else
        {
            ::PostQuitMessage(exitCode);
        }
    }
}
