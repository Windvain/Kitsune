#pragma once

#include "Foundation/String/String.h"
#include "Foundation/String/Transcode.h"

#include "Foundation/String/Utf8Encoding.h"
#include "Foundation/String/Utf16Encoding.h"
#include "Foundation/String/Utf32Encoding.h"

#include "Foundation/Iterators/BackInsertIterator.h"

namespace Kitsune
{
    // Transcodes a string encoded in `InEncoding` into a string encoded
    // with `OutEncoding`.
    template<TextEncoding InEncoding, TextEncoding OutEncoding>
    inline auto TranscodeString(
        BasicStringView<typename InEncoding::CodeunitType> string)
    {
        BasicString<typename OutEncoding::CodeunitType> outputString;
        while (!string.IsEmpty())
        {
            auto [newIterator, newOutIter_] = Transcode<InEncoding, OutEncoding>(
                string.GetBegin(), string.GetEnd(),
                BackInsertIterator<decltype(outputString)>(outputString));

            string.RemovePrefix(newIterator - string.GetBegin());

            // Append a unicode "replacement character" to the end of the string, no need
            // for the function to continue.
            if (newIterator != string.GetEnd())
            {
                const char32_t InvalidCharacter = 0xFFFD;
                KITSUNE_UNUSED(OutEncoding::EncodeSingle(
                    &InvalidCharacter, &InvalidCharacter + 1,
                    BackInsertIterator<decltype(outputString)>(outputString)));

                string.RemovePrefix(1);
            }
        }

        return outputString;
    }

    // Transcodes a string from UTF-8 to UTF-16.
    template<Utf8Character Char8, Utf16Character Char16>
    inline auto Utf8ToUtf16(BasicStringView<Char8> string)
    {
        return TranscodeString<Utf8Encoding<Char8>, Utf16Encoding<Char16>>(string);
    }

    // Transcodes a string from UTF-8 to UTF-32.
    template<Utf8Character Char8, Utf32Character Char32>
    inline auto Utf8ToUtf32(BasicStringView<Char8> string)
    {
        return TranscodeString<Utf8Encoding<Char8>, Utf32Encoding<Char32>>(string);
    }

    // Transcodes a string from UTF-16 to UTF-8.
    template<Utf16Character Char16, Utf8Character Char8>
    inline auto Utf16ToUtf8(BasicStringView<Char16> string)
    {
        return TranscodeString<Utf16Encoding<Char16>, Utf8Encoding<Char8>>(string);
    }

    // Transcodes a string from UTF-16 to UTF-32.
    template<Utf16Character Char16, Utf32Character Char32>
    inline auto Utf16ToUtf32(BasicStringView<Char16> string)
    {
        return TranscodeString<Utf16Encoding<Char16>, Utf32Encoding<Char32>>(string);
    }

    // Transcodes a string from UTF-32 to UTF-8.
    template<Utf32Character Char32, Utf8Character Char8>
    inline auto Utf32ToUtf8(BasicStringView<Char32> string)
    {
        return TranscodeString<Utf32Encoding<Char32>, Utf8Encoding<Char8>>(string);
    }

    // Transcodes a string from UTF-32 to UTF-16.
    template<Utf32Character Char32, Utf16Character Char16>
    inline auto Utf32ToUtf16(BasicStringView<Char32> string)
    {
        return TranscodeString<Utf32Encoding<Char32>, Utf16Encoding<Char16>>(string);
    }
}
