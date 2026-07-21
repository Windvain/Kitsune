#pragma once

#include "Foundation/Memory/AddressOf.h"
#include "Foundation/String/TextEncoding.h"

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
            auto [newBegin, _] = Encoding::Decode(begin, end, AddressOf(codepoint));
            if (newBegin == begin)
                return false;

            begin = newBegin;
        }

        return true;
    }
}
