#pragma once

#include "Foundation/String/String.h"

#include "Foundation/Containers/Pair.h"
#include "Foundation/Containers/Array.h"

namespace Kitsune
{
    using MessageBoxButtonID = Uint16;

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
        MessageBoxButtonID Id;
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
    // Returns a pair of values, the former is true when the message box
    // was successfully shown, else it is false. The latter returns the ID
    // of the button pressed by the user.
    KITSUNE_API Pair<bool, MessageBoxButtonID> ShowMessageBox(
        const MessageBoxSpecifications& specs);
}
