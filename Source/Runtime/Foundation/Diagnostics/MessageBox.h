#pragma once

#include "Foundation/Common/Types.h"

#include "Foundation/String/String.h"
#include "Foundation/Containers/Array.h"

namespace Kitsune
{
    using MessageBoxButtonId = Uint16;

    // Specifies the icon that is shown on the created message box.
    // The visuals of the shown icon is dependent on the operating system.
    enum class MessageBoxIcon
    {
        None,       //< No icon will be shown.
        Info,       //< An info icon will be shown.
        Warning,    //< A warning icon will be shown.
        Error       //< An error icon will be shown.
    };

    // Represents a button on the created message box.
    struct MessageBoxButton
    {
        MessageBoxButtonId Id;
        String Text;
    };

    // Settings for creating a message box.
    struct MessageBoxSpecifications
    {
        String Title;
        String Description;

        MessageBoxIcon Icon = MessageBoxIcon::Info;
        Array<MessageBoxButton> Buttons;
    };

    // Shows a platform native message box.
    // If `pressed` is not a null pointer, this function will write the ID of the button
    // pressed by the user into it.
    // Returns true if the message box was successfully shown, else returns false.
    bool ShowMessageBox(const MessageBoxSpecifications& specs,
                        MessageBoxButtonId* pressed = nullptr);
}
