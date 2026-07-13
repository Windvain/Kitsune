#include "Foundation/Diagnostics/MessageBox.h"

#include <Windows.h>
#include <CommCtrl.h>

#include "Foundation/Containers/Array.h"
#include "Foundation/String/TranscodePresets.h"

namespace Kitsune
{
    Pair<bool, MessageBoxButtonID> ShowFallbackMessageBox(
        const MessageBoxSpecifications& specs)
    {
        // TODO: Unimplemented. Will implement once I iron out the needed changes.
        KITSUNE_UNUSED(specs);
        return { false, 0 };
    }

    Pair<bool, MessageBoxButtonID> ShowMessageBox(
        const MessageBoxSpecifications& specs)
    {
        // As of now, Windows will always load versions <6.0, which doesn't have
        // TaskDialogIndirect(). Applications will have to use an application
        // manifest to load the correct version.
        HMODULE comctl32 = ::LoadLibraryW(L"comctl32.dll");
        if (comctl32 == nullptr)
            return ShowFallbackMessageBox(specs);

        using TaskDialogIndirectProc =
            HRESULT (*)(const TASKDIALOGCONFIG*, int*, int*, BOOL*);

        auto taskDialogIndirect = (TaskDialogIndirectProc)(void*)(
            ::GetProcAddress(comctl32, "TaskDialogIndirect"));

        if (taskDialogIndirect == nullptr)
        {
            ::FreeLibrary(comctl32);
            return ShowFallbackMessageBox(specs);
        }

        // The actual message box code.
        TASKDIALOGCONFIG config;
        ::ZeroMemory(&config, sizeof(config));

        auto wideTitle = Utf8ToUtf16<char, wchar_t>(specs.Title);
        auto wideDescription = Utf8ToUtf16<char, wchar_t>(specs.Description);

        Array<WideString> buttonTexts(specs.Buttons.Size());
        Array<TASKDIALOG_BUTTON> buttons(specs.Buttons.Size());

        for (const MessageBoxButton& button : specs.Buttons)
        {
            buttonTexts.PushBack(Utf8ToUtf16<char, wchar_t>(button.Text));

            TASKDIALOG_BUTTON nativeButton;
            nativeButton.nButtonID = IDCONTINUE + button.Id + 1;
            nativeButton.pszButtonText = buttonTexts.Back().Data();

            buttons.PushBack(nativeButton);
        }

        config.cbSize = sizeof(config);

        config.pszWindowTitle = wideTitle.Raw();
        config.pszContent = wideDescription.Raw();

        config.pButtons = buttons.Data();
        config.cButtons = static_cast<UINT>(buttons.Size());

        switch (specs.Icon)
        {
        case MessageBoxIcon::None:
            config.pszMainIcon = nullptr;
            break;
        case MessageBoxIcon::Info:
            config.pszMainIcon = TD_INFORMATION_ICON;
            break;
        case MessageBoxIcon::Warning:
            config.pszMainIcon = TD_WARNING_ICON;
            break;
        case MessageBoxIcon::Error:
            config.pszMainIcon = TD_ERROR_ICON;
            break;
        }

        int internalPressed;
        HRESULT result = taskDialogIndirect(
            &config, &internalPressed, nullptr, nullptr);

        ::FreeLibrary(comctl32);

        if (FAILED(result))
            return { false, 0 };

        // HACK: IDCANCEL, IDABORT, and all other predefined IDs cause unintended
        // consequences. Offset the button ID internally to avoid this issue.
        // IDCONTINUE is used as the offset because it has the largest value out
        // of all of the predefined button IDs.
        //
        // If no buttons were specified, then the message box automatically adds
        // an Ok button.
        Uint16 pressedId = (!buttons.IsEmpty()) ?
            internalPressed - (IDCONTINUE + 1) :
            0;

        return { true, pressedId };
    }
}
