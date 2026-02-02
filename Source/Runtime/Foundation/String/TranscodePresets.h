#pragma once

#include "Foundation/String/String.h"
#include "Foundation/String/Transcode.h"

#include "Foundation/String/Utf8Encoding.h"
#include "Foundation/String/Utf16Encoding.h"
#include "Foundation/String/Utf32Encoding.h"

#include "Foundation/Iterators/BackInsertIterator.h"

namespace Kitsune
{
    template<TextEncoding InEncoding, TextEncoding OutEncoding>
    inline auto TranscodeString(
        BasicStringView<typename InEncoding::CodeunitType> string)
    {
        BasicString<typename OutEncoding::CodeunitType> outputString;
        auto [newIterator, _] = Transcode<InEncoding, OutEncoding>(
            string.GetBegin(), string.GetEnd(),
            BackInsertIterator<decltype(outputString)>(outputString));

        // Append a unicode "replacement character" to the end of the string, no need
        // for the function to continue.
        if (newIterator != string.GetEnd())
        {
            const char32_t InvalidCharacter = 0xFFFD;
            KITSUNE_UNUSED(InEncoding::EncodeSingle(
                &InvalidCharacter, &InvalidCharacter + 1,
                BackInsertIterator<decltype(outputString)>(outputString)));
        }

        return outputString;
    }

    template<Utf8Character Char8, Utf16Character Char16>
    inline auto Utf8ToUtf16(BasicStringView<Char8> string)
    {
        return TranscodeString<Utf8Encoding<Char8>, Utf16Encoding<Char16>>(string);
    }

    template<Utf8Character Char8, Utf32Character Char32>
    inline auto Utf8ToUtf32(BasicStringView<Char8> string)
    {
        return TranscodeString<Utf8Encoding<Char8>, Utf32Encoding<Char32>>(string);
    }

    template<Utf16Character Char16, Utf8Character Char8>
    inline auto Utf16ToUtf8(BasicStringView<Char16> string)
    {
        return TranscodeString<Utf16Encoding<Char16>, Utf8Encoding<Char8>>(string);
    }

    template<Utf16Character Char16, Utf32Character Char32>
    inline auto Utf16ToUtf32(BasicStringView<Char16> string)
    {
        return TranscodeString<Utf16Encoding<Char16>, Utf32Encoding<Char32>>(string);
    }

    template<Utf32Character Char32, Utf8Character Char8>
    inline auto Utf32ToUtf8(BasicStringView<Char32> string)
    {
        return TranscodeString<Utf32Encoding<Char32>, Utf8Encoding<Char8>>(string);
    }

    template<Utf32Character Char32, Utf16Character Char16>
    inline auto Utf32ToUtf16(BasicStringView<Char32> string)
    {
        return TranscodeString<Utf32Encoding<Char32>, Utf16Encoding<Char16>>(string);
    }
}
