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

    bool ShowMessageBox(const MessageBoxSpecifications& specs,
                        MessageBoxButtonId* pressed = nullptr);
}
