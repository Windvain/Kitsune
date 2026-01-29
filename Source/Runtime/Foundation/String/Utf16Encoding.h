#pragma once

#include "Foundation/Common/Types.h"
#include "Foundation/String/Encoding.h"

#include "Foundation/Concepts/Character.h"
#include "Foundation/Algorithms/Distance.h"

namespace Kitsune
{
    // An abstract representation of the UTF-16 encoding. Fairly rare to see this encoding
    // out in the wild except for when dealing with the Win32 API or the Qt libraries.
    template<Utf16Character T>
    class Utf16Encoding
    {
    public:
        using CodepointType = Uint32;
        using CodeunitType = T;

    public:
        inline static CodepointType MaxCodepointValue()
        {
            return 0x10FFFF;
        }

    public:
        template<ForwardIterator InputIter, OutputIterator<CodepointType> OutputIter>
        inline static DecodeResult<InputIter, OutputIter> DecodeSingle(
            InputIter begin, InputIter end,
            OutputIter outBegin)
        {
            using Result = DecodeResult<InputIter, OutputIter>;
            if (begin == end)
                return Result(begin, outBegin);

            auto originalBegin = begin;

            auto distance = Algorithms::Distance(begin, end);
            auto character = static_cast<char16_t>(*begin);

            // Low surrogates cannot come before a high surrogate, this string
            // is invalid.
            if ((character & 0xFC00) == 0xDC00)
                return Result(originalBegin, outBegin);

            if ((character & 0xFC00) != 0xD800)
                *outBegin = static_cast<CodepointType>(*begin);
            else
            {
                if (distance <= 1)
                    return Result(originalBegin, outBegin);

                CodepointType codepoint = ((*begin - 0xD800) << 10);
                character = static_cast<char16_t>(*++begin);

                if ((character & 0xFC00) != 0xDC00)
                    return Result(originalBegin, outBegin);

                codepoint |= (*begin - 0xDC00);
                codepoint += 0x10000;

                *outBegin = codepoint;
            }

            return Result(++begin, ++outBegin);
        }

        template<ForwardIterator InputIter, OutputIterator<CodeunitType> OutputIter>
        inline static EncodeResult<InputIter, OutputIter> EncodeSingle(
            InputIter begin, InputIter end, OutputIter outBegin)
        {
            using Result = EncodeResult<InputIter, OutputIter>;
            if (begin == end)
                return Result(begin, outBegin);

            if ((*begin > MaxCodepointValue) || ((*begin >= 0xD800) && (*begin <= 0xDFFF)))
                return Result(begin, outBegin);

            if (*begin <= 0xFFFF)
                *outBegin = static_cast<T>(*begin);
            else
            {
                CodepointType codepoint = *begin;
                codepoint -= 0x10000;

                *outBegin = 0xD800 + (codepoint >> 10);
                *++outBegin = 0xDC00 + (codepoint & 0x3FF);
            }

            return Result(++begin, ++outBegin);
        }
    };
}
