#pragma once

#include <type_traits>

#include "Foundation/String/String.h"
#include "Foundation/Templates/Forward.h"

#include "Foundation/String/Formatter.h"

namespace Kitsune
{
    enum class FormatType
    {
        Boolean,
        Char,
        UnsignedInteger, SignedInteger,
        FloatingPoint,
        Pointer,
        String,
        Custom
    };

    template<WritableIterator<char> OutIt>
    class CustomTypeHandle
    {
    public:
        template<typename T>
        explicit CustomTypeHandle(const T& value)
            : m_Pointer(AddressOf(value)),
              m_ParseAndFormat(reinterpret_cast<void(*)()>(StaticParseAndFormat<T>))
        {
        }

    public:
        inline OutIt ParseAndFormat(const ParseContext& parseContext,
                                    const FormatContext<CustomTypeHandle, OutIt>& formatContext) const
        {
            typedef OutIt (*FunctionType)(OutIt, const ParseContext&, const void*);
            auto func = reinterpret_cast<FunctionType>(m_ParseAndFormat);

            return func(formatContext.GetOutput(), parseContext, m_Pointer);
        }

    public:
        template<typename T>
        static OutIt StaticParseAndFormat(OutIt out, const ParseContext& parseContext,
                                          const void* pointer)
        {
            const T& typedValue = *static_cast<const T*>(pointer);
            Formatter<T> formatter;

            formatter.Parse(parseContext);
            return formatter.Format(FormatContext(out, typedValue));
        }

    private:
        const void* m_Pointer;
        void (*m_ParseAndFormat)();
    };

    template<WritableIterator<char> OutIt>
    class FormatArgument
    {
    public:
        template<typename T>
        FormatArgument(const T& value)
            : m_EnumType(GetFormatType<T>())
        {
            using Pure = std::remove_cvref_t<T>;

            if constexpr (std::is_same_v<Pure, bool>)
                m_SharedData.Boolean = value;
            else if constexpr (std::is_same_v<Pure, char>)
                m_SharedData.Char = value;
            else if constexpr (std::is_signed_v<Pure> && std::is_integral_v<Pure>)
                m_SharedData.SignedInteger = value;
            else if constexpr (std::is_unsigned_v<Pure> && std::is_integral_v<Pure>)
                m_SharedData.UnsignedInteger = value;
            else if constexpr (std::is_floating_point_v<Pure>)
                m_SharedData.FloatingPoint = value;

            // Important! Check for string should be done before pointer
            // check, b.c. const char* will fail.
            else if constexpr (std::is_convertible_v<Pure, StringView>)
                m_SharedData.String = value;
            else if constexpr (std::is_pointer_v<T>)
                m_SharedData.Pointer = value;
            else
                m_SharedData.Custom = CustomTypeHandle<OutIt>(value);
        }

    public:
        template<typename Fn>
        inline void Visit(Fn&& visitor) const
        {
            switch (m_EnumType)
            {
            case FormatType::Boolean:         Forward<Fn>(visitor)(m_SharedData.Boolean);         break;
            case FormatType::Char:            Forward<Fn>(visitor)(m_SharedData.Char);            break;
            case FormatType::SignedInteger:   Forward<Fn>(visitor)(m_SharedData.SignedInteger);   break;
            case FormatType::UnsignedInteger: Forward<Fn>(visitor)(m_SharedData.UnsignedInteger); break;
            case FormatType::FloatingPoint:   Forward<Fn>(visitor)(m_SharedData.FloatingPoint);   break;
            case FormatType::Pointer:         Forward<Fn>(visitor)(m_SharedData.Pointer);         break;
            case FormatType::String:          Forward<Fn>(visitor)(m_SharedData.String);          break;
            default:
                Forward<Fn>(visitor)(m_SharedData.Custom);
            }
        }

    public:
        template<typename T>
        [[nodiscard]] static inline FormatType GetFormatType()
        {
            using Pure = std::remove_cvref_t<T>;

            if constexpr (std::is_same_v<Pure, bool>)
                return FormatType::Boolean;
            else if constexpr (std::is_same_v<Pure, char>)
                return FormatType::Char;
            else if constexpr (std::is_signed_v<Pure> && std::is_integral_v<Pure>)
                return FormatType::SignedInteger;
            else if constexpr (std::is_unsigned_v<Pure> && std::is_integral_v<Pure>)
                return FormatType::UnsignedInteger;
            else if constexpr (std::is_floating_point_v<Pure>)
                return FormatType::FloatingPoint;

            // Important! Check for string should be done before pointer
            // check, b.c. const char* will fail.
            else if constexpr (std::is_convertible_v<Pure, StringView>)
                return FormatType::String;
            else if constexpr (std::is_pointer_v<T>)
                return FormatType::Pointer;
            else
                return FormatType::Custom;
        }

    private:
        FormatType m_EnumType;
        union
        {
            bool Boolean;
            char Char;
            Uint64 UnsignedInteger;
            Int64 SignedInteger;
            long double FloatingPoint;
            void* Pointer;
            StringView String;
            CustomTypeHandle<OutIt> Custom;
        } m_SharedData = { 0 };
    };

    template<WritableIterator<char> OutIt, Usize N>
    class FormatArgumentStore
    {
    public:
        FormatArgument<OutIt> Data[N];
    };

    template<WritableIterator<char> OutIt>
    class FormatArgumentPack
    {
    public:
        template<Usize N>
        FormatArgumentPack(const FormatArgumentStore<OutIt, N>& store)
            : m_Size(N), m_Arguments(store.Data)
        {
        }

    public:
        const FormatArgument<OutIt>& operator[](Index index) const
        {
            if (index >= GetCount())
                throw OutOfRangeException();

            return m_Arguments[index];
        }

    public:
        [[nodiscard]] Usize GetCount() const { return m_Size; }
        [[nodiscard]] const FormatArgument<OutIt>* GetArguments() const
        {
            return m_Arguments;
        }

    private:
        Usize m_Size;
        const FormatArgument<OutIt>* m_Arguments;
    };

    template<WritableIterator<char> OutIt, typename... Args>
    [[nodiscard]]
    FormatArgumentStore<OutIt, sizeof...(Args)> MakeFormatArgumentPack(Args&&... args)
    {
        return { Forward<Args>(args)... };
    }
}
