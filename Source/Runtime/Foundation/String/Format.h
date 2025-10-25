#pragma once

#include <cstdlib>
#include <type_traits>

#include "Foundation/String/String.h"
#include "Foundation/String/Formatter.h"
#include "Foundation/String/FormatException.h"

#include "Foundation/Algorithms/Find.h"
#include "Foundation/Iterators/Iterator.h"
#include "Foundation/Iterators/BackInsertIterator.h"

namespace Kitsune
{
    namespace Details
    {
        enum class FormatType
        {
            Boolean, Char,
            UnsignedChar, UnsignedShort, UnsignedInt, UnsignedLong, UnsignedLongLong,
            SignedChar, SignedShort, SignedInt, SignedLong, SignedLongLong,
            Float, Double, LongDouble,
            Pointer, String,
            Custom
        };

        template<WritableIterator<char> OutIt, Character Char>
        class CustomTypeHandle
        {
        private:
            using GeneralFunctionType = void(*)();

        public:
            template<typename T>
            inline explicit CustomTypeHandle(const T& value)
                : m_Pointer(AddressOf(value)),
                  m_ParseAndFormat(reinterpret_cast<GeneralFunctionType>(StaticParseAndFormat<T>))
            {
            }

        public:
            [[nodiscard]]
            inline OutIt ParseAndFormat(const BasicParseContext<Char>& parseContext,
                                        const BasicFormatContext<Char, OutIt>& formatContext) const
            {
                using FunctionType = OutIt (*)(const BasicParseContext<Char>&,
                                               const BasicFormatContext<Char, OutIt>&, const void*);

                auto func = reinterpret_cast<FunctionType>(m_ParseAndFormat);
                return func(parseContext, formatContext, m_Pointer);
            }

        public:
            template<typename T>
            static OutIt StaticParseAndFormat(const BasicParseContext<Char>& parseContext,
                                              const BasicFormatContext<Char, OutIt>& formatContext,
                                              const void* pointer)
            {
                const T& typedValue = *static_cast<const T*>(pointer);
                Formatter<T, Char> formatter;

                formatter.Parse(parseContext);
                return formatter.Format(typedValue, formatContext);
            }

        private:
            const void* m_Pointer;
            GeneralFunctionType m_ParseAndFormat;
        };

        template<WritableIterator<char> OutIt, Character Ch>
        class FormatArgument
        {
        public:
            template<typename T>
            inline FormatArgument(const T& value)
                : m_EnumType(GetFormatType<T>())
            {
                using PureType = std::remove_cvref_t<T>;

                if constexpr (std::is_same_v<PureType, bool>)
                    m_SharedData.Boolean = value;
                else if constexpr (std::is_same_v<PureType, Ch>)
                    m_SharedData.Char = value;
                else if constexpr (std::is_same_v<PureType, unsigned char>)
                    m_SharedData.UnsignedChar = value;
                else if constexpr (std::is_same_v<PureType, unsigned short>)
                    m_SharedData.UnsignedShort = value;
                else if constexpr (std::is_same_v<PureType, unsigned int>)
                    m_SharedData.UnsignedInt = value;
                else if constexpr (std::is_same_v<PureType, unsigned long>)
                    m_SharedData.UnsignedLong = value;
                else if constexpr (std::is_same_v<PureType, unsigned long long>)
                    m_SharedData.UnsignedLongLong = value;

                else if constexpr (std::is_same_v<PureType, signed char>)
                    m_SharedData.SignedChar = value;
                else if constexpr (std::is_same_v<PureType, signed short>)
                    m_SharedData.SignedShort = value;
                else if constexpr (std::is_same_v<PureType, signed int>)
                    m_SharedData.SignedInt = value;
                else if constexpr (std::is_same_v<PureType, signed long>)
                    m_SharedData.SignedLong = value;
                else if constexpr (std::is_same_v<PureType, signed long long>)
                    m_SharedData.SignedLongLong = value;

                else if constexpr (std::is_same_v<PureType, float>)
                    m_SharedData.Float = value;
                else if constexpr (std::is_same_v<PureType, double>)
                    m_SharedData.Double = value;
                else if constexpr (std::is_same_v<PureType, long double>)
                    m_SharedData.LongDouble = value;

                // Important! Check for string should be done before pointer
                // check, b.c. const char* will fail.
                else if constexpr (std::is_convertible_v<PureType, BasicStringView<Ch>>)
                    m_SharedData.String = value;
                else if constexpr (std::is_pointer_v<T> || std::is_null_pointer_v<T>)
                    m_SharedData.Pointer = value;
                else
                    m_SharedData.Custom = CustomTypeHandle<OutIt, Ch>(value);
            }

