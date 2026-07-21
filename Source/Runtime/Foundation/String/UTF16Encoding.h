#pragma once

#include "Foundation/Containers/Pair.h"
#include "Foundation/String/StringView.h"

namespace Kitsune
{
    // An abstract representation of the UTF-16 encoding. Fairly rare to see this
    // encoding out in the wild except for when dealing with the Win32 API or the
    // Qt libraries.
    template<UTF16Character T>
    class UTF16Encoding
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
            return 2;
        }

        [[nodiscard]]
        static constexpr BasicStringView<T> GetPreamble()
        {
            if constexpr (std::is_same_v<T, char16_t>)
                return U16StringView(u"\uFEFF");
            else if constexpr (std::is_same_v<T, wchar_t>)
                return WideStringView(L"\uFEFF");

            KITSUNE_UNREACHABLE();
        }

        [[nodiscard]]
        static constexpr BasicStringView<T> GetReplacement()
        {
            if constexpr (std::is_same_v<T, char16_t>)
                return U16StringView(u"\uFFFD");
            else if constexpr (std::is_same_v<T, wchar_t>)
                return WideStringView(L"\uFFFD");
        }

    public:
        template<ForwardIterator Iter, OutputIterator<CodepointType> OutIter>
        inline static Pair<Iter, OutIter> Decode(Iter begin, Iter end, OutIter outBegin)
        {
            if (begin == end)
                return { begin, outBegin };

            // Low surrogates cannot come before a high surrogate.
            if ((*begin & 0xFC00) == 0xDC00)
                return { begin, outBegin };

            if ((*begin & 0xFC00) != 0xD800)
                *outBegin = static_cast<CodepointType>(*begin);
            else
            {
                Iter high = begin++;        // high == high surr., begin == low surr.

                // The high surrogate is not preceeded by a low surrogate.
                if ((begin == end) || ((*begin & 0xFC00) != 0xDC00))
                    return { high, outBegin };

                CodepointType codepoint = ((*high - 0xD800) << 10) | (*begin - 0xDC00);
                codepoint += 0x10000;

                *outBegin = codepoint;
            }

            return { ++begin, ++outBegin };
        }

        template<ForwardIterator Iter, OutputIterator<CodeunitType> OutIter>
        inline static Pair<Iter, OutIter> Encode(Iter begin, Iter end, OutIter outBegin)
        {
            if (begin == end)
                return { begin, outBegin };

            if ((*begin > MaxCodepointValue()) ||
                ((*begin >= 0xD800) && (*begin <= 0xDFFF)))
            {
                return { begin, outBegin };
            }

            if (*begin <= 0xFFFF)
                *outBegin = static_cast<T>(*begin);
            else
            {
                CodepointType codepoint = *begin;
                codepoint -= 0x10000;

                *outBegin = 0xD800 + (codepoint >> 10);
                *++outBegin = 0xDC00 + (codepoint & 0x3FF);
            }

            return { ++begin, ++outBegin };
        }
    };
}
