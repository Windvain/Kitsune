#pragma once

#include "Foundation/String/Encoding.h"
#include "Foundation/Iterators/Iterator.h"

namespace Kitsune
{
    // Decodes a range of characters into codepoints with the specified encoding.
    template<TextEncoding Encoding,
             ForwardIterator InputIter,
             OutputIterator<typename Encoding::CodeunitType> OutputIter>
    inline DecodeResult<InputIter, OutputIter> Decode(InputIter begin, InputIter end,
                                                      OutputIter outBegin)
    {
        using Result = DecodeResult<InputIter, OutputIter>;
        while (begin != end)
        {
            auto [newBegin, newOutBegin] = Encoding::DecodeSingle(
                begin, end, outBegin);

            if (newBegin == begin)
                return Result(newBegin, newOutBegin);

            begin = newBegin;
            outBegin = newOutBegin;
        }

        return Result(begin, outBegin);
    }
}
