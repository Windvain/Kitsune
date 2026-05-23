#pragma once

#include "Foundation/Meta/RemoveCv.h"
#include "Foundation/Concepts/AnyOf.h"

namespace Kitsune
{
    // Describes a C++ native character type. Could be `char`, `wchar_t`, `char8_t`,
    // `char16_t` or `char32_t`.
    template<typename T>
    concept Character = AnyOf<RemoveCv<T>, char, wchar_t,
                              char8_t, char16_t, char32_t>;

    // Describes a character which has a size of 8 bits, and that can be
    // used for storing UTF-8 code units.
    template<typename T>
    concept Utf8Character = Character<T> && (sizeof(T) == 1);

    // Concept of a character which can be used for storing UTF-16 code
    // units. The character must be exactly 16 bits in size.
    template<typename T>
    concept Utf16Character = Character<T> && (sizeof(T) == 2);

    // Concept of a character which can be used for storing UTF-32 code units. This
    // character must be exactly 32 bits in size.
    template<typename T>
    concept Utf32Character = Character<T> && (sizeof(T) == 4);
}
