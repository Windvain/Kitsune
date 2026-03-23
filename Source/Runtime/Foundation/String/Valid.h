#pragma once

#include "Foundation/String/Encoding.h"
#include "Foundation/Memory/AddressOf.h"

namespace Kitsune
{
    // Returns an iterator to the first invalid sequence of code units in the
    // range `[begin, end]`, if none were found, returns `end`.
    template<TextEncoding Encoding, ForwardIterator Iter>
    inline Iter FindInvalidEncoding(Iter begin, Iter end)
    {
        typename Encoding::CodepointType codepoint;
        while (begin != end)
        {
            auto [newBegin, outIter_] = Encoding::DecodeSingle(
                begin, end, AddressOf(codepoint));

            if (newBegin == begin)
                return newBegin;

            begin = newBegin;
        }

        return end;
    }

    // Returns true if the range `[begin, end]` is valid in the specified text
    // encoding `Encoding`.
    template<TextEncoding Encoding, ForwardIterator Iter>
    inline bool IsValidEncoding(Iter begin, Iter end)
    {
        return (FindInvalidEncoding<Encoding>(begin, end) == end);
    }
}
