#pragma once

#include <cmath>
#include <cstdio>

#include "Foundation/String/String.h"

#include "Foundation/Algorithms/Copy.h"
#include "Foundation/Algorithms/Reverse.h"

namespace Kitsune
{
    template<Character Char>
    class BasicParseContext
    {
    public:
        BasicParseContext() = default;
        inline explicit BasicParseContext(const BasicStringView<Char> specs)
            : m_FormatSpecs(specs)
        {
        }

    public:
        [[nodiscard]]
        inline BasicStringView<Char> GetFormatSpecs() const { return m_FormatSpecs; }

    private:
        BasicStringView<Char> m_FormatSpecs;
    };

    template<Character Char, WritableIterator<Char> Iter>
    class BasicFormatContext
    {
    public:
        using Iterator = Iter;

    public:
        inline explicit BasicFormatContext(Iter outputIter)
            : m_OutputIter(outputIter)
        {
        }

    public:
        [[nodiscard]] inline Iterator GetOutput() const { return m_OutputIter; }

    private:
        Iter m_OutputIter;
    };

    using ParseContext = BasicParseContext<char>;
    using WideParseContext = BasicParseContext<wchar_t>;

    template<WritableIterator<char> Iter>
    using FormatContext = BasicFormatContext<char, Iter>;

    template<WritableIterator<wchar_t> Iter>
    using WideFormatContext = BasicFormatContext<wchar_t, Iter>;

    template<typename T, Character Char>
    class Formatter { /* ... */ };

    template<>
    class Formatter<bool, char>
    {
    public:
        inline void Parse(const ParseContext& context)
        {
            StringView specs = context.GetFormatSpecs();
            for (char ch : specs)
            {
                if ((ch == 'i') || (ch == 'I'))
                    m_AsInteger = true;
            }
        }

        template<WritableIterator<char> Iter>
        [[nodiscard]]
        inline Iter Format(bool boolean, const FormatContext<Iter>& context)
        {
            StringView str = (m_AsInteger) ? (boolean ? "1"    : "0") :
                                             (boolean ? "true" : "false");

            return Algorithms::Copy(str.GetBegin(), str.GetEnd(),
                                    context.GetOutput());
        }

    private:
        bool m_AsInteger = false;
    };

    template<std::integral T>
    class Formatter<T, char>
    {
    public:
        inline void Parse(const ParseContext& context)
        {
            StringView specs = context.GetFormatSpecs();
            for (char spec : specs)
            {
                if (spec == '+')      m_AlwaysAddSign = true;
                else if (spec == '-') m_AlwaysAddSign = false;
                else if (spec == '#') m_AddPrefix = true;
                else if (spec == '0') m_AddLeadingZeroes = true;

                m_Base = ((spec == 'b') || (spec == 'B')) ? 2 :
                         ((spec == 'o') || (spec == 'O')) ? 8 :
                         ((spec == 'd') || (spec == 'D')) ? 10 :
                         ((spec == 'x') || (spec == 'X')  ? 16 :
                                                            m_Base);
            }
        }

        template<WritableIterator<char> Iter>
        [[nodiscard]]
        inline Iter Format(T integer, const FormatContext<Iter>& context)
        {
            using UnsignedType = std::make_unsigned_t<T>;
            UnsignedType unsignedValue;

            if ((m_Base != 10) || (integer >= 0))
                std::memcpy(&unsignedValue, &integer, sizeof(T));
            else
                unsignedValue = static_cast<UnsignedType>(~integer) + 1;

            UnsignedType remainder;
            String str;

            constexpr const char* DigitsArray = "0123456789ABCDEF";

            do
            {
                remainder = unsignedValue % m_Base;
                str += DigitsArray[remainder];

                unsignedValue /= m_Base;
            } while (unsignedValue != 0);

            if (m_AddLeadingZeroes)
            {
                // This can be derived from the equation: (base)^(width in base) >= 2^(width in base 2) - 1.
                // With a little bit of algebra: width >= log_<base>(2^<width in base 2> - 1)
                bool adjustPadding = std::is_signed_v<T> && (m_Base == 10);
                float pow = std::pow(2, sizeof(T) * (8 - Usize(adjustPadding))) - 1;

                float numerator = std::logf(pow);
                float denominator = std::logf(m_Base);

                Usize count = Usize(std::ceil(numerator / denominator)) - str.Size();
                str.Append(count, '0');
            }

            if (m_AddPrefix && (m_Base != 10))
            {
                // b0, 0, x0 gets flipped at the end and turns into 0b, 0, and 0x.
                str += (m_Base == 2)  ? "b0" :
                       (m_Base == 8)  ? "0"  :
                       (m_Base == 16) ? "x0" :
                                        "";
            }

            if (m_Base == 10)
            {
                if (integer < 0)
                    str += '-';
                else if (m_AlwaysAddSign)
                    str += '+';
            }

            Algorithms::Reverse(str.GetBegin(), str.GetEnd());
            return Algorithms::Copy(str.GetBegin(), str.GetEnd(), context.GetOutput());
        }

