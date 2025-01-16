#pragma once

#include <cctype>
#include <type_traits>

#include "Foundation/String/String.h"
#include "Foundation/String/Formatter.h"

#include "Foundation/Memory/AddressOf.h"
#include "Foundation/Diagnostics/Assert.h"

#include "Foundation/Templates/Forward.h"
#include "Foundation/Templates/IsAnyOf.h"

#include "Foundation/Algorithms/Find.h"
#include "Foundation/Algorithms/ForEach.h"

#include "Foundation/String/FormatException.h"
#include "Foundation/Diagnostics/OutOfRangeException.h"

namespace Kitsune
{
    namespace Internal
    {
        [[noreturn]]
        KITSUNE_FORCEINLINE void ThrowUnmatchedBraces()
        {
            throw FormatException("The format string contains an unmatched brace.");
        }

        template<Character Char>
        class FormatCustomTypeHandle
        {
        public:
            typedef BasicString<Char> (*FunctionType)(const void*, const ParseContext<Char>&);

        public:
            template<typename T>
            explicit FormatCustomTypeHandle(const T& value)
                : m_Pointer(AddressOf(value)),
                  m_Function(reinterpret_cast<FunctionType>(StaticParseAndFormat<T>))
            {
            }

        public:
            BasicString<Char> ParseAndFormat(const ParseContext<Char>& specs) const
            {
                return m_Function(m_Pointer, specs);
            }

        public:
            template<typename T>
            static inline BasicString<Char> StaticParseAndFormat(const void* that, const ParseContext<Char>& context)
            {
                const T& typedValue = *static_cast<const T*>(that);
                Formatter<T, Char> formatter;

                formatter.Parse(context);
                return formatter.Format(typedValue);
            }

        private:
            const void* m_Pointer;
            FunctionType m_Function;
        };

        template<typename T, Character Ch,
            bool IsString = IsAnyOf<std::remove_cvref_t<std::decay_t<T>>, Ch*, BasicStringView<Ch>, BasicString<Ch>>,
            bool IsPointer = std::is_pointer_v<T>,
            bool IsCustom = std::is_class_v<T> || std::is_union_v<T> || std::is_enum_v<T>>
        struct NormalizedFormatType
        {
            using Type = std::remove_cvref_t<std::decay_t<T>>;
        };

        template<typename T, Character Ch, bool IsPointer, bool IsCustom>
        struct NormalizedFormatType<T, Ch, true, IsPointer, IsCustom>
        {
            using Type = BasicStringView<Ch>;
        };

        template<typename T, Character Ch>
        struct NormalizedFormatType<T, Ch, false, true, false>
        {
            using Type = void*;
        };

        template<typename T, Character Ch>
        struct NormalizedFormatType<T, Ch, false, false, true>
        {
            using Type = FormatCustomTypeHandle<Ch>;
        };

        enum class FormatType
        {
            Bool,
            SignedChar, UnsignedChar,
            Char, Short, Int, Long, LongLong,
            UnsignedShort, UnsignedInt, UnsignedLong, UnsignedLongLong,
            Float, Double, LongDouble,
            Pointer, StringView,
            CustomType
        };

        template<typename T, typename Ch>
        auto NormalizeFormatType(const T& value)
        {
            using Normalized = typename NormalizedFormatType<T, Ch>::Type;
            return Normalized(value);
        }

