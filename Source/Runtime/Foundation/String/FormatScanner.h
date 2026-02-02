#pragma once

#include <cinttypes>

#include "Foundation/Common/Macros.h"
#include "Foundation/Algorithms/Copy.h"

#include "Foundation/String/StringView.h"

#include "Foundation/String/FormatArguments.h"
#include "Foundation/String/FormatException.h"

namespace Kitsune
{
    template<Character Char, OutputIterator<const Char&> OutputIter>
    struct BasicFormatResult
    {
        BasicStringView<Char> FormatString;
        OutputIter Output;
    };

    template<OutputIterator<const char&> OutputIter>
    using FormatResult = BasicFormatResult<char, OutputIter>;

    class DefaultFormatScanner
    {
    public:
        using CharType = char;

    public:
        template<OutputIterator<const CharType&> OutputIter, Usize ArgCount>
        inline static FormatResult<OutputIter> Format(
            const FormatArgumentPack<ArgCount, OutputIter>& arguments,
            StringView formatString,
            OutputIter outputIter)
        {
            if (formatString.IsEmpty())
                return { formatString, outputIter };

            auto leftBrace = formatString.Find('{');
            auto rightBrace = formatString.Find('}');

            // Handle regular text.
            if ((leftBrace != formatString.GetBegin()) &&
                (rightBrace != formatString.GetBegin()))
            {
                outputIter = Algorithms::Copy(
                    formatString.GetBegin(), KITSUNE_MIN(leftBrace, rightBrace),
                    outputIter);

                formatString.RemovePrefix(leftBrace - formatString.GetBegin());
                return { formatString, outputIter };
            }

            // Handle braces.
            const Usize DoubleBracesSize = 2;
            if (leftBrace < rightBrace)
            {
                bool isDoubleBrace = (leftBrace[1] == '{');
                if ((rightBrace == formatString.GetEnd()) && !isDoubleBrace)
                    throw FormatException("Format string contains a hanging left brace.");

                if (!isDoubleBrace)
                {
                    StringView formatArguments(leftBrace + 1, rightBrace);
                    outputIter = HandleFormatting(arguments, formatArguments, outputIter);

                    formatString.RemovePrefix(formatArguments.Size() + 2);
                }
                else
                {
                    formatString.RemovePrefix(DoubleBracesSize);
                    *outputIter++ = '{';
                }
            }
            else /* leftBrace > rightBrace */
            {
                if (rightBrace[1] != '}')
                    throw FormatException("Format string contains a hanging right brace.");

                formatString.RemovePrefix(DoubleBracesSize);
                *outputIter++ = '}';
            }

            return { formatString, outputIter };
        }

    private:
        template<OutputIterator<const CharType&> OutputIter, Usize ArgCount>
        inline static OutputIter HandleFormatting(
            const FormatArgumentPack<ArgCount, OutputIter>& arguments,
            StringView formatArguments,
            OutputIter outputIter)
        {
            char* indexEnd;
            auto index = static_cast<Index>(
                std::strtoumax(formatArguments.Data(), &indexEnd, 10));

            formatArguments.RemovePrefix(indexEnd - formatArguments.GetBegin());

            FormatContext<OutputIter> context(formatArguments, outputIter);
            arguments[index].Visit([&](const auto value) -> void
            {
                using ValueType = std::remove_cvref_t<decltype(value)>;
                constexpr bool isHandleType = std::is_same_v<
                    ValueType,
                    typename FormatArgument<OutputIter>::HandleType>;

                if constexpr (isHandleType)
                    outputIter = value.Format(context);
                else
                    outputIter = Formatter<ValueType, CharType>::Format(value, context);
            });

            return outputIter;
        }
    };
}
