#pragma once

#include "Foundation/Algorithms/Copy.h"
#include "Foundation/Memory/AddressOf.h"

#include "Foundation/String/TextEncoding.h"

namespace Kitsune
{
    // Transcodes the characters in the range [begin, end] from `InEncoding` to
    // `OutEncoding`.
    template<
        TextEncoding InEncoding,
        TextEncoding OutEncoding,
        ForwardIterator Iter,
        OutputIterator<typename OutEncoding::CodeunitType> OutIter>
    inline Pair<Iter, OutIter> Transcode(Iter begin, Iter end, OutIter outBegin)
        requires (
            !std::same_as<InEncoding, OutEncoding> &&
            std::same_as<
                typename InEncoding::CodepointType,
                typename OutEncoding::CodepointType>)
    {
        typename InEncoding::CodepointType codepoint;
        auto* pointer = AddressOf(codepoint);

        while (begin != end)
        {
            auto [newBegin, newOutIter] = InEncoding::Decode(begin, end, pointer);
            if (newBegin == begin)
                return { newBegin, outBegin };

            begin = newBegin;
            auto [newPointer, newOutBegin] = OutEncoding::Encode(
                pointer, pointer + 1, outBegin);

            if (newPointer == pointer)
                return { newBegin, outBegin };

            outBegin = newOutBegin;
        }

        return { begin, outBegin };
    }

    template<
        TextEncoding InEncoding,
        TextEncoding OutEncoding,
        ForwardIterator Iter,
        OutputIterator<typename OutEncoding::CodeunitType> OutIter>
    inline Pair<Iter, OutIter> Transcode(Iter begin, Iter end, OutIter outBegin)
        requires (std::same_as<InEncoding, OutEncoding>)
    {
        return { end, Algorithms::Copy(begin, end, outBegin) };
    }
}
