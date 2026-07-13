#pragma once

#include "Foundation/String/String.h"
#include "Foundation/String/Transcode.h"

#include "Foundation/String/UTF8Encoding.h"
#include "Foundation/String/UTF16Encoding.h"
#include "Foundation/String/UTF32Encoding.h"

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
    template<UTF8Character Char8, UTF16Character Char16>
    inline auto UTF8ToUTF16(BasicStringView<Char8> string)
    {
        return TranscodeString<UTF8Encoding<Char8>, UTF16Encoding<Char16>>(string);
    }

    // Transcodes a string from UTF-8 to UTF-32.
    template<UTF8Character Char8, UTF32Character Char32>
    inline auto UTF8ToUTF32(BasicStringView<Char8> string)
    {
        return TranscodeString<UTF8Encoding<Char8>, UTF32Encoding<Char32>>(string);
    }

    // Transcodes a string from UTF-16 to UTF-8.
    template<UTF16Character Char16, UTF8Character Char8>
    inline auto UTF16ToUTF8(BasicStringView<Char16> string)
    {
        return TranscodeString<UTF16Encoding<Char16>, UTF8Encoding<Char8>>(string);
    }

    // Transcodes a string from UTF-16 to UTF-32.
    template<UTF16Character Char16, UTF32Character Char32>
    inline auto UTF16ToUTF32(BasicStringView<Char16> string)
    {
        return TranscodeString<UTF16Encoding<Char16>, UTF32Encoding<Char32>>(string);
    }

    // Transcodes a string from UTF-32 to UTF-8.
    template<UTF32Character Char32, UTF8Character Char8>
    inline auto UTF32ToUTF8(BasicStringView<Char32> string)
    {
        return TranscodeString<UTF32Encoding<Char32>, UTF8Encoding<Char8>>(string);
    }

    // Transcodes a string from UTF-32 to UTF-16.
    template<UTF32Character Char32, UTF16Character Char16>
    inline auto UTF32ToUTF16(BasicStringView<Char32> string)
    {
        return TranscodeString<UTF32Encoding<Char32>, UTF16Encoding<Char16>>(string);
    }
}
