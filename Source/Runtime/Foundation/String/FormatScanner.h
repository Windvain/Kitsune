#pragma once

#include "Foundation/Common/Macros.h"

#include "Foundation/Maths/Minimum.h"
#include "Foundation/Algorithms/Copy.h"

#include "Foundation/String/StringView.h"
#include "Foundation/String/FormatArguments.h"
#include "Foundation/String/FormatException.h"

KITSUNE_PUSH_COMPILER_WARNINGS()
KITSUNE_IGNORE_MSVC_WARNING(4702)   // Unreachable code. No idea why this happens, just
                                    // going to ignore this. Seems to be a false alarm.

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
        inline static FormatResult<OutputIter> FormatSingle(
            const FormatArgumentPack<ArgCount, OutputIter>& arguments,
            StringView substring,
            OutputIter outputIter)
        {
            auto leftBrace = substring.Find('{');
            auto rightBrace = substring.Find('}');

            // Handle regular text.
            if ((leftBrace != substring.GetBegin()) &&
                (rightBrace != substring.GetBegin()))
            {
                return {
                    StringView(leftBrace, substring.GetEnd()),
                    Algorithms::Copy(
                        substring.GetBegin(),
                        Maths::Minimum(leftBrace, rightBrace),
                        outputIter)
                };
            }

            if (leftBrace < rightBrace)
            {
                bool isDoubleBrace = ((leftBrace + 1) != substring.GetEnd()) &&
                                     (leftBrace[1] == '{');

                if (!isDoubleBrace)
                {
                    if (rightBrace == substring.GetEnd())
                    {
                        throw FormatException(
                            "Failed to format the specified format string, because the "
                            "format string contains a hanging brace.");
                    }

                    StringView argsView(leftBrace + 1, rightBrace);
                    return {
                        StringView(rightBrace + 1, substring.GetEnd()),
                        HandleFormatting_(arguments, argsView, outputIter)
                    };
                }
                else
                {
                    return {
                        StringView(leftBrace + 2, substring.GetEnd()),
                        *outputIter++ = '{'
                    };
                }
            }
            else /* leftBrace > rightBrace */
            {
                if (((rightBrace + 1) != substring.GetEnd()) && (rightBrace[1] == '{'))
                {
                    throw FormatException(
                        "Failed to format the specified format string, because the "
                        "format string contains a hanging brace.");
                }

                return {
                    StringView(rightBrace + 2, substring.GetEnd()),
                    *outputIter++ = '}'
                };
            }

            return { substring, outputIter };
        }

    private:
        template<OutputIterator<const char&> OutputIter, Usize ArgCount>
        inline static OutputIter HandleFormatting_(
            const FormatArgumentPack<ArgCount, OutputIter>& arguments,
            StringView argsView,
            OutputIter outputIter)
        {
            StringView indexSubstring(argsView.GetBegin(), argsView.Find(':'));
            Index index = GetIndex_(indexSubstring);

            argsView.RemovePrefix(indexSubstring.Size());
            if (!argsView.IsEmpty())
                argsView.RemovePrefix(1);

            FormatContext<OutputIter> context(argsView, outputIter);
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

        inline static Index GetIndex_(StringView indexView)
        {
            Index index = 0;
            for (char digit : indexView)
            {
                if ((digit < '0') || (digit > '9'))
                    throw FormatException("The format string contains an invalid index.");

                index *= 10;
                index += (digit - '0');
            }

            return index;
        }
    };
}

KITSUNE_POP_COMPILER_WARNINGS()
