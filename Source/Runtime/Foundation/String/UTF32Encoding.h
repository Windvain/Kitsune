#pragma once

#include "Foundation/Common/Types.h"
#include "Foundation/String/Encoding.h"

#include "Foundation/Concepts/Character.h"

namespace Kitsune
{
    // An abstract representation of the UTF-32 encoding. it is a fixed-length
    // encoding, which means that it can be encoded into and decoded from relatively
    // quickly, at the price of a larger memory footprint.
    template<UTF32Character T>
    class UTF32Encoding
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
            return 1;
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

            if ((*begin > MaxCodepointValue()) ||
                ((*begin >= 0xD800) && (*begin <= 0xDFFF)))
            {
                return Result(begin, outBegin);
            }

            *outBegin = static_cast<CodepointType>(*begin);
            return Result(++begin, ++outBegin);
        }

        template<ForwardIterator InputIter,
                 OutputIterator<CodeunitType> OutputIter>
        inline static EncodeResult<InputIter, OutputIter> EncodeSingle(
            InputIter begin, InputIter end,
            OutputIter outBegin)
        {
            using Result = EncodeResult<InputIter, OutputIter>;
            if (begin == end)
                return Result(begin, outBegin);

            if ((*begin > MaxCodepointValue()) ||
                ((*begin >= 0xD800) && (*begin <= 0xDFFF)))
            {
                return Result(begin, outBegin);
            }

            *outBegin = static_cast<CodeunitType>(*begin);
            return Result(++begin, ++outBegin);
        }
    };
}
