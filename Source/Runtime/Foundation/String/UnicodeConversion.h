#pragma once

#include "Foundation/String/String.h"
#include "Foundation/Containers/Array.h"

#include "Foundation/String/ToCodepoints.h"
#include "Foundation/String/FromCodepoints.h"

#include "Foundation/Iterators/Iterator.h"
#include "Foundation/Iterators/BackInsertIterator.h"

namespace Kitsune::Unicode
{
    template<Character T, ForwardIterator It, WritableIterator<T> OutIt>
    inline OutIt ConvertTo(It begin, It end, OutIt outBegin)
    {
        Array<Codepoint> buffer;
        ToCodepoints(begin, end, BackInsertIterator<decltype(buffer)>(buffer));

        return FromCodepoints<T>(buffer.GetBegin(), buffer.GetEnd(), outBegin);
    }

    template<Character From, Character To, typename Alloc = GlobalAllocator>
    inline BasicString<To, Alloc> ConvertString(const BasicStringView<From> str)
    {
        BasicString<To, Alloc> result;
        ConvertTo<To>(str.GetBegin(), str.GetEnd(), BackInsertIterator<decltype(result)>(result));

        return result;
    }
}
