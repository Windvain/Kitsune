#pragma once

#include "Foundation/Containers/Pair.h"

#include "Foundation/String/LineEnding.h"
#include "Foundation/String/StringView.h"

namespace Kitsune
{
    // An abstract representation of the UTF-32 encoding. it is a fixed-length
    // encoding, which means that it can be encoded into and decoded from relatively
    // quickly, at the price of a larger memory footprint.
    template<UTF32Character T>
    class UTF32Encoding
    {
    public:
        using CodepointType = Uint32;
        using CodeunitType = T;

    public:
        [[nodiscard]]
        static constexpr CodepointType MaxCodepointValue()
        {
            return 0x10FFFF;
        }

        [[nodiscard]]
        static constexpr Usize MaxCodeunits()
        {
            return 1;
        }

    public:
        [[nodiscard]]
        static constexpr BasicStringView<T> GetLineEnding(LineEndingOptions options)
        {
            // No constexpr ternary operator, no can do.
            BasicStringView<T> lineEnding;
            if constexpr (std::is_same_v<T, char32_t>)
                lineEnding = u"\r\n";
            else if constexpr (std::is_same_v<T, wchar_t>)
                lineEnding = L"\r\n";
            else
                KITSUNE_UNREACHABLE();

            if (options == LineEndingOptions::LF)
                lineEnding.RemovePrefix(1);

            return lineEnding;
        }

    public:
        [[nodiscard]]
        static constexpr BasicStringView<T> GetPreamble()
        {
            if constexpr (std::is_same_v<T, char32_t>)
                return U32StringView(U"\uFEFF");
            else if constexpr (std::is_same_v<T, wchar_t>)
                return WideStringView(L"\uFEFF");

            KITSUNE_UNREACHABLE();
        }

        [[nodiscard]]
        static constexpr BasicStringView<T> GetReplacement()
        {
            if constexpr (std::is_same_v<T, char32_t>)
                return U32StringView(U"\uFFFD");
            else if constexpr (std::is_same_v<T, wchar_t>)
                return WideStringView(L"\uFFFD");
        }

    public:
        template<ForwardIterator Iter, OutputIterator<CodepointType> OutIter>
        inline static Pair<Iter, OutIter> Decode(Iter begin, Iter end, OutIter outBegin)
        {
            if (begin == end)
                return { begin, outBegin };

            if ((*begin > MaxCodepointValue()) ||
                ((*begin >= 0xD800) && (*begin <= 0xDFFF)))
            {
                return { begin, outBegin };
            }

            *outBegin = static_cast<CodepointType>(*begin);
            return { ++begin, ++outBegin };
        }

        template<ForwardIterator Iter, OutputIterator<CodeunitType> OutIter>
        inline static Pair<Iter, OutIter> Encode(Iter begin, Iter end, OutIter outBegin)
        {
            if (begin == end)
                return { begin, outBegin };

            if ((*begin > 0x10FFFF) || ((*begin >= 0xD800) && (*begin <= 0xDFFF)))
                return { begin, outBegin };

            *outBegin = static_cast<CodeunitType>(*begin);
            return { ++begin, ++outBegin };
        }
    };
}
