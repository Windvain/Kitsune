#pragma once

#include <memory>
#include <concepts>

#include "Foundation/Concepts/Character.h"
#include "Foundation/Iterators/Iterator.h"

#include "Foundation/String/ToString.h"
#include "Foundation/String/FormatException.h"

namespace Kitsune
{
    namespace Internal
    {
        [[noreturn]]
        KITSUNE_FORCEINLINE void ThrowIncorrectFormatSpecs()
        {
            throw FormatException("The format string contains an invalid format specification.");
        }
    }

    template<Character Char>
    class ParseContext
    {
    public:
        using CharType = Char;
        using ViewType = BasicStringView<Char>;

        using Iterator = ViewType::ConstIterator;
        using ConstIterator = ViewType::ConstIterator;

    public:
        ParseContext() = default;
        explicit ParseContext(const ViewType& formatSpec)
            : m_FormatSpec(formatSpec)
        {
        };

    public:
        ConstIterator GetBegin() const { return m_FormatSpec.GetBegin(); }
        ConstIterator GetEnd() const { return m_FormatSpec.GetEnd(); }

    private:
        ViewType m_FormatSpec;
    };

    template<typename T, Character Char>
    class Formatter
    {
    public:
        static_assert(false,
            "A valid specicalization for Formatter<T> has not been defined.");
    };

    template<>
    class Formatter<bool, char>
    {
    public:
        void Parse(const ParseContext<char>& context)
        {
            auto it = context.GetBegin();
            if (it == context.GetEnd())
                return;

            m_ShowInt = (*it != 's') && (*it != 'S');
            ++it;

            // Make sure we only have one argument.
            if (it != context.GetEnd())
                Internal::ThrowIncorrectFormatSpecs();
        }

        String Format(bool val)
        {
            String out;
            if (!m_ShowInt) out = val ? "true" : "false";
            else            out = val ? "1" : "0";

            return out;
        }

    private:
        bool m_ShowInt = false;
    };

    template<std::integral T>
    class Formatter<T, char>
    {
    public:
        void Parse(const ParseContext<char>& context)
        {
            auto it = context.GetBegin();
            if (it == context.GetEnd())
                return;

            m_Base = ((*it == 'b') || (*it == 'B')) ? 2 :
                     ((*it == 'o') || (*it == 'O')) ? 8 :
                     ((*it == 'd') || (*it == 'D')) ? 10 :
                     ((*it == 'x') || (*it == 'X')  ? 16 :
                     10);

            ++it;
            if (it != context.GetEnd())
                Internal::ThrowIncorrectFormatSpecs();
        }

        String Format(T integer)
        {
            return ToString(integer, m_Base);
        }

    private:
        T m_Base = 10;
    };

    template<std::floating_point T>
    class Formatter<T, char>
    {
    public:
        void Parse(const ParseContext<char>&) { /* ... */ }
        String Format(T val) { return ToString(val); }
    };

    template<>
    class Formatter<char, char>
    {
    public:
        void Parse(const ParseContext<char>& context)
        {
            auto it = context.GetBegin();
            if (it == context.GetEnd())
                return;

            m_IntPresent = ((*it != 'c') && (*it != 'C'));
            ++it;

            if (it != context.GetEnd())
                Internal::ThrowIncorrectFormatSpecs();
        }

        String Format(char ch)
        {
            if (m_IntPresent)
                return ToString(ch);

            return String(1, ch);
        }

    private:
        bool m_IntPresent = false;
    };

    template<>
    class Formatter<void*, char>
    {
    public:
        void Parse(const ParseContext<char>&) { /* ... */ };
        String Format(void* ptr)
        {
            Uintptr intRep;
            std::memcpy(&intRep, &ptr, sizeof(void*));

            return ToString(intRep, Uintptr(16));
        }
    };

    template<>
    class Formatter<StringView, char>
    {
    public:
        void Parse(const ParseContext<char>&) { /* ... */ }
        String Format(const StringView strv)
        {
            return String(strv);
        }
    };
}
