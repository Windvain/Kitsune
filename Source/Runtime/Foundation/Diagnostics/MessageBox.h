#pragma once

#include "Foundation/Common/Types.h"
#include "Foundation/Common/Macros.h"

#include "Foundation/String/String.h"
#include "Foundation/Containers/Array.h"

namespace Kitsune
{
    using MessageBoxButtonId = Int32;

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

    struct MessageBoxSpecs
    {
        String Title;
        String Description;

        MessageBoxIcon Icon = MessageBoxIcon::Info;
        Array<MessageBoxButton> Buttons;
    };

    KITSUNE_API_ bool ShowMessageBox(const MessageBoxSpecs& specs, MessageBoxButtonId* pressed = nullptr);
}