        public:
            template<typename Fn>
            inline void RetrieveValue(Fn func) const
            {
                switch (m_EnumType)
            {
                case FormatType::Boolean:          func(m_SharedData.Boolean);          break;
                case FormatType::Char:             func(m_SharedData.Char);             break;

                case FormatType::SignedChar:       func(m_SharedData.SignedChar);       break;
                case FormatType::SignedShort:      func(m_SharedData.SignedShort);      break;
                case FormatType::SignedInt:        func(m_SharedData.SignedInt);        break;
                case FormatType::SignedLong:       func(m_SharedData.SignedLong);       break;
                case FormatType::SignedLongLong:   func(m_SharedData.SignedLongLong);   break;

                case FormatType::UnsignedChar:     func(m_SharedData.UnsignedChar);     break;
                case FormatType::UnsignedShort:    func(m_SharedData.UnsignedShort);    break;
                case FormatType::UnsignedInt:      func(m_SharedData.UnsignedInt);      break;
                case FormatType::UnsignedLong:     func(m_SharedData.UnsignedLong);     break;
                case FormatType::UnsignedLongLong: func(m_SharedData.UnsignedLongLong); break;

                case FormatType::Float:            func(m_SharedData.Float);            break;
                case FormatType::Double:           func(m_SharedData.Double);           break;
                case FormatType::LongDouble:       func(m_SharedData.LongDouble);       break;

                case FormatType::Pointer:          func(m_SharedData.Pointer);          break;
                case FormatType::String:           func(m_SharedData.String);           break;
                default:
                    func(m_SharedData.Custom);
                }
            }

        public:
            template<typename T>
            [[nodiscard]] static inline FormatType GetFormatType()
            {
                using PureType = std::remove_cvref_t<T>;

                if constexpr (std::is_same_v<PureType, bool>)
                    return FormatType::Boolean;
                else if constexpr (std::is_same_v<PureType, Ch>)
                    return FormatType::Char;
                else if constexpr (std::is_same_v<PureType, unsigned char>)
                    return FormatType::UnsignedChar;
                else if constexpr (std::is_same_v<PureType, unsigned short>)
                    return FormatType::UnsignedShort;
                else if constexpr (std::is_same_v<PureType, unsigned int>)
                    return FormatType::UnsignedInt;
                else if constexpr (std::is_same_v<PureType, unsigned long>)
                    return FormatType::UnsignedLong;
                else if constexpr (std::is_same_v<PureType, unsigned long long>)
                    return FormatType::UnsignedLongLong;

                else if constexpr (std::is_same_v<PureType, signed char>)
                    return FormatType::SignedChar;
                else if constexpr (std::is_same_v<PureType, signed short>)
                    return FormatType::SignedShort;
                else if constexpr (std::is_same_v<PureType, signed int>)
                    return FormatType::SignedInt;
                else if constexpr (std::is_same_v<PureType, signed long>)
                    return FormatType::SignedLong;
                else if constexpr (std::is_same_v<PureType, signed long long>)
                    return FormatType::SignedLongLong;

                else if constexpr (std::is_same_v<PureType, float>)
                    return FormatType::Float;
                else if constexpr (std::is_same_v<PureType, double>)
                    return FormatType::Double;
                else if constexpr (std::is_same_v<PureType, long double>)
                    return FormatType::LongDouble;

                // Important! Check for string should be done before pointer
                // check, b.c. const char* will fail.
                else if constexpr (std::is_convertible_v<PureType, BasicStringView<Ch>>)
                    return FormatType::String;
                else if constexpr (std::is_pointer_v<T> || std::is_null_pointer_v<T>)
                    return FormatType::Pointer;
                else
                    return FormatType::Custom;
            }

