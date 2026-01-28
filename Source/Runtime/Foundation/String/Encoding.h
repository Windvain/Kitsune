#pragma once

#include "Foundation/Iterators/Iterator.h"

// TODO!!!!
namespace Kitsune
{
    template<Iterator InputIter, Iterator OutputIter>
    struct EncodeResult
    {
        InputIter InputPosition;
        OutputIter OutputPosition;
    };

    template<Iterator InputIter, Iterator OutputIter>
    struct DecodeResult
    {
        InputIter InputPosition;
        OutputIter OutputPosition;
    };

    template<Iterator InputIter, Iterator OutputIter>
    struct TranscodeResult
    {
        InputIter InputPosition;
        OutputIter OutputPosition;
    };
}
