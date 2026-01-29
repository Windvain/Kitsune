#pragma once

#include "Foundation/String/String.h"
#include "Foundation/String/Encoding.h"

#include "Foundation/Iterators/BackInsertIterator.h"

namespace Kitsune
{
    template<Iterator InputIter, Iterator OutputIter>
    struct TranscodeResult
    {
        InputIter InputPosition;
        OutputIter OutputPosition;
    };

    // Transcodes the characters in the range [begin, end] from `InEncoding` to `OutEncoding`.
    template<TextEncoding InEncoding, TextEncoding OutEncoding,
             ForwardIterator InputIter,
             OutputIterator<typename OutEncoding::CodeunitType> OutputIter>
    inline TranscodeResult<InputIter, OutputIter> Transcode(InputIter begin, InputIter end,
                                                            OutputIter outBegin)
    {
        using Result = TranscodeResult<InputIter, OutputIter>;

        typename InEncoding::CodepointType codepoint;
        auto* codepointPtr = AddressOf(codepoint);

        while (begin != end)
        {
            auto [newBegin, _] = InEncoding::DecodeSingle(begin, end, codepointPtr);
            if (newBegin == begin)
                return Result(newBegin, outBegin);

            begin = newBegin;

            auto [newCodepointPtr, newOutBegin] = OutEncoding::EncodeSingle(codepointPtr, codepointPtr + 1, outBegin);
            if (newCodepointPtr == codepointPtr)
                return Result(newBegin, outBegin);

            outBegin = newOutBegin;
        }

        return Result(begin, outBegin);
    }

    template<TextEncoding InEncoding, TextEncoding OutEncoding>
    inline auto Transcode(BasicStringView<typename InEncoding::CodeunitType> string)
    {
        BasicString<typename OutEncoding::CodeunitType> outputString;
        auto [newIterator, _] = Transcode<InEncoding, OutEncoding>(
            string.GetBegin(), string.GetEnd(), BackInsertIterator<decltype(outputString)>(outputString));

        // Append a unicode "replacement character" to the end of the string, no need for us
        // to continue.
        if (newIterator != string.GetEnd())
        {
            // The transcoding has already failed here, no need to check for the return value
            // of EncodeSingle().
            const char32_t InvalidCharacter = 0xFFFD;
            InEncoding::EncodeSingle(&InvalidCharacter, &InvalidCharacter + 1, BackInsertIterator<decltype(outputString)>(outputString));
        }

        return outputString;
    }
}
