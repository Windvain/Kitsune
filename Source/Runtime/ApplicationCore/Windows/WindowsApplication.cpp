#include "ApplicationCore/Windows/WindowsApplication.h"
#include <Windows.h>

namespace Kitsune
{
    void WindowsApplication::Exit(int exitCode)
    {
        m_ExitRequested = true;
        m_ExitCode = exitCode;

        ::PostQuitMessage(exitCode);
    }

    void WindowsApplication::ForceExit(int exitCode)
    {
        Exit(exitCode);
        ::TerminateProcess(::GetCurrentProcess(), static_cast<UINT>(exitCode));
    }

    void WindowsApplication::PollEvents()
    {
        MSG message;
        if (::PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE) != 0)
        {
            ::TranslateMessage(&message);
            ::DispatchMessageW(&message);
        }
    }

    ScopedPtr<IPlatformApplication> IPlatformApplication::CreateApplicationImpl()
    {
        return MakeScoped<WindowsApplication>();
    }
}
