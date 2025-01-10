#pragma once

#include "Foundation/Templates/IsAnyOf.h"

namespace Kitsune
{
    template<typename T>
    concept Character = IsAnyOf<T, char, wchar_t, char8_t, char16_t, char32_t>;
}