        template<typename T, typename Ch>
        constexpr FormatType GetFormatTypeEnum()
        {
            using Normalized = typename NormalizedFormatType<T, Ch>::Type;

            if constexpr (std::is_same_v<Normalized, bool>)                     return FormatType::Bool;
            else if constexpr (std::is_same_v<Normalized, Ch>)                  return FormatType::Char;
            else if constexpr (std::is_same_v<Normalized, short>)               return FormatType::Short;
            else if constexpr (std::is_same_v<Normalized, int>)                 return FormatType::Int;
            else if constexpr (std::is_same_v<Normalized, long>)                return FormatType::Long;
            else if constexpr (std::is_same_v<Normalized, long long>)           return FormatType::LongLong;
            else if constexpr (std::is_same_v<Normalized, signed char>)         return FormatType::SignedChar;
            else if constexpr (std::is_same_v<Normalized, unsigned char>)       return FormatType::UnsignedChar;
            else if constexpr (std::is_same_v<Normalized, unsigned short>)      return FormatType::UnsignedShort;
            else if constexpr (std::is_same_v<Normalized, unsigned int>)        return FormatType::UnsignedInt;
            else if constexpr (std::is_same_v<Normalized, unsigned long>)       return FormatType::UnsignedLong;
            else if constexpr (std::is_same_v<Normalized, unsigned long long>)  return FormatType::UnsignedLongLong;
            else if constexpr (std::is_same_v<Normalized, float>)               return FormatType::Float;
            else if constexpr (std::is_same_v<Normalized, double>)              return FormatType::Double;
            else if constexpr (std::is_same_v<Normalized, long double>)         return FormatType::LongDouble;
            else if constexpr (std::is_same_v<Normalized, BasicStringView<Ch>>) return FormatType::StringView;
            else if constexpr (std::is_same_v<Normalized, void*>)               return FormatType::Pointer;
            else                                                                return FormatType::CustomType;
        }

        template<Character Char>
        class FormatArgument
        {
        public:
            template<typename T>
            FormatArgument(const T& value)
                : m_FormatType(GetFormatTypeEnum<T, Char>())
            {
                using Normalized = typename NormalizedFormatType<T, Char>::Type;
                GetValue<Normalized>() = NormalizeFormatType<T, Char>(value);
            }

        public:
            template<typename T>
            inline T& GetValue()
            {
                if constexpr (std::is_same_v<T, bool>)                       return m_Data.Bool;
                else if constexpr (std::is_same_v<T, Char>)                  return m_Data.Chr;
                else if constexpr (std::is_same_v<T, short>)                 return m_Data.Short;
                else if constexpr (std::is_same_v<T, int>)                   return m_Data.Int;
                else if constexpr (std::is_same_v<T, long>)                  return m_Data.Long;
                else if constexpr (std::is_same_v<T, long long>)             return m_Data.LongLong;
                else if constexpr (std::is_same_v<T, signed char>)           return m_Data.SignedChar;
                else if constexpr (std::is_same_v<T, unsigned char>)         return m_Data.UnsignedChar;
                else if constexpr (std::is_same_v<T, unsigned short>)        return m_Data.UnsignedShort;
                else if constexpr (std::is_same_v<T, unsigned int>)          return m_Data.UnsignedInt;
                else if constexpr (std::is_same_v<T, unsigned long>)         return m_Data.UnsignedLong;
                else if constexpr (std::is_same_v<T, unsigned long long>)    return m_Data.UnsignedLongLong;
                else if constexpr (std::is_same_v<T, float>)                 return m_Data.Float;
                else if constexpr (std::is_same_v<T, double>)                return m_Data.Double;
                else if constexpr (std::is_same_v<T, long double>)           return m_Data.LongDouble;
                else if constexpr (std::is_same_v<T, BasicStringView<Char>>) return m_Data.Strv;
                else if constexpr (std::is_same_v<T, void*>)                 return m_Data.Pointer;
                else                                                         return m_Data.CustomHandle;
            }

            inline FormatType GetTypeEnum() { return m_FormatType; }

