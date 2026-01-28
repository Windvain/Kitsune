#pragma once

#include "Foundation/String/Encoding.h"
#include "Foundation/Iterators/Iterator.h"

namespace Kitsune
{
    namespace Details
    {
        template<typename Iter, typename Encoding>
        concept ForwardIteratorToCodepoints =
            ForwardIterator<Iter> &&
            std::same_as<typename IteratorTraits<Iter>::ValueType, typename Encoding::CodepointType>;
    }

    // Encodes codepoints in the range [begin, end] into the specified encoding.
    template<typename Encoding,
             Details::ForwardIteratorToCodepoints<Encoding> InputIter,
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
