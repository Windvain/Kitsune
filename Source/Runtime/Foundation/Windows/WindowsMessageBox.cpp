#include "Foundation/Diagnostics/MessageBox.h"

#include <Windows.h>
#include <CommCtrl.h>

#include "Foundation/Containers/Array.h"
#include "Foundation/String/UnicodeConversion.h"

namespace Kitsune
{
    bool ShowFallbackMessageBox(const MessageBoxSpecifications& specs, MessageBoxButtonId* pressed)
    {
        // Unimplemented. Will implement once I iron out the needed changes.
        KITSUNE_UNUSED(specs);
        KITSUNE_UNUSED(pressed);

        return false;
    }

    bool ShowMessageBox(const MessageBoxSpecifications& specs, MessageBoxButtonId* pressed)
    {
        HMODULE comctl32 = ::LoadLibraryW(L"comctl32.dll");
        if (comctl32 == nullptr)
            return ShowFallbackMessageBox(specs, pressed);

        using TaskDialogIndirectProc = HRESULT (*)(const TASKDIALOGCONFIG*, int*, int*, BOOL*);
        auto taskDialogIndirect = (TaskDialogIndirectProc)(void*)(::GetProcAddress(comctl32, "TaskDialogIndirect"));

        if (taskDialogIndirect == nullptr)
        {
            ::FreeLibrary(comctl32);
            return ShowFallbackMessageBox(specs, pressed);
        }

        TASKDIALOGCONFIG config;
        ::ZeroMemory(&config, sizeof(config));

        WideString wideTitle = Unicode::ConvertString<char, wchar_t>(specs.Title);
        WideString wideDescription = Unicode::ConvertString<char, wchar_t>(specs.Description);

        Array<WideString> buttonTexts(specs.Buttons.Size());
        Array<TASKDIALOG_BUTTON> buttons(specs.Buttons.Size());

        for (const MessageBoxButton& button : specs.Buttons)
        {
            buttonTexts.PushBack(Unicode::ConvertString<char, wchar_t>(button.Text));

            TASKDIALOG_BUTTON nativeButton;
            nativeButton.nButtonID = IDCONTINUE + button.Id + 1;        // Look at the comment below.
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
        case MessageBoxIcon::None:    config.pszMainIcon = nullptr;             break;
        case MessageBoxIcon::Info:    config.pszMainIcon = TD_INFORMATION_ICON; break;
        case MessageBoxIcon::Warning: config.pszMainIcon = TD_WARNING_ICON;     break;
        case MessageBoxIcon::Error:   config.pszMainIcon = TD_ERROR_ICON;       break;
        }

        int internalPressed;
        HRESULT result = taskDialogIndirect(&config, &internalPressed, nullptr, nullptr);

        ::FreeLibrary(comctl32);

        if (FAILED(result))
            return false;

        // HACK: IDCANCEL, IDABORT, and all other predefined IDs cause unintended consequences.
        // Offset the button ID internally to avoid this issue. Why IDCONTINUE, you might ask?
        // Because it has the largest value out of all of the predefined button IDs.
        //
        // If no buttons were specified, then the message box automatically adds an Ok button.
        if ((pressed != nullptr) && !buttons.IsEmpty())
            *pressed = internalPressed - (IDCONTINUE + 1);

        return true;
    }
}
