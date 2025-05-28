#pragma once

#include "Foundation/Templates/IsAnyOf.h"

namespace Kitsune
{
    template<typename T>
    concept Character = IsAnyOf<std::remove_cvref_t<T>, char, wchar_t, char8_t, char16_t, char32_t>;

    template<typename T>
    concept Utf8Character = Character<T> && (sizeof(T) == 1);

    template<typename T>
    concept Utf16Character = Character<T> && (sizeof(T) == 2);

    template<typename T>
    concept Utf32Character = Character<T> && (sizeof(T) == 4);
}
