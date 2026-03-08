#pragma once

#include <concepts>
#include "Foundation/Iterators/Iterator.h"

namespace Kitsune
{
    namespace Details
    {
        template<typename T>
        class EncodingForwardIter_
        {
        public:
            using ValueType = T;
            using DifferenceType = int;

            inline ValueType& operator*() const { return T(); }
            inline EncodingForwardIter_<T>& operator++()
            {
                return *this;
            }

            inline EncodingForwardIter_<T> operator++(int)
            {
                return *this;
            }

            inline bool operator==(const EncodingForwardIter_<T>&) const
            {
                return true;
            }
        };

        template<typename T>
        class EncodingOutputIter_
        {
        public:
            using ValueType = T;
            using DifferenceType = int;

            inline ValueType& operator*() const { return T(); }
            inline EncodingOutputIter_<T>& operator++()
            {
                return *this;
            }

            inline EncodingOutputIter_<T> operator++(int)
            {
                return *this;
            }
        };

        static_assert(ForwardIterator<EncodingForwardIter_<char>>,
                      "The test iterator used for the TextEncoding concept "
                      "does not satisfy the requirements of ForwardIterator.");

        static_assert(OutputIterator<EncodingOutputIter_<char>, char>,
                      "The test iterator used for the TextEncoding concept "
                      "does not satisfy the requirements of OutputIterator.");
    }

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
    concept TextEncoding =
        requires
        {
            typename Encoding::CodepointType;
            typename Encoding::CodeunitType;

            { Encoding::MaxCodepointValue() }
                -> std::same_as<typename Encoding::CodepointType>;
        } &&
        // DecodeSingle()
        requires (Details::EncodingForwardIter_<typename Encoding::CodeunitType> input,
                  Details::EncodingOutputIter_<typename Encoding::CodepointType> output)
        {
            { Encoding::DecodeSingle(input, input, output) }
                -> std::same_as<DecodeResult<decltype(input), decltype(output)>>;
        } &&
        // EncodeSingle()
        requires (Details::EncodingForwardIter_<typename Encoding::CodepointType> input,
                  Details::EncodingOutputIter_<typename Encoding::CodeunitType> output)
        {
            { Encoding::EncodeSingle(input, input, output) }
                -> std::same_as<EncodeResult<decltype(input), decltype(output)>>;
        };
}