    private:
        T m_Base = 10;

        bool m_AlwaysAddSign = false;
        bool m_AddPrefix = false;
        bool m_AddLeadingZeroes = false;
    };

    template<>
    class Formatter<char, char>
    {
    public:
        inline void Parse(const ParseContext& context)
        {
            StringView specs = context.GetFormatSpecs();
            for (char spec : specs)
            {
                m_AsInteger = ((spec == 'i') || (spec == 'I')) ? true :
                              ((spec == 'c') || (spec == 'C')) ? false :
                                                                 m_AsInteger;
            }
        }

        template<WritableIterator<char> Iter>
        [[nodiscard]]
        inline Iter Format(char ch, const FormatContext<Iter>& context)
        {
            if (m_AsInteger)
            {
                ParseContext parseContext(m_AsInteger ? "i" : "");

                m_Formatter.Parse(parseContext);
                return m_Formatter.Format(static_cast<IntType>(ch), context);
            }

            return Algorithms::CopyN(&ch, 1, context.GetOutput());
        }

    private:
        template<bool Signed>
        struct IntegerTypeHelper { using Type = signed char; };

        template<>
        struct IntegerTypeHelper<false> { using Type = unsigned char; };

    private:
        using IntType = typename IntegerTypeHelper<std::is_signed_v<char>>::Type;

    private:
        bool m_AsInteger = false;
        Formatter<IntType, char> m_Formatter;
    };

    template<std::floating_point T>
    class Formatter<T, char>
    {
    public:
        inline void Parse(const ParseContext& /* context */)
        {
        }

        template<WritableIterator<char> Iter>
        [[nodiscard]]
        inline Iter Format(T floatingPoint, const FormatContext<Iter>& context)
        {
            long double value = static_cast<long double>(floatingPoint);
            Usize count = static_cast<Usize>(std::snprintf(nullptr, 0, "%Lf", value) - 1);

            String str(count, '\0');
            std::snprintf(str.Data(), count, "%Lf", value);

            return Algorithms::Copy(str.GetBegin(), str.GetEnd(), context.GetOutput());
        }

    private:
        T m_Base;
    };

    template<typename T>
    class Formatter<T*, char>
    {
    public:
        inline void Parse(const ParseContext& /* context */)
        {
            StringView specs = "0#X";
            m_Formatter.Parse(ParseContext(specs));
        }

        template<WritableIterator<char> Iter>
        [[nodiscard]]
        inline Iter Format(T* value, const FormatContext<Iter>& context)
        {
            Uintptr ptr;
            std::memcpy(&ptr, &value, sizeof(void*));

            return m_Formatter.Format(ptr, context);
        }

    private:
        Formatter<Uintptr, char> m_Formatter;
    };

    template<>
    class Formatter<StringView, char>
    {
    public:
        inline void Parse(const ParseContext& /* context */)
        {
        }

        template<WritableIterator<char> Iter>
        [[nodiscard]]
        inline Iter Format(const StringView str, const FormatContext<Iter>& context)
        {
            return Algorithms::Copy(str.GetBegin(), str.GetEnd(),
                                    context.GetOutput());
        }
    };
}