        private:
            FormatType m_EnumType;
            union
            {
                bool Boolean;
                Ch Char;

                unsigned char UnsignedChar;
                unsigned short UnsignedShort;
                unsigned int UnsignedInt;
                unsigned long UnsignedLong;
                unsigned long long UnsignedLongLong;

                signed char SignedChar;
                signed short SignedShort;
                signed int SignedInt;
                signed long SignedLong;
                signed long long SignedLongLong;

                float Float;
                double Double;
                long double LongDouble;

                void* Pointer;
                BasicStringView<Ch> String;
                CustomTypeHandle<OutIt, Ch> Custom;
            } m_SharedData = { 0 };
        };

        template<Character Char, WritableIterator<Char> OutIt, Usize N>
        class FormatArgumentStore
        {
        public:
            FormatArgument<OutIt, Char> Data[N];
        };

        template<Character Char, WritableIterator<Char> OutIt>
        class FormatArgumentStore<Char, OutIt, 0>
        {
        public:
            FormatArgument<OutIt, Char>* Data = nullptr;
        };

        template<Character Char, WritableIterator<Char> OutIt>
        class FormatArgumentPack
        {
        public:
            template<Usize N>
            inline FormatArgumentPack(FormatArgumentStore<Char, OutIt, N>& store)
                : m_Size(N), m_Arguments(store.Data)
            {
            }

        public:
            inline const FormatArgument<OutIt, Char>& operator[](Index index) const
            {
                if (index >= GetCount())
                    throw OutOfRangeException();

                return m_Arguments[index];
            }

        public:
            [[nodiscard]] inline Usize GetCount() const { return m_Size; }
            [[nodiscard]] inline const FormatArgument<OutIt, Char>* GetArguments() const
            {
                return m_Arguments;
            }

        private:
            Usize m_Size;
            const FormatArgument<OutIt, Char>* m_Arguments;
        };

        template<Character Char>
        class DefaultFormatScanner;

        template<>
        class DefaultFormatScanner<char>
        {
        public:
            using CharType = char;

        public:
            inline explicit DefaultFormatScanner(const BasicStringView<CharType> fmt)
                : m_BeginSpecs(fmt.GetBegin()), m_EndSpecs(fmt.GetBegin()),
                  m_EndFormat(fmt.GetEnd())
            {
            }

            [[nodiscard]]
            inline BasicStringView<CharType> Next()
            {
                // Heavily based off GCC's format code.
                using namespace Details;

                auto lbr = Algorithms::Find(m_EndSpecs, m_EndFormat, '{');
                auto rbr = Algorithms::Find(m_EndSpecs, m_EndFormat, '}');

                if (lbr == rbr)
                    m_BeginSpecs = m_EndSpecs = m_EndFormat;
                else if (lbr < rbr)
                {
                    bool isDoubleBrace = (lbr[1] == '{');

                    // If rbr == GetEnd(), lbr is left hanging, unless lbr is a double brace.
                    if ((rbr == m_EndFormat) && !isDoubleBrace)
                        throw FormatException("Format string contains mismatched braces.");

                    m_BeginSpecs = lbr;
                    m_EndSpecs = (isDoubleBrace) ? (lbr + 2) : (rbr + 1);
                }
                else
                {
                    // lbr is only larger than rbr if and only if a double brace was used.
                    if (rbr[1] != '}')
                        throw FormatException("Format string contains mismatched braces.");

                    m_BeginSpecs = rbr;
                    m_EndSpecs = rbr + 2;
                }

                return BasicStringView<CharType>(m_BeginSpecs, m_EndSpecs);
            }

