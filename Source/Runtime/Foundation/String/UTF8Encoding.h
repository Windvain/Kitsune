#pragma once

#include "Foundation/Common/Types.h"
#include "Foundation/String/Encoding.h"

#include "Foundation/Concepts/Character.h"
#include "Foundation/Algorithms/Distance.h"

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
        constexpr static CodepointType MaxCodepointValue()
        {
            return 0x10FFFF;
        }

        constexpr static Usize MaximumCodeunits()
        {
            return 4;
        }

    public:
        template<ForwardIterator InputIter,
                 OutputIterator<CodepointType> OutputIter>
        inline static DecodeResult<InputIter, OutputIter> DecodeSingle(
            InputIter begin, InputIter end,
            OutputIter outBegin)
        {
            using Result = DecodeResult<InputIter, OutputIter>;
            if (begin == end)
                return Result(begin, outBegin);

            auto rangeSize = Algorithms::Distance(begin, end);
            auto currentByte = static_cast<char8_t>(*begin);

            // Fast path for ASCII characters.
            if (currentByte <= 0x7F)
                *outBegin = static_cast<CodepointType>(currentByte);
            else
            {
                CodepointType codepoint = 0;
                auto currentIter = begin;

                // The first byte should be in the range [0x00, 0x7F] U [0xC2, 0xF4]
                if ((currentByte < 0xC2) || (currentByte > 0xF4))
                    return Result(currentIter, outBegin);

                typename IteratorTraits<InputIter>::DifferenceType byteCount = 0;
                for (char8_t copy = currentByte; (copy & 0x80) != 0;
                     copy <<= 1, ++byteCount)
                {
                }

                if (rangeSize < byteCount)
                    return Result(currentIter, outBegin);

                codepoint |= currentByte & (0xFF >> (byteCount + 1));
                for (auto iterations = 1; iterations != byteCount; ++iterations)
                {
                    char8_t lowEnd = 0x80;
                    char8_t highEnd = 0xBF;

                    // The second byte has specific constraints depending on the first
                    // byte's value.
                    currentByte = static_cast<char8_t>(*++begin);
                    if (iterations == 1)
                    {
                        if (currentByte == 0xE0)      lowEnd = 0xA0;
                        else if (currentByte == 0xED) highEnd = 0x9F;
                        else if (currentByte == 0xF0) lowEnd = 0x90;
                        else if (currentByte == 0xF4) highEnd = 0x8F;
                    }

                    if ((currentByte < lowEnd) || (currentByte > highEnd))
                        return Result(currentIter, outBegin);

                    codepoint <<= 6;
                    codepoint |= (currentByte & 0x3F);
                }

                *outBegin = codepoint;
            }

            return Result(++begin, ++outBegin);
        }

        template<ForwardIterator InputIter,
                 OutputIterator<CodeunitType> OutputIter>
        inline static EncodeResult<InputIter, OutputIter> EncodeSingle(
            InputIter begin, InputIter end, OutputIter outBegin)
        {
            using Result = EncodeResult<InputIter, OutputIter>;
            if (begin == end)
                return Result(begin, outBegin);

            if ((*begin > MaxCodepointValue()) ||
                ((*begin >= 0xD800) && (*begin <= 0xDFFF)))
            {
                return Result(begin, outBegin);
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
                return Result(begin, outBegin);
            }

            return Result(++begin, ++outBegin);
        }
    };
}
