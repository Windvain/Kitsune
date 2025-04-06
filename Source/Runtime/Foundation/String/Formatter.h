#pragma once

#include <cstdio>

#include "Foundation/String/String.h"
#include "Foundation/String/FormatException.h"

#include "Foundation/Algorithms/Copy.h"
#include "Foundation/Algorithms/Reverse.h"

namespace Kitsune
{
    namespace Internal
    {
        [[noreturn]]
        KITSUNE_FORCEINLINE void ThrowTypeInvalidSpecifier()
        {
            throw FormatException("Formatter<T> tried to parse an invalid format specifier.");
        }
    }

    class ParseContext
    {
    public:
        using ViewType = StringView;

        using Iterator = ViewType::ConstIterator;
        using ConstIterator = ViewType::ConstIterator;

    public:
        ParseContext() = default;
        explicit ParseContext(const ViewType specifier)
            : m_FormatSpecifier(specifier)
        {
        }

    public:
        [[nodiscard]]
        inline ConstIterator GetBegin() const
        {
            return m_FormatSpecifier.GetBegin();
        }

        [[nodiscard]]
        inline ConstIterator GetEnd() const
        {
            return m_FormatSpecifier.GetEnd();
        }

    public:
        [[nodiscard]]
        inline Usize GetSpecifierLength() const
        {
            return m_FormatSpecifier.Size();
        }

    private:
        StringView m_FormatSpecifier;
    };

    template<typename T, WritableIterator<char> Iter>
    class FormatContext
    {
    public:
        using Iterator = Iter;

    public:
        explicit FormatContext(Iter outputIter, const T& value)
            : m_OutputIter(outputIter), m_Value(value)
        {
        }

    public:
        [[nodiscard]] inline Iterator GetOutput() const { return m_OutputIter; }
        [[nodiscard]] inline const T& GetValue()  const { return m_Value; }

    private:
        Iter m_OutputIter;
        const T& m_Value;
    };

    template<typename T>
    class Formatter { /* ... */ };

    template<>
    class Formatter<bool>
    {
    public:
        void Parse(const ParseContext& context)
        {
            if (context.GetSpecifierLength() > 1)
                Internal::ThrowTypeInvalidSpecifier();

            if (context.GetSpecifierLength() == 0)
                m_AsInteger = false;
            else
            {
               m_AsInteger = (*context.GetBegin() == 'i') ||
                             (*context.GetBegin() == 'I');
            }
        }

        template<WritableIterator<char> Iter>
        Iter Format(const FormatContext<bool, Iter>& context)
        {
            StringView str = (m_AsInteger) ? (context.GetValue() ? "1"    : "0") :
                                             (context.GetValue() ? "true" : "false");

            return Algorithms::Copy(str.GetBegin(), str.GetEnd(),
                                    context.GetOutput());
        }

    private:
        bool m_AsInteger;
    };

    template<>
    class Formatter<char>
    {
    public:
        void Parse(const ParseContext& /* context */)
        {
        }

        template<WritableIterator<char> Iter>
        Iter Format(const FormatContext<char, Iter>& context)
        {
            return Algorithms::CopyN(&context.GetValue(), 1, context.GetOutput());
        }
    };


    template<std::integral T>
    class Formatter<T>
    {
    public:
        void Parse(const ParseContext& context)
        {
            if (context.GetSpecifierLength() > 1)
                Internal::ThrowTypeInvalidSpecifier();

            auto it = context.GetBegin();
            m_Base = ((*it == 'b') || (*it == 'B')) ? 2 :
                     ((*it == 'o') || (*it == 'O')) ? 8 :
                     ((*it == 'd') || (*it == 'D')) ? 10 :
                     ((*it == 'x') || (*it == 'X')  ? 16 :
                                                      10);
        }

        template<WritableIterator<char> Iter>
        Iter Format(const FormatContext<T, Iter>& context)
        {
            using UnsignedType = std::make_unsigned_t<T>;
            String str;

            UnsignedType remainder;
            UnsignedType value;

            bool isNegative = (context.GetValue() < 0);

            if ((m_Base != 10) || !isNegative)
                std::memcpy(&value, &context.GetValue(), sizeof(T));
            else
            {
                value = static_cast<UnsignedType>(~context.GetValue()) + 1;
            }

            do
            {
                remainder = value % m_Base;
                str += s_DigitsRep[remainder];

                value /= m_Base;
            } while (value != 0);

            if (isNegative && (m_Base == 10))
                str += '-';

            Algorithms::Reverse(str.GetBegin(), str.GetEnd());
            return Algorithms::Copy(str.GetBegin(), str.GetEnd(),
                                    context.GetOutput());
        }

    private:
        constexpr static char s_DigitsRep[] = "0123456789ABCDEF";
        T m_Base;
    };

    template<std::floating_point T>
    class Formatter<T>
    {
    public:
        void Parse(const ParseContext& /* context */)
        {
        }

        template<WritableIterator<char> Iter>
        Iter Format(const FormatContext<T, Iter>& context)
        {
            long double value = static_cast<long double>(context.GetValue());
            Usize count = static_cast<Usize>(std::snprintf(nullptr, 0, "%Lf", value) - 1);

            String str(count, '\0');
            std::snprintf(str.Data(), count, "%Lf", value);

            return Algorithms::Copy(str.GetBegin(), str.GetEnd(), context.GetOutput());
        }

    private:
        constexpr static char s_DigitsRep[] = "0123456789ABCDEF";
        T m_Base;
    };

    template<typename T>
    class Formatter<T*>
    {
    public:
        void Parse(const ParseContext& /* context */)
        {
            StringView specs = "X";
            m_Formatter.Parse(ParseContext(specs));
        }

        template<WritableIterator<char> Iter>
        Iter Format(const FormatContext<T*, Iter>& context)
        {
            Uintptr ptr;
            std::memcpy(&ptr, &context.GetValue(), sizeof(void*));

            auto modified = FormatContext<Uintptr, Iter>(context.GetOutput(), ptr);
            return m_Formatter.Format(modified);
        }

    private:
        Formatter<Uint64> m_Formatter;
    };

    template<>
    class Formatter<StringView>
    {
    public:
        void Parse(const ParseContext& /* context */)
        {
        }

        template<WritableIterator<char> Iter>
        Iter Format(const FormatContext<StringView, Iter>& context)
        {
            StringView str = context.GetValue();
            return Algorithms::Copy(str.GetBegin(), str.GetEnd(),
                                    context.GetOutput());
        }
    };
}
