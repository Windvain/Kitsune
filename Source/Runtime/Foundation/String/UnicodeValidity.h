#pragma once

#include "Foundation/String/UnicodeTypes.h"

namespace Kitsune::Unicode
{
    inline bool IsCodepointValid(Codepoint codepoint)
    {
        return (codepoint <= 0x10FFFF);
    }
}