        public:
            template<typename Fn>
            void Visit(Fn&& visitor)
            {
                switch (m_FormatType)
                {
                case FormatType::Bool:
                    Forward<Fn>(visitor)(GetValue<bool>()); break;
                case FormatType::Char:
                    Forward<Fn>(visitor)(GetValue<Char>()); break;
                case FormatType::Short:
                    Forward<Fn>(visitor)(GetValue<short>()); break;
                case FormatType::Int:
                    Forward<Fn>(visitor)(GetValue<int>()); break;
                case FormatType::Long:
                    Forward<Fn>(visitor)(GetValue<long>()); break;
                case FormatType::LongLong:
                    Forward<Fn>(visitor)(GetValue<long long>()); break;
                case FormatType::SignedChar:
                    Forward<Fn>(visitor)(GetValue<signed char>()); break;
                case FormatType::UnsignedChar:
                    Forward<Fn>(visitor)(GetValue<unsigned char>()); break;
                case FormatType::UnsignedShort:
                    Forward<Fn>(visitor)(GetValue<unsigned short>()); break;
                case FormatType::UnsignedInt:
                    Forward<Fn>(visitor)(GetValue<unsigned int>()); break;
                case FormatType::UnsignedLong:
                    Forward<Fn>(visitor)(GetValue<unsigned long>()); break;
                case FormatType::UnsignedLongLong:
                    Forward<Fn>(visitor)(GetValue<unsigned long long>()); break;
                case FormatType::Float:
                    Forward<Fn>(visitor)(GetValue<float>()); break;
                case FormatType::Double:
                    Forward<Fn>(visitor)(GetValue<double>()); break;
                case FormatType::LongDouble:
                    Forward<Fn>(visitor)(GetValue<long double>()); break;
                case FormatType::Pointer:
                    Forward<Fn>(visitor)(GetValue<void*>()); break;
                case FormatType::StringView:
                    Forward<Fn>(visitor)(GetValue<BasicStringView<Char>>()); break;
                case FormatType::CustomType:
                    Forward<Fn>(visitor)(GetValue<FormatCustomTypeHandle<Char>>()); break;
                default:
                    KITSUNE_UNREACHABLE();
                };
            }

        private:
            // Do not access directly.
            union
            {
                bool Bool;
                Char Chr;
                short Short;
                int Int;
                long Long;
                long long LongLong;

                signed char SignedChar;
                unsigned char UnsignedChar;
                unsigned short UnsignedShort;
                unsigned int UnsignedInt;
                unsigned long UnsignedLong;
                unsigned long long UnsignedLongLong;

                float Float;
                double Double;
                long double LongDouble;

                void* Pointer;
                BasicStringView<Char> Strv;
                FormatCustomTypeHandle<Char> CustomHandle;
            } m_Data = { 0 };

            FormatType m_FormatType;
        };

        template<Character Char, typename... Args>
        class InternalFormatPack
        {
        public:
            InternalFormatPack(Args&&... args)
                : Arguments{FormatArgument<Char>(args)...}
            {
            }

        public:
            FormatArgument<Char> Arguments[sizeof...(Args)];
        };

        template<Character Char>
        class InternalFormatPack<Char>
        {
        public:
            InternalFormatPack() = default;
        };

        template<Character Char>
        class FormatArgumentPack
        {
        public:
            template<typename... Args>
            FormatArgumentPack(const InternalFormatPack<Char, Args...>& intrPack)
                : m_Size(sizeof...(Args))
            {
                if constexpr (sizeof...(Args) == 0) return;
                else { m_Args = intrPack.Arguments; }
            }

        public:
            FormatArgument<Char> Get(Usize index) const
            {
                if (index >= m_Size) throw OutOfRangeException();
                return m_Args[index];
            }

        private:
            const FormatArgument<Char>* m_Args;
            Usize m_Size;
        };

        template<Character Char>
        Usize GetIndexFromFormatSpec(const BasicStringView<Char>& spec, BasicStringView<Char>& fmtSpec)
        {
            // We're expecting unicode here, so hopefully all characters will be encoded in unicode..
            Int32 index = 0;
            auto it = spec.GetBegin() + 1;

            if (it == spec.GetEnd())
                ThrowIncorrectFormatSpecs();

            for (; it != spec.GetEnd(); ++it)
            {
                if (*it == static_cast<Char>(':'))
                {
                    ++it;
                    break;
                }
                if (!std::isdigit(*it))
                    ThrowIncorrectFormatSpecs();

                index *= 10;
                index -= (*it - static_cast<Char>('0'));
            }

            fmtSpec = BasicStringView<Char>(it, spec.GetEnd() - it);
            index = -index;

            return index;
        }

