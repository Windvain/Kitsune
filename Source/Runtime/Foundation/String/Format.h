#pragma once

#include "Foundation/String/FormatScanner.h"
#include "Foundation/String/FormatArguments.h"

#include "Foundation/Iterators/BackInsertIterator.h"

namespace Kitsune
{
    // Formats the string `formatString` with the specified arguments and outputs
    // it into `outputIter`.
    template<OutputIterator<const char&> OutputIter, typename... Args>
    inline OutputIter FormatTo(OutputIter outputIter, StringView formatString,
                               Args&&... args)
    {
        FormatArgumentPack<sizeof...(Args), OutputIter> argumentPack = {
            Forward<Args>(args)...
        };

        while (!formatString.IsEmpty())
        {
            auto [newFormatString, newOutputIter] = DefaultFormatScanner::Format(
                argumentPack, formatString, outputIter);

            formatString = newFormatString;
            outputIter = newOutputIter;
        }

        return outputIter;
    }

    // Formats the string `formatString` with the specified arguments and returns
    // the output as a string.
    template<typename... Args>
    [[nodiscard]]
    inline String Format(StringView formatString, Args&&... args)
    {
        using OutputIter = BackInsertIterator<String>;
        String result;

        FormatTo(OutputIter(result), formatString, Forward<Args>(args)...);
        return result;
    }
}
