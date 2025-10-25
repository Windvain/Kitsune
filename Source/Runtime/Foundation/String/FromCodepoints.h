#pragma once

#include "Foundation/Common/Macros.h"
#include "Foundation/Concepts/Character.h"

#include "Foundation/Iterators/Iterator.h"
#include "Foundation/Iterators/IteratorTraits.h"

#include "Foundation/String/UnicodeTypes.h"
#include "Foundation/String/UnicodeValidity.h"

#include "Foundation/String/UnicodeException.h"

namespace Kitsune::Unicode
{
    namespace Details
    {
        [[noreturn]]
        KITSUNE_FORCEINLINE void ThrowInvalidCodepoint()
        {
            throw UnicodeException("One or more invalid codepoints were passed into a unicode function.");
        }
    }

    template<ForwardIterator It, Iterator OutIt>
        requires (Utf8Character<typename IteratorTraits<OutIt>::ValueType> &&
                  WritableIterator<OutIt, typename IteratorTraits<OutIt>::ValueType>)
    OutIt CodepointsToUtf8(It begin, It end, OutIt outBegin)
    {
        for (; begin != end; ++begin, ++outBegin)
        {
            if (!IsCodepointValid(*begin))
                Details::ThrowInvalidCodepoint();

            if (*begin <= 0x7F)
                *outBegin = *begin;
            else if (*begin <= 0x7FF)
            {
                *outBegin = (*begin >> 6) + 0xC0;
                *++outBegin = (*begin & 0x3F) + 0x80;
            }
            else if (*begin <= 0xFFFF)
            {
                *outBegin = (*begin >> 12) + 0xE0;
                *++outBegin = ((*begin & 0xFC0) >> 6) + 0x80;
                *++outBegin = (*begin & 0x3F) + 0x80;
            }
            else
            {
                *outBegin = (*begin >> 18) + 0xF0;
                *++outBegin = ((*begin & 0x3F000) >> 12) + 0x80;
                *++outBegin = ((*begin & 0xFC0) >> 6) + 0x80;
                *++outBegin = (*begin & 0x3F) + 0x80;
            }
        }

        return outBegin;
    }

    template<ForwardIterator It, Iterator OutIt>
        requires (Utf16Character<typename IteratorTraits<OutIt>::ValueType> &&
                  WritableIterator<OutIt, typename IteratorTraits<OutIt>::ValueType>)
    OutIt CodepointsToUtf16(It begin, It end, OutIt outBegin)
    {
        for (; begin != end; ++begin, ++outBegin)
        {
            if (!IsCodepointValid(*begin))
                Details::ThrowInvalidCodepoint();

            if (*begin < 0x10000)
                *outBegin = *begin;
            else
            {
                Codepoint sub = *begin - 0x10000;
                *outBegin = (sub >> 10) + 0xD800;
                *++outBegin = (sub & 0x3FF) + 0xDC00;
            }
        }

        return outBegin;
    }

    template<ForwardIterator It, Iterator OutIt>
        requires (Utf32Character<typename IteratorTraits<OutIt>::ValueType> &&
                  WritableIterator<OutIt, typename IteratorTraits<OutIt>::ValueType>)
    OutIt CodepointsToUtf32(It begin, It end, OutIt outBegin)
    {
        for (; begin != end; ++begin, ++outBegin)
        {
            if (!IsCodepointValid(*begin))
                Details::ThrowInvalidCodepoint();

            *outBegin = *begin;
        }

        return outBegin;
    }

    template<ForwardIterator It, Iterator OutIt>
    inline OutIt FromCodepoints(It begin, It end, OutIt outBegin)
    {
        using OutputChar = typename IteratorTraits<OutIt>::ValueType;
        static_assert(WritableIterator<OutIt, OutputChar>,
            "Output should satisfy the WritableIterator concept.");

        if constexpr (Utf8Character<OutputChar>)
            return CodepointsToUtf8(begin, end, outBegin);
        else if constexpr (Utf16Character<OutputChar>)
            return CodepointsToUtf16(begin, end, outBegin);
        else
            return CodepointsToUtf32(begin, end, outBegin);
    }
}
