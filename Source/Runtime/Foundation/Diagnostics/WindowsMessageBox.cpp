#include "Foundation/Diagnostics/MessageBox.h"

#include <Windows.h>
#include <CommCtrl.h>

#include "Foundation/String/InvalidUnicodeException.h"

namespace Kitsune
{
    WideString ConvertToUtf16(const StringView string)
    {
        int wideSize = ::MultiByteToWideChar(CP_UTF8, 0, string.Data(), string.Size(), nullptr, 0);
        if (wideSize == ERROR_NO_UNICODE_TRANSLATION)
            throw InvalidUnicodeException();

        WideString wideString(wideSize, '\0');
        ::MultiByteToWideChar(CP_UTF8, 0, string.Data(), string.Size(), wideString.Data(), wideSize);

        return wideString;
    }

    bool ShowMessageBox(const MessageBoxSpecs& specs, MessageBoxButtonId* pressed)
    {
        TASKDIALOGCONFIG config;
        ::ZeroMemory(&config, sizeof(config));

        WideString wideTitle = ConvertToUtf16(specs.Title);
        WideString wideDescription = ConvertToUtf16(specs.Description);

        Array<WideString> buttonTexts(specs.Buttons.Size());
        Array<TASKDIALOG_BUTTON> buttons(specs.Buttons.Size());

        for (const MessageBoxButton& button : specs.Buttons)
        {
            buttonTexts.PushBack(ConvertToUtf16(button.Text));

            TASKDIALOG_BUTTON nativeButton;
            nativeButton.nButtonID = button.Id;
            nativeButton.pszButtonText = buttonTexts.Back().Data();

            buttons.PushBack(nativeButton);
        }

        config.cbSize = sizeof(config);
        config.pszWindowTitle = wideTitle.Raw();
        config.pszContent = wideDescription.Raw();

        config.pButtons = buttons.Data();
        config.cButtons = buttons.Size();

        switch (specs.Icon)
        {
        case MessageBoxIcon::None:    config.pszMainIcon = nullptr;             break;
        case MessageBoxIcon::Info:    config.pszMainIcon = TD_INFORMATION_ICON; break;
        case MessageBoxIcon::Warning: config.pszMainIcon = TD_WARNING_ICON;     break;
        case MessageBoxIcon::Error:   config.pszMainIcon = TD_ERROR_ICON;       break;
        }

        return ::TaskDialogIndirect(&config, pressed, nullptr, nullptr);
    }
}
