#include "ApplicationCore/CoreApplication.h"

#include <Windows.h>
#include "Foundation/Windows/StringConversions.h"

#include "ApplicationCore/Windows/WindowsWindow.h"

KITSUNE_PUSH_COMPILER_WARNINGS()
KITSUNE_IGNORE_CLANG_WARNING(-Winvalid-noreturn)

namespace Kitsune
{
    void CoreApplication::Exit(int exitCode)
    {
        m_ExitRequested = true;
        m_ExitCode = exitCode;

        ::PostQuitMessage(exitCode);
    }

    void CoreApplication::ForceExit(int exitCode)
    {
        Exit(exitCode);
        ::TerminateProcess(::GetCurrentProcess(), static_cast<UINT>(exitCode));
    }

    SharedPtr<IWindow> CoreApplication::MakeWindow(const WindowProperties& props)
    {
        VerifyWindowProperties(props);
        WideString wideTitle = Details::WindowsConvertToUtf16(props.Title);

        return MakeShared<WindowsWindow>(props.Size.x, props.Size.y,
                                         props.Position.x, props.Position.y,
                                         wideTitle.Raw(), props.State, props.Flags);
    }
}

KITSUNE_POP_COMPILER_WARNINGS()
