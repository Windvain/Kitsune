#pragma once

#include "Foundation/String/Encoding.h"
#include "Foundation/Iterators/Iterator.h"

namespace Kitsune
{
    // Encodes codepoints in the range [begin, end] into the specified encoding.
    template<TextEncoding Encoding,
             ForwardIterator InputIter,
             OutputIterator<typename Encoding::CodeunitType> OutputIter>
    inline EncodeResult<InputIter, OutputIter> Encode(InputIter begin, InputIter end,
                                                      OutputIter outBegin)
    {
        using Result = EncodeResult<InputIter, OutputIter>;
        while (begin != end)
        {
            auto [newBegin, newOutBegin] = Encoding::EncodeSingle(begin, end, outBegin);
            if (newBegin == begin)
                return Result(newBegin, newOutBegin);

            begin = newBegin;
            outBegin = newOutBegin;
        }

        return Result(begin, outBegin);
    }
}
