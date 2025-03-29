#pragma once

#include <cstdlib>
#include <concepts>

#include "Foundation/String/String.h"
#include "Foundation/String/FormatArguments.h"

#include "Foundation/Algorithms/Find.h"
#include "Foundation/String/FormatException.h"

#include "Foundation/String/Formatter.h"

namespace Kitsune
{
    namespace Internal
    {
        [[noreturn]]
        KITSUNE_FORCEINLINE void ThrowInvalidFormatSpecs()
        {
            throw FormatException("Tried to parse an invalid format specification.");
        }

        [[noreturn]]
        KITSUNE_FORCEINLINE void ThrowUnmatchedBraces()
        {
            throw FormatException("Format string contains an unmatched brace.");
        }
    }

    template<typename T, typename OutIt>
    concept FormatScanner = requires (OutIt out, T scanner, const StringView specs,
                                      const FormatArgumentPack<OutIt> pack)
    {
        scanner.Step();

        { scanner.GetFormatSpecs() } -> std::convertible_to<StringView>;
        { T::ParseFormatSpecs(out, specs, pack)  } -> std::same_as<OutIt>;
    };

    class DefaultFormatScanner
    {
    public:
        DefaultFormatScanner() = default;
        DefaultFormatScanner(const StringView fmt)
            : m_BeginSpecs(fmt.GetBegin()), m_EndSpecs(fmt.GetBegin()),
              m_EndFormat(fmt.GetEnd())
        {
        }

    public:
        KITSUNE_API_ void Step();

    public:
        [[nodiscard]]
        inline StringView GetFormatSpecs() const
        {
            return StringView(m_BeginSpecs, m_EndSpecs);
        }

    public:
        template<WritableIterator<char> OutIt>
        inline static OutIt ParseFormatSpecs(OutIt out, const StringView formatSpecs,
                                             const FormatArgumentPack<OutIt>& argumentPack)
        {
            using namespace Internal;

            if (formatSpecs.Size() == 2)
            {
                // Can either be a double brace or an invalid format spec.
                if ((formatSpecs[0] == '{') && (formatSpecs[1] == '{'))
                    *out++ = '{';
                else if ((formatSpecs[0] == '}') && (formatSpecs[1] == '}'))
                    *out++ = '}';
                else
                    ThrowInvalidFormatSpecs();
            }
            else if (formatSpecs.Size() > 2)
            {
                Index index;
                StringView contents(formatSpecs.GetBegin() + 1, formatSpecs.GetEnd() - 1);

                auto colon = Algorithms::FindIf(contents.GetBegin(), contents.GetEnd(), IsNotDigit);
                bool isColon = (*colon == ':');

                if ((colon != contents.GetEnd()) && !isColon)
                    ThrowInvalidFormatSpecs();

                // Get index from contents.
                String indexStr = { contents.GetBegin(), colon };
                index = static_cast<Index>(std::atoll(indexStr.Raw()));

                // Pass the rest of the arguments over to the formatter.
                argumentPack[index].Visit([&](const auto& value)
                {
                    using Type = std::remove_cvref_t<decltype(value)>;

                    StringView formatArguments(colon + isColon, contents.GetEnd());

                    ParseContext parseContext(formatArguments);
                    FormatContext<Type, OutIt> formatContext(out, value);

                    if constexpr (std::is_same_v<Type, CustomTypeHandle<OutIt>>)
                        out = value.ParseAndFormat(parseContext, formatContext);
                    else
                    {
                        Formatter<Type> formatter;
                        formatter.Parse(parseContext);

                        out = formatter.Format(formatContext);
                    }
                });
            }
            else
            {
                ThrowInvalidFormatSpecs();
            }

            return out;
        }

    private:
        static bool IsNotDigit(char ch)
        {
            return ((ch < '0') || (ch > '9'));
        }

    public:
        StringView::Iterator m_BeginSpecs;
        StringView::Iterator m_EndSpecs;
        StringView::Iterator m_EndFormat;
    };
}
