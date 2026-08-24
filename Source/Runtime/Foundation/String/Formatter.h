#pragma once

#include <cmath>
#include <cstdio>

#include "Foundation/String/String.h"

#include "Foundation/Algorithms/Copy.h"
#include "Foundation/Algorithms/Reverse.h"

namespace Kitsune
{
    template<Character Char, OutputIterator<const Char&> Iter>
    class BasicFormatContext
    {
    public:
        using Iterator = Iter;

    public:
        inline explicit BasicFormatContext(
            BasicStringView<Char> formatSpecs,
            Iter outputIter)
            : m_FormatSpecs(formatSpecs), m_OutputIter(outputIter)
        {
        }

    public:
        [[nodiscard]]
        inline BasicStringView<Char> GetFormatSpecifications() const
        {
            return m_FormatSpecs;
        }

        [[nodiscard]]
        inline Iterator GetOutput() const
        {
            return m_OutputIter;
        }

    private:
        BasicStringView<Char> m_FormatSpecs;
        Iter m_OutputIter;
    };

    template<OutputIterator<const char&> Iter>
    using FormatContext = BasicFormatContext<char, Iter>;

    template<OutputIterator<const wchar_t&> Iter>
    using WideFormatContext = BasicFormatContext<wchar_t, Iter>;

    template<typename T, Character Char>
    class Formatter { /* ... */ };

    template<>
    class Formatter<bool, char>
    {
    public:
        template<OutputIterator<const char&> Iter>
        inline static Iter Format(
            bool boolean,
            const FormatContext<Iter>& context)
        {
            StringView specs = context.GetFormatSpecifications();
            bool formatAsInteger = false;

            if (specs.Contains('i') || specs.Contains('I'))
                formatAsInteger = true;

            StringView result = formatAsInteger ? FormatAsInteger(boolean) :
                                                  FormatAsBoolean(boolean);

            return Algorithms::Copy(result.GetBegin(), result.GetEnd(),
                                    context.GetOutput());
        }

    private:
        inline static StringView FormatAsInteger(bool boolean)
        {
            // A false value will be converted to zero, while a true value
            // to one, so there is no need to use the integer formatter.
            return boolean ? "1" : "0";
        }

        inline static StringView FormatAsBoolean(bool boolean)
        {
            return boolean ? "true" : "false";
        }
    };

    template<std::integral Integer>
    class Formatter<Integer, char>
    {
    public:
        template<OutputIterator<const char&> Iter>
        inline static Iter Format(
            Integer integer,
            const FormatContext<Iter>& context)
        {
            using UnsignedType = std::make_unsigned_t<Integer>;
            StringView specs = context.GetFormatSpecifications();

            UnsignedType unsignedValue;
            UnsignedType base = 10;

            if (specs.Contains('b') || specs.Contains('B'))
                base = 2;
            else if (specs.Contains('o') || specs.Contains('O'))
                base = 8;
            else if (specs.Contains('d') || specs.Contains('D'))
                base = 10;
            else if (specs.Contains('x') || specs.Contains('X'))
                base = 16;

            if ((base != 10) || (integer >= 0))
                std::memcpy(&unsignedValue, &integer, sizeof(Integer));
            else
                unsignedValue = static_cast<UnsignedType>(~integer) + 1;

            UnsignedType remainder;
            String result;

            const char DigitsArray[] = "0123456789ABCDEF";
            do
            {
                remainder = unsignedValue % base;
                result += DigitsArray[remainder];

                unsignedValue /= base;
            } while (unsignedValue != 0);

            if (specs.Contains('0'))
            {
                // This can be derived from the equation:
                // <base>^<width in base> >= 2^<width in base 2> - 1.
                bool adjustPadding = std::is_signed_v<Integer> && (base == 10);
                float pow = std::pow(
                    2,
                    sizeof(Integer) * (8 - Usize(adjustPadding))) - 1;

                float numerator = std::logf(pow);
                float denominator = std::logf(base);

                Usize count = Usize(std::ceil(numerator / denominator)) - result.Size();
                result.Append(count, '0');
            }

            if (specs.Contains('#') && (base != 10))
            {
                // b0, 0, x0 gets flipped at the end and turns into 0b, 0, and 0x.
                result += (base == 2)  ? "b0" :
                          (base == 8)  ? "0"  :
                          (base == 16) ? "x0" :
                                         "";
            }

            if (base == 10)
            {
                if (integer < 0)
                    result += '-';
                else if (specs.Contains('+'))
                    result += '+';
            }

            Algorithms::Reverse(result.GetBegin(), result.GetEnd());

            return Algorithms::Copy(
                result.GetBegin(), result.GetEnd(),
                context.GetOutput());
        }
    };

    template<>
    class Formatter<char, char>
    {
    public:
        template<OutputIterator<const char&> Iter>
        inline static Iter Format(
            char character,
            const FormatContext<Iter>& context)
        {
            StringView specs = context.GetFormatSpecifications();
            if (specs.Contains('I') || specs.Contains('i'))
            {
                using IntType = std::conditional_t<
                    std::is_signed_v<char>,
                    signed char,
                    unsigned char>;

                return Formatter<IntType, char>::Format(
                    static_cast<IntType>(character),
                    context);
            }
            else
            {
                auto iter = context.GetOutput();

                *iter++ = character;
                return iter;
            }
        }
    };

    template<std::floating_point Float>
    class Formatter<Float, char>
    {
    public:
        template<OutputIterator<const char&> Iter>
        inline static Iter Format(
            Float floatNum,
            const FormatContext<Iter>& context)
        {
            auto value = static_cast<long double>(floatNum);
            int count = std::snprintf(nullptr, 0, "%Lf", value);

            String str(static_cast<Usize>(count), '\0');
            std::snprintf(str.Data(), count, "%Lf", value);

            return Algorithms::Copy(
                str.GetBegin(), str.GetEnd(),
                context.GetOutput());
        }
    };

    template<typename T>
    class Formatter<T*, char>
    {
    public:
        template<OutputIterator<const char&> Iter>
        inline static Iter Format(
            T* pointer,
            const FormatContext<Iter>& context)
        {
            auto pointerInt = reinterpret_cast<Uintptr>(pointer);
            FormatContext<Iter> modifiedContext("0#x", context.GetOutput());

            return Formatter<Uintptr, char>::Format(pointerInt, modifiedContext);
        }

    private:
        Formatter<Uintptr, char> m_Formatter;
    };

    template<>
    class Formatter<StringView, char>
    {
    public:
        template<OutputIterator<const char&> Iter>
        inline static Iter Format(
            StringView string,
            const FormatContext<Iter>& context)
        {
            return Algorithms::Copy(string.GetBegin(), string.GetEnd(),
                                    context.GetOutput());
        }
    };
}
