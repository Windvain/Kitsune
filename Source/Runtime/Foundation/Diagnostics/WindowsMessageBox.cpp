#include "Foundation/Diagnostics/MessageBox.h"

#include <Windows.h>
#include <CommCtrl.h>

#include "Foundation/Windows/StringConversions.h"

namespace Kitsune
{
    bool ShowMessageBox(const MessageBoxSpecs& specs, MessageBoxButtonId* pressed)
    {
        using namespace Internal;

        TASKDIALOGCONFIG config;
        ::ZeroMemory(&config, sizeof(config));

        WideString wideTitle = WindowsConvertToUtf16(specs.Title);
        WideString wideDescription = WindowsConvertToUtf16(specs.Description);

        Array<WideString> buttonTexts(specs.Buttons.Size());
        Array<TASKDIALOG_BUTTON> buttons(specs.Buttons.Size());

        for (const MessageBoxButton& button : specs.Buttons)
        {
            buttonTexts.PushBack(WindowsConvertToUtf16(button.Text));

            TASKDIALOG_BUTTON nativeButton;
            nativeButton.nButtonID = button.Id;
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

        return ::TaskDialogIndirect(&config, pressed, nullptr, nullptr);
    }
}
