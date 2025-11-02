#pragma once

#include "Foundation/Common/Macros.h"
#include "Foundation/Concepts/Character.h"

#include "Foundation/Iterators/Iterator.h"

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

    template<Utf8Character T, ForwardIterator It, WritableIterator<T> OutIt>
    OutIt FromCodepoints(It begin, It end, OutIt outBegin)
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

    template<Utf16Character T, ForwardIterator It, WritableIterator<T> OutIt>
    OutIt FromCodepoints(It begin, It end, OutIt outBegin)
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

    template<Utf32Character T, ForwardIterator It, WritableIterator<T> OutIt>
    OutIt FromCodepoints(It begin, It end, OutIt outBegin)
    {
        for (; begin != end; ++begin, ++outBegin)
        {
            if (!IsCodepointValid(*begin))
                Details::ThrowInvalidCodepoint();

            *outBegin = *begin;
        }

        return outBegin;
    }
}
