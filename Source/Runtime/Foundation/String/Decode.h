#pragma once

#include "Foundation/String/TextEncoding.h"

namespace Kitsune
{
    // Decodes a range of characters into codepoints with the specified encoding.
    template<TextEncoding Encoding,
             ForwardIterator Iter,
             OutputIterator<typename Encoding::CodeunitType> OutIter>
    inline Pair<Iter, OutIter> Decode(Iter begin, Iter end, OutIter outBegin)
    {
        while (begin != end)
        {
            auto [newBegin, newOutBegin] = Encoding::Decode(begin, end, outBegin);
            if (newBegin == begin)
                return { newBegin, newOutBegin };

            begin = newBegin;
            outBegin = newOutBegin;
        }

        return { begin, outBegin };
    }
}
