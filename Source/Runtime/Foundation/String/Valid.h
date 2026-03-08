#pragma once

#include "Foundation/String/Encoding.h"
#include "Foundation/Memory/AddressOf.h"

namespace Kitsune
{
    // Returns true if the range `[begin, end]` is valid in the specified text
    // encoding `Encoding`.
    template<TextEncoding Encoding, ForwardIterator Iter>
    inline bool IsValidEncoding(Iter begin, Iter end)
    {
        typename Encoding::CodepointType codepoint;
        while (begin != end)
        {
            auto [newBegin, _] = Encoding::DecodeSingle(
                begin, end, AddressOf(codepoint));

            if (newBegin == begin)
                return false;

            begin = newBegin;
        }

        return true;
    }
}
