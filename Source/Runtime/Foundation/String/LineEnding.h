#pragma once

#include "Foundation/Common/Predefined.h"

namespace Kitsune
{
    enum class LineEndingOptions
    {
        LF,
        CRLF
    };

#if defined(KITSUNE_OS_WINDOWS)
    inline constexpr LineEndingOptions NativeLineEnding = LineEndingOptions::CRLF;
#else
    inline constexpr LineEndingOptions NativeLineEnding = LineEndingOptions::LF;
#endif
}
