#pragma once

#include "Foundation/Containers/Pair.h"

#include "Foundation/String/LineEnding.h"
#include "Foundation/String/StringView.h"

namespace Kitsune
{
    // An abstract representation of the UTF-8 encoding. This is the main encoding
    // used in most software and all of the content on the Web. Takes up less space
    // at the cost of computational speed.
    template<UTF8Character T>
    class UTF8Encoding
    {
    public:
        using CodepointType = Uint32;
        using CodeunitType = T;

    public:
        [[nodiscard]]
        static constexpr CodepointType MaxCodepointValue()
        {
            return 0x10FFFF;
        }

        [[nodiscard]]
        static constexpr Usize MaxCodeunits()
        {
            return 4;
        }

    public:
        [[nodiscard]]
        static constexpr BasicStringView<T> GetLineEnding(LineEndingOptions options)
        {
            // No constexpr ternary operator, no can do.
            BasicStringView<T> lineEnding;
            if constexpr (std::is_same_v<T, char8_t>)
                lineEnding = u8"\r\n";
            else if constexpr (std::is_same_v<T, char>)
                lineEnding = "\r\n";
            else
                KITSUNE_UNREACHABLE();

            if (options == LineEndingOptions::LF)
                lineEnding.RemovePrefix(1);

            return lineEnding;
        }

    public:
        [[nodiscard]]
        static constexpr BasicStringView<T> GetPreamble()
        {
            // The Unicode Consortium recommends to not append a BOM for UTF-8 files to
            // keep backwards compatibility with ASCII.
            if constexpr (std::is_same_v<T, char8_t>)
                return U8StringView();
            else if constexpr (std::is_same_v<T, char>)
                return StringView();

            KITSUNE_UNREACHABLE();
        }

        [[nodiscard]]
        static constexpr BasicStringView<T> GetReplacement()
        {
            if constexpr (std::is_same_v<T, char8_t>)
                return U8StringView(u8"\uFFFD");
            else if constexpr (std::is_same_v<T, char>)
                return StringView("\xEF\xBF\xBD");
        }

    public:
        template<ForwardIterator Iter, OutputIterator<CodepointType> OutIter>
        inline static Pair<Iter, OutIter> Decode(Iter begin, Iter end, OutIter outBegin)
        {
            if (begin == end)
                return { begin, outBegin };

            if (static_cast<Uint8>(*begin) <= Uint8(0x7F))
            {
                *outBegin = static_cast<CodepointType>(*begin);
                return { ++begin, ++outBegin };
            }

            Iter header = begin++;
            Uint8 headerVal = *header;

            if (IsContinuationByte(headerVal) || IsInvalidByte(headerVal))
                return { header, outBegin };

            Usize trailingBytes = GetTrailingBytes(headerVal);
            CodepointType codepoint = 0;

            for (Usize index = 0; index < trailingBytes; ++index, ++begin)
            {
                // Invalid byte/the string ending before the end of the character.
                if ((begin == end) || IsInvalidByte(*header))
                    return { header, outBegin };

                // Checks for overlong encodings. No need to check for continuation
                // bytes, because we check it together w/ overlong encodings.
                // Ex: U+0020 being represented as 0xC0 0xA0 instead of 0x20.
                Uint8 lowEnd = 0x80, highEnd = 0xBF;
                if (index == 0)
                {
                    // Comparisons only work when both operands are of the same sign.
                    if (headerVal == Uint8(0xE0))      lowEnd = Uint8(0xA0);
                    else if (headerVal == Uint8(0xED)) highEnd = Uint8(0x9F);
                    else if (headerVal == Uint8(0xF0)) lowEnd = Uint8(0x90);
                    else if (headerVal == Uint8(0xF4)) highEnd = Uint8(0x8F);
                }

                Uint8 beginVal = *begin;
                if ((beginVal < lowEnd) || (beginVal > highEnd))
                    return { header, outBegin };

                codepoint <<= 6;
                codepoint |= (beginVal & Uint8(0x3F));
            }

            Uint8 mask = (Uint8(0x1F) >> (trailingBytes - 1));
            codepoint |= CodepointType(headerVal & mask) << (trailingBytes * 6);

            *outBegin++ = codepoint;
            return { begin, outBegin };
        }

        template<ForwardIterator Iter, OutputIterator<CodeunitType> OutIter>
        inline static Pair<Iter, OutIter> Encode(Iter begin, Iter end, OutIter outBegin)
        {
            if (begin == end)
                return { begin, outBegin };

            if ((*begin > MaxCodepointValue()) ||
                ((*begin >= 0xD800) && (*begin <= 0xDFFF)))
            {
                return { begin, outBegin };
            }

            if (*begin <= 0x7F)
                *outBegin = static_cast<CodepointType>(*begin);
            else if (*begin <= 0x7FF)
            {
                *outBegin = 0xC0 + (*begin >> 6);
                *++outBegin = 0x80 + (*begin & 0x3F);
            }
            else if (*begin <= 0xFFFF)
            {
                *outBegin = 0xE0 + (*begin >> 12);
                *++outBegin = 0x80 + ((*begin >> 6) & 0x3F);
                *++outBegin = 0x80 + (*begin & 0x3F);
            }
            else if (*begin <= 0x10FFFF)
            {
                *outBegin = 0xF0 + (*begin >> 18);
                *++outBegin = 0x80 + ((*begin >> 12) & 0x3F);
                *++outBegin = 0x80 + ((*begin >> 6) & 0x3F);
                *++outBegin = 0x80 + (*begin & 0x3F);
            }
            else
            {
                return { begin, outBegin };
            }

            return { ++begin, ++outBegin };
        }

    private:
        inline static bool IsContinuationByte(Uint8 byte)
        {
            return ((byte & Uint8(0xC0)) == Uint8(0x80));
        }

        inline static bool IsInvalidByte(char8_t byte)
        {
            // Uses of bytes ranging from 0xC0 - 0xC1 and 0xF5 - 0xFF are disallowed.
            return ((byte == Uint8(0xC0)) ||
                    (byte == Uint8(0xC1)) ||
                    ((byte >= Uint8(0xF5)) && (byte <= Uint8(0xFF))));
        }

        inline static Usize GetTrailingBytes(char8_t byte)
        {
            // Only meant for non-ASCII characters.
            Usize trailing = 0;
            for (; (byte & Uint8(0x80)) != 0; byte <<= 1, ++trailing);

            return trailing - 1;
        }
    };
}