            [[nodiscard]]
            inline Index GetIndex() const
            {
                // HACK: For special characters. ('{{' and '}}')
                if ((m_EndSpecs - m_BeginSpecs) == 2)
                    return Index(-1);

                auto isEnd = [](char ch) -> bool { return (ch == '}') || (ch == ':'); };
                auto colon = Algorithms::FindIf(m_BeginSpecs, m_EndSpecs, isEnd);

                if (colon == m_EndSpecs)
                    return Index(-1);

                String substr(m_BeginSpecs + 1, colon);
                auto isNotDigit = [](char ch) -> bool { return (ch < '0') || (ch > '9'); };

                if (Algorithms::FindIf(substr.GetBegin(), substr.GetEnd(), isNotDigit) != substr.GetEnd())
                    throw FormatException("Tried to parse an invalid index argument.");

                return static_cast<Index>(std::atoll(substr.Raw()));
            }

            [[nodiscard]]
            inline BasicStringView<CharType> GetArguments() const
            {
                auto colon = Algorithms::Find(m_BeginSpecs, m_EndSpecs, ':');
                if (colon != m_EndSpecs)
                    ++colon;

                return StringView(colon, m_EndSpecs);
            }

            [[nodiscard]]
            inline CharType GetSpecialCharacter() const
            {
                if ((m_EndSpecs - m_BeginSpecs) != 2)
                    throw FormatException("Current format specification is not a special character.");

                if ((m_BeginSpecs[0] == '{') && (m_BeginSpecs[1] == '{'))
                    return '{';
                else if ((m_BeginSpecs[0] == '}') && (m_BeginSpecs[1] == '}'))
                    return '}';
                else
                    throw FormatException("Current format specification is invalid.");
            }

        private:
            StringView::Iterator m_BeginSpecs;
            StringView::Iterator m_EndSpecs;
            StringView::Iterator m_EndFormat;
        };
    }

    template<Character Char, WritableIterator<Char> OutIt, typename... Args>
    inline void FormatTo(OutIt out, BasicStringView<Char> fmt, Args&&... args)
    {
        using namespace Details;
        DefaultFormatScanner<Char> scanner(fmt);

        // REMINDER: This should be able to be removed once pack indexing gets implemented. (C++26)
        auto argumentStore = FormatArgumentStore<Char, OutIt, sizeof...(Args)>{ Forward<Args>(args)... };
        FormatArgumentPack<Char, OutIt> argumentPack(argumentStore);

        auto prev = fmt.GetBegin();
        while (true)
        {
            BasicStringView<Char> formatSpecs = scanner.Next();

            // Copy everything that comes before the format specifiers.
            out = Algorithms::Copy(prev, formatSpecs.GetBegin(), out);
            prev = formatSpecs.GetEnd();

            if (formatSpecs.IsEmpty())
                break;

            // Insert special characters.
            Index index = scanner.GetIndex();
            if (index == Index(-1))
            {
                *out = scanner.GetSpecialCharacter();
                continue;
            }

            BasicParseContext<Char> parseContext(scanner.GetArguments());
            BasicFormatContext<Char, OutIt> formatContext(out);

            // Parse and format!
            argumentPack[index].RetrieveValue([&](const auto value) -> void
            {
                using ValueType = std::remove_cvref_t<decltype(value)>;

                if constexpr (std::is_same_v<ValueType, CustomTypeHandle<OutIt, Char>>)
                    out = value.ParseAndFormat(parseContext, formatContext);
                else
                {
                    Formatter<ValueType, Char> formatter;
                    formatter.Parse(parseContext);

                    out = formatter.Format(value, formatContext);
                }
            });
        }
    }

    template<typename... Args>
    [[nodiscard]]
    inline String Format(const StringView fmt, Args&&... args)
    {
        String str;
        FormatTo(BackInsertIterator<String>(str), fmt, Forward<Args>(args)...);

        return str;
    }
}
