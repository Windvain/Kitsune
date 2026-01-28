#pragma once

#include "Foundation/Common/Types.h"
#include "Foundation/String/Encoding.h"

#include "Foundation/Concepts/Character.h"

namespace Kitsune
{
    // An abstract representation of the UTF-32 encoding. it is a fixed-length encoding
    // which means that it can be encoded into and decoded from extremely quickly, at the price
    // of a larger memory requirement.
    template<Utf32Character T>
    class Utf32Encoding
    {
    public:
        using CodepointType = Uint32;
        using CodeunitType = T;

    public:
        static constexpr CodepointType MaxCodepointValue = 0x10FFFF;
        static constexpr Usize MaxCodeunits = 1;

    public:
        template<ForwardIterator InputIter, OutputIterator<CodepointType> OutputIter>
        inline static DecodeResult<InputIter, OutputIter> DecodeSingle(
            InputIter begin, InputIter end,
            OutputIter outBegin)
        {
            using Result = DecodeResult<InputIter, OutputIter>;
            if (begin == end)
                return Result(begin, outBegin);

            if ((*begin > MaxCodepointValue) || ((*begin >= 0xD800) && (*begin <= 0xDFFF)))
                return Result(begin, outBegin);

            *outBegin = static_cast<CodepointType>(*begin);
            return Result(++begin, ++outBegin);
        }

        template<ForwardIterator InputIter, OutputIterator<CodeunitType> OutputIter>
        inline static InputIter Encode(InputIter begin, InputIter end,
                                       OutputIter outBegin)
        {
            using Result = EncodeResult<InputIter, OutputIter>;
            if (begin == end)
                return Result(begin, outBegin);

            if ((*begin > MaxCodepointValue) || ((*begin >= 0xD800) && (*begin <= 0xDFFF)))
                return Result(begin, outBegin);

            *outBegin = static_cast<CodeunitType>(*begin);
            return Result(++begin, ++outBegin);
        }
    };
}
