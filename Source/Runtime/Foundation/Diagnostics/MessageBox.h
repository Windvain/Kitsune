#pragma once

#include "Foundation/Common/Types.h"

#include "Foundation/String/String.h"
#include "Foundation/Containers/Array.h"

namespace Kitsune
{
    using MessageBoxButtonId = Uint16;

    enum class MessageBoxIcon
    {
        None    = 0,
        Info    = 1,
        Warning = 2,
        Error   = 3
    };

    struct MessageBoxButton
    {
        MessageBoxButtonId Id;
        String Text;
    };

    struct MessageBoxSpecifications
    {
        String Title;
        String Description;

        MessageBoxIcon Icon = MessageBoxIcon::Info;
        Array<MessageBoxButton> Buttons;
    };

    // Shows a platform-specific native message box with the specifications passed
    // in. If `pressed` is not a null pointer, this function will write the ID
    // of the button pressed by the user into `pressed`.
    // Returns true if the message box was successfully shown, else returns false.
    bool ShowMessageBox(const MessageBoxSpecifications& specs,
                        MessageBoxButtonId* pressed = nullptr);
}
