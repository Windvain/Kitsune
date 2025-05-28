#pragma once

#include "Foundation/Diagnostics/Assert.h"
#include "Foundation/Concepts/Character.h"

#include "Foundation/Iterators/Iterator.h"
#include "Foundation/Iterators/IteratorTraits.h"

#include "Foundation/String/UnicodeTypes.h"
#include "Foundation/String/UnicodeException.h"

namespace Kitsune::Unicode
{
    namespace Details
    {
        template<ForwardIterator It>
        inline bool CheckUtf8HeaderLength(It curr, It end, Uint32& trailingBytes)
        {
            trailingBytes = ((*curr & 0b1111'1000) == 0b1111'0000) ? 3 :
                            ((*curr & 0b1111'0000) == 0b1110'0000) ? 2 :
                            ((*curr & 0b1110'0000) == 0b1100'0000) ? 1 :
                                                                     0;

            if (trailingBytes == 0)
                return false;

            if constexpr (RandomAccessIterator<It>)
                return ((curr + trailingBytes) < end);
            else
            {
                for (Uint32 copy = trailingBytes; copy > 0; --copy, ++curr)
                {
                    if (curr == end)
                        return false;
                }

                return true;
            }
        }

        template<ForwardIterator It>
        inline bool CheckUtf16HeaderLength(It curr, It end)
        {
            KITSUNE_ASSERT((*curr & 0xFC00) == 0xD800, "The current iterator does not contain a high surrogate. This is an implementation bug.");
            return (++curr != end);
        }

        [[noreturn]]
        KITSUNE_FORCEINLINE void ThrowInvalidUnicodeString()
        {
            throw UnicodeException("An invalid unicode string was passed in.");
        }
    }

    template<ForwardIterator It, WritableIterator<Codepoint> OutIt>
        requires Utf8Character<typename IteratorTraits<It>::ValueType>
    OutIt ToCodepoints(It begin, It end, OutIt outBegin)
    {
        for (; begin != end; ++begin, ++outBegin)
        {
            // Fast path for ASCII characters.
            if ((*begin & 0x80) == 0)
            {
                *outBegin = *begin;
                continue;
            }

            Uint32 trailingBytes;
            if (!Details::CheckUtf8HeaderLength(begin, end, trailingBytes))
                Details::ThrowInvalidUnicodeString();

            Codepoint codepoint = 0;
            Uint8 mask = ~(0xFF << (6 - trailingBytes));

            codepoint |= *begin & mask;
            codepoint <<= 6;

            for (++begin; /* ... */ ; --trailingBytes, ++begin)
            {
                if ((*begin & 0b1100'0000) == 0b1100'0000)
                    Details::ThrowInvalidUnicodeString();

                codepoint |= (*begin & 0b0011'1111);
                if (trailingBytes == 1)
                    break;

                codepoint <<= 6;
            }

            *outBegin = codepoint;
        }

        return outBegin;
    }

    template<ForwardIterator It, WritableIterator<Codepoint> OutIt>
        requires Utf16Character<typename IteratorTraits<It>::ValueType>
    OutIt ToCodepoints(It begin, It end, OutIt outBegin)
    {
        for (; begin != end; ++begin, ++outBegin)
        {
            if ((*begin & 0xFC00) != 0xD800)
                *outBegin = *begin;
            else
            {
                Codepoint codepoint = *begin - 0xD800;
                codepoint <<= 10;

                if (!Details::CheckUtf16HeaderLength(begin, end) || ((*++begin & 0xFC00) != 0xDC00))
                    Details::ThrowInvalidUnicodeString();

                codepoint += (*begin - 0xDC00);
                codepoint += 0x10000;

                *outBegin = codepoint;
            }
        }

        return outBegin;
    }

    template<ForwardIterator It, WritableIterator<Codepoint> OutIt>
        requires Utf32Character<typename IteratorTraits<It>::ValueType>
    OutIt ToCodepoints(It begin, It end, OutIt outBegin)
    {
        for (; begin != end; ++begin, ++outBegin)
            *outBegin = *begin;

        return outBegin;
    }
}