        template<Character Char>
        class Scanner
        {
        public:
            Scanner(const BasicStringView<Char>& fmt, FormatArgumentPack<Char>&& pack)
                : m_FormatString(fmt), m_FormatPack(Move(pack))
            {
            }

        public:
            void Scan(BasicString<Char>& str)
            {
                KITSUNE_ASSERT(str.IsEmpty(), "Input string should be empty.");

                constexpr Char LeftBrace = static_cast<Char>('{');
                constexpr Char RightBrace = static_cast<Char>('}');

                // Heavily based off GCC's format code.
                while (m_FormatString.Size())
                {
                    auto lbr = Algorithms::Find(m_FormatString.GetBegin(), m_FormatString.GetEnd(), LeftBrace);
                    auto rbr = Algorithms::Find(m_FormatString.GetBegin(), m_FormatString.GetEnd(), RightBrace);

                    // Both are equal to GetEnd(), no braces were found in the format string.
                    if (lbr == rbr)
                    {
                        str += m_FormatString;
                        m_FormatString.RemovePrefix(m_FormatString.Size());
                    }
                    else if (lbr < rbr)
                    {
                        bool isDoubleBrace = (lbr[1] == LeftBrace);
                        Usize end = static_cast<Usize>(lbr + Usize(isDoubleBrace) - m_FormatString.GetBegin());

                        // If rbr == GetEnd(), lbr is left hanging, unless lbr is a double brace.
                        if ((rbr == m_FormatString.GetEnd()) && (lbr[1] != LeftBrace))
                            ThrowUnmatchedBraces();

                        str += BasicStringView<Char>(m_FormatString.GetBegin(), end);
                        m_FormatString.RemovePrefix(end + Usize(isDoubleBrace));

                        if (!isDoubleBrace)
                        {
                            Usize specSize = static_cast<Usize>(Algorithms::Distance(lbr, rbr));
                            BasicStringView<Char> formatSpec = { lbr, specSize };   // From brace to brace.
                            BasicStringView<Char> formatterSpec;                    // From colon to right brace.

                            Usize index = Internal::GetIndexFromFormatSpec(formatSpec, formatterSpec);
                            ParseContext<Char> context = ParseContext<Char>(formatterSpec);

                            m_FormatPack.Get(index).Visit([&](const auto& val)
                            {
                                using ArgType = std::remove_cvref_t<decltype(val)>;
                                if constexpr (std::is_same_v<ArgType, FormatCustomTypeHandle<Char>>)
                                    str += val.ParseAndFormat(context);
                                else
                                {
                                    Formatter<ArgType, Char> formatter;
                                    formatter.Parse(context);
                                    str += formatter.Format(val);
                                }
                            });

                            m_FormatString.RemovePrefix(specSize + 1);
                        }
                    }
                    else /* (lbr > rbr) or (rbr < lbr) */
                    {
                        // lbr is only larger than rbr if and only if a double brace was used.
                        if (rbr[1] != RightBrace) ThrowUnmatchedBraces();
                        Usize end = static_cast<Usize>(rbr + 1 - m_FormatString.GetBegin());

                        str += BasicStringView<Char>(m_FormatString.GetBegin(), end);
                        m_FormatString.RemovePrefix(end + 1);
                    }
                }
            }

        private:
            BasicStringView<Char> m_FormatString;
            FormatArgumentPack<Char> m_FormatPack;
        };

        template<Character Char, typename... Args>
        BasicString<Char> GenericFormat(const BasicStringView<Char>& format, Args&&... args)
        {
            BasicString<Char> formatted;
            Internal::InternalFormatPack<Char, Args...> internalPack(Forward<Args>(args)...);
            Internal::FormatArgumentPack<Char> pack = internalPack;

            Internal::Scanner<Char> scanner(format, Move(pack));
            scanner.Scan(formatted);

            return formatted;
        }
    }

    template<typename... Args>
    String Format(const StringView& format, Args&&... args)
    {
        return Internal::GenericFormat<char>(format, Forward<Args>(args)...);
    }
}
