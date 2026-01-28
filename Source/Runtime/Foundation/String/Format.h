#pragma once

#include "Foundation/String/FormatScanner.h"
#include "Foundation/String/FormatArguments.h"

#include "Foundation/Iterators/BackInsertIterator.h"

namespace Kitsune
{
    template<OutputIterator<const char&> OutputIter, typename... Args>
    inline OutputIter FormatTo(OutputIter outputIter, StringView formatString, Args&&... args)
    {
        FormatArgumentPack<sizeof...(Args), OutputIter> argumentPack = { Forward<Args>(args)... };
        while (!formatString.IsEmpty())
        {
            auto [newFormatString, newOutputIter] = DefaultFormatScanner::Format(argumentPack, formatString, outputIter);
            formatString = newFormatString;
            outputIter = newOutputIter;
        }

        return outputIter;
    }

    template<typename... Args>
    [[nodiscard]]
    inline String Format(const StringView formatString, Args&&... args)
    {
        using OutputIter = BackInsertIterator<String>;
        String result;

        FormatTo(OutputIter(result), formatString, Forward<Args>(args)...);
        return result;
    }
}
