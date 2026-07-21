#pragma once

#include <concepts>

#include "Foundation/Containers/Pair.h"
#include "Foundation/String/StringView.h"

namespace Kitsune
{
    namespace Details
    {
        template<typename T>
        class TestForwardIter
        {
        public:
            using ValueType = T;
            using DifferenceType = Ptrdiff;

        public:
            ValueType& operator*() const { return std::declval<ValueType&>(); }
            TestForwardIter& operator++() { return *this; }
            TestForwardIter operator++(int) { return *this; }

        public:
            bool operator==(const TestForwardIter&) const { return true; }
        };

        template<typename T>
        class TestOutputIter
        {
        public:
            using ValueType = T;
            using DifferenceType = Ptrdiff;

        public:
            ValueType& operator*() const { return std::declval<ValueType&>(); }
            TestOutputIter& operator++() { return *this; }
            TestOutputIter operator++(int) { return *this; }
        };

        static_assert(
            ForwardIterator<TestForwardIter<Uint32>>,
            "TestForwardIter does not satisfy the requirements of ForwardIterator.");

        static_assert(
            OutputIterator<TestOutputIter<Uint32>, Uint32>,
            "TestOutputIter does not satisfy the requirements of OutputIterator.");

        template<typename T, typename Codeunit, typename Codepoint>
        concept EncodingCanDecode = requires (
            TestForwardIter<Codeunit> inputIter,
            TestOutputIter<Codepoint> outputBegin)
        {
            { T::Decode(inputIter, inputIter, outputBegin) } -> std::same_as<
                Pair<TestForwardIter<Codeunit>, TestOutputIter<Codepoint>>>;
        };

        template<typename T, typename Codeunit, typename Codepoint>
        concept EncodingCanEncode = requires (
            TestForwardIter<Codepoint> inputIter,
            TestOutputIter<Codeunit> outputBegin)
        {
            { T::Encode(inputIter, inputIter, outputBegin) } -> std::same_as<
                Pair<TestForwardIter<Codepoint>, TestOutputIter<Codeunit>>>;
        };
    }

    template<typename T>
    concept TextEncoding = requires
    {
        typename T::CodepointType;      // The type to which the codeunit maps to.
        typename T::CodeunitType;       // Commonly known as the "character type".

        { T::MaxCodepointValue() } -> std::same_as<typename T::CodepointType>;
        { T::MaxCodeunits() } -> std::same_as<Usize>;
        { T::GetPreamble() } -> std::same_as<BasicStringView<typename T::CodeunitType>>;

        { T::GetReplacement() }
            -> std::same_as<BasicStringView<typename T::CodeunitType>>;
    } &&
    Details::EncodingCanDecode<T, typename T::CodeunitType, typename T::CodepointType> &&
    Details::EncodingCanEncode<T, typename T::CodeunitType, typename T::CodepointType>;
}
