#pragma once

#include "Foundation/String/TextEncoding.h"

namespace Kitsune
{
    // Encodes codepoints in the range [begin, end] into the specified encoding.
    template<TextEncoding Encoding,
             ForwardIterator Iter,
             OutputIterator<typename Encoding::CodeunitType> OutIter>
    inline Pair<Iter, OutIter> Encode(Iter begin, Iter end, OutIter outBegin)
    {
        while (begin != end)
        {
            auto [newBegin, newOutBegin] = Encoding::Encode(begin, end, outBegin);
            if (newBegin == begin)
                return { newBegin, newOutBegin };

            begin = newBegin;
            outBegin = newOutBegin;
        }

        return { begin, outBegin };
    }
}
