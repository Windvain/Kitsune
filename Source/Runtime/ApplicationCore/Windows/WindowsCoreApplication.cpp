#include "ApplicationCore/CoreApplication.h"
#include <Windows.h>

#include "Foundation/String/UnicodeConversion.h"
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

        WideString wideTitle;
        Unicode::Convert(props.Title.GetBegin(), props.Title.GetEnd(),
                         BackInsertIterator<WideString>(wideTitle));

        return MakeShared<WindowsWindow>(props.Size.x, props.Size.y,
                                         props.Position.x, props.Position.y,
                                         wideTitle.Raw(), props.State, props.Flags);
    }
}

KITSUNE_POP_COMPILER_WARNINGS()
