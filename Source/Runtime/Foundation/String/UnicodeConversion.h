#pragma once

#include "Foundation/String/ToCodepoints.h"
#include "Foundation/String/FromCodepoints.h"

#include "Foundation/Containers/Array.h"
#include "Foundation/Iterators/Iterator.h"
#include "Foundation/Iterators/BackInsertIterator.h"

namespace Kitsune::Unicode
{
    template<ForwardIterator It, Iterator OutIt>
    OutIt Convert(It begin, It end, OutIt outBegin)
    {
        Array<Codepoint> buffer;
        ToCodepoints(begin, end, BackInsertIterator<decltype(buffer)>(buffer));

        return FromCodepoints(buffer.GetBegin(), buffer.GetEnd(), outBegin);
    }
}
