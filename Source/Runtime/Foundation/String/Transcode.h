#pragma once

#include "Foundation/String/Encoding.h"
#include "Foundation/Memory/AddressOf.h"

namespace Kitsune
{
    template<Iterator InputIter, Iterator OutputIter>
    struct TranscodeResult
    {
        InputIter InputPosition;
        OutputIter OutputPosition;
    };

    // Transcodes the characters in the range [begin, end] from `InEncoding` to
    // `OutEncoding`.
    template<TextEncoding InEncoding, TextEncoding OutEncoding,
             ForwardIterator InputIter,
             OutputIterator<typename OutEncoding::CodeunitType> OutputIter>
    inline TranscodeResult<InputIter, OutputIter> Transcode(
        InputIter begin, InputIter end, OutputIter outBegin)
    {
        using Result = TranscodeResult<InputIter, OutputIter>;

        typename InEncoding::CodepointType codepoint;
        auto* codepointPtr = AddressOf(codepoint);

        while (begin != end)
        {
            auto [newBegin, newOutIter_] = InEncoding::DecodeSingle(
                begin, end, codepointPtr);

            if (newBegin == begin)
                return Result(newBegin, outBegin);

            begin = newBegin;

            auto [newCodepointPtr, newOutBegin] = OutEncoding::EncodeSingle(
                codepointPtr, codepointPtr + 1, outBegin);

            if (newCodepointPtr == codepointPtr)
                return Result(newBegin, outBegin);

            outBegin = newOutBegin;
        }

        return Result(begin, outBegin);
    }
}
