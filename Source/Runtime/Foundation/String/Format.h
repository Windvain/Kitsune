#pragma once

#include "Foundation/String/FormatScanner.h"
#include "Foundation/String/FormatArguments.h"

#include "Foundation/Iterators/BackInsertIterator.h"

namespace Kitsune
{
    // Formats the string `formatString` with the specified argument pack `arguments`,
    // and outputs it into `outputIter`.
    template<OutputIterator<const char&> OutputIter, Usize ArgsCount>
    inline OutputIter VariadicFormatTo(
        OutputIter outputIter, StringView formatString,
        const FormatArgumentPack<ArgsCount, OutputIter>& arguments)
    {
        while (!formatString.IsEmpty())
        {
            auto [substring, newOutput] = DefaultFormatScanner::FormatSingle(
                arguments, formatString, outputIter);

            formatString = substring;
            outputIter = newOutput;
        }

        return outputIter;
    }

    // Formats the string `formatString` with the specified arguments and outputs
    // it into `outputIter`.
    template<OutputIterator<const char&> OutputIter, typename... Args>
    inline OutputIter FormatTo(OutputIter outputIter, StringView formatString,
                               Args&&... arguments)
    {
        return VariadicFormatTo(
            outputIter,
            formatString,
            MakeFormatArgumentPack<OutputIter>(Forward<Args>(arguments)...));
    }

    // Formats the string `formatString` with the specified argument pack `arguments`,
    // and returns the output as a string.
    template<Usize ArgsCount>
    [[nodiscard]]
    inline String VariadicFormat(
        StringView formatString,
        const FormatArgumentPack<ArgsCount, BackInsertIterator<String>>& arguments)
    {
        String result;
        VariadicFormatTo(BackInsertIterator<String>(result), formatString, arguments);

        return result;
    }

    // Formats the string `formatString` with the specified arguments and returns
    // the output as a string.
    template<typename... Args>
    [[nodiscard]]
    inline String Format(StringView formatString, Args&&... args)
    {
        using OutputIter = BackInsertIterator<String>;
        String result;

        VariadicFormatTo(
            OutputIter(result),
            formatString,
            MakeFormatArgumentPack<OutputIter>(Forward<Args>(args)...));

        return result;
    }
}
