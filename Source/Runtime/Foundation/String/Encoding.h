#pragma once

#include "Foundation/Iterators/Iterator.h"

namespace Kitsune
{
    // The result of a text encoding operation. Contains the values of the input
    // and output iterators when the encoding was stopped.
    template<Iterator InputIter, Iterator OutputIter>
    struct EncodeResult
    {
        InputIter InputPosition;
        OutputIter OutputPosition;
    };

    // The result of a text decoding operation. Contains the values of the input
    // and output iterators when the decoding operation was stopped.
    template<Iterator InputIter, Iterator OutputIter>
    struct DecodeResult
    {
        InputIter InputPosition;
        OutputIter OutputPosition;
    };

    // Describes an object which has a function to encode strings into the
    // text encoding, and a function to decode strings into codepoints.
    template<typename Encoding>
    concept TextEncoding = true;
}
