#pragma once

#include "Foundation/String/Encoding.h"
#include "Foundation/Memory/AddressOf.h"

namespace Kitsune
{
    namespace Details
    {
        template<typename Iter, typename Encoding>
        concept ForwardIteratorToChars =
            ForwardIterator<Iter> &&
            std::same_as<typename Encoding::ValueType,
                         typename IteratorTraits<Iter>::ValueType>;
    }

    template<typename Encoding,
             Details::ForwardIteratorToChars<Encoding> Iter>
    inline bool IsValidEncoding(Iter begin, Iter end)
    {
        typename Encoding::CodepointType codepoint;
        while (begin != end)
        {
            auto [newBegin, _] = Encoding::DecodeSingle(begin, end, AddressOf(codepoint));
            if (newBegin == begin)
                return false;
        }

        return true;
    }
}
