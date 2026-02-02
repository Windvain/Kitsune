#pragma once

#include "Foundation/String/Formatter.h"
#include "Foundation/String/StringView.h"

#include "Foundation/Iterators/Iterator.h"

namespace Kitsune
{
    template<Character Char, OutputIterator<const Char&> OutputIter>
    class BasicFormatArgument
    {
    private:
        enum class FormatType
        {
            Boolean,
            Character,
            UnsignedChar,
            UnsignedShort,
            UnsignedInt,
            UnsignedLong,
            UnsignedLongLong,
            SignedChar,
            SignedShort,
            SignedInt,
            SignedLong,
            SignedLongLong,
            Float,
            Double,
            LongDouble,
            Pointer,
            String,
            Custom
        };

        class CustomTypeHandle
        {
        private:
            using GeneralFunctionType = void(*)();

        public:
            template<typename T>
            inline explicit CustomTypeHandle(const T& value)
                : m_Pointer(AddressOf(value)),
                  m_FormatFunction(reinterpret_cast<GeneralFunctionType>(
                      StaticFormatFunction<T>))
            {
            }

        public:
            [[nodiscard]]
            inline OutputIter Format(const FormatContext<OutputIter>& context) const
            {
                using FunctionType = OutputIter (*)(
                    const void*, const FormatContext<OutputIter>&);

                auto func = reinterpret_cast<FunctionType>(m_FormatFunction);
                return func(m_Pointer, context);
            }

        public:
            template<typename T>
            static OutputIter StaticFormatFunction(
                const void* pointer, const FormatContext<OutputIter>& context)
            {
                const T& typedValue = *static_cast<const T*>(pointer);
                return Formatter<T, Char>::Format(typedValue, context);
            }

        private:
            const void* m_Pointer;
            GeneralFunctionType m_FormatFunction;
        };

    public:
        using HandleType = CustomTypeHandle;

    public:
        template<typename T>
        inline BasicFormatArgument(const T& value)
            : m_EnumType(GetFormatType<T>())
        {
            using PureType = std::remove_cvref_t<T>;
            if constexpr (std::is_same_v<PureType, bool>)
                m_SharedData.Boolean = value;
            else if constexpr (std::is_same_v<PureType, Char>)
                m_SharedData.Character = value;
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
            else if constexpr (std::is_convertible_v<PureType, BasicStringView<Char>>)
                m_SharedData.String = value;
            else if constexpr (std::is_pointer_v<T> || std::is_null_pointer_v<T>)
                m_SharedData.Pointer = value;
            else
                m_SharedData.Custom = CustomTypeHandle(value);
        }

    public:
        template<typename Function>
        inline void Visit(Function func) const
        {
            switch (m_EnumType)
        {
            case FormatType::Boolean:          func(m_SharedData.Boolean);          break;
            case FormatType::Character:        func(m_SharedData.Character);        break;

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
        inline static FormatType GetFormatType()
        {
            using PureType = std::remove_cvref_t<T>;
            if constexpr (std::is_same_v<PureType, bool>)
                return FormatType::Boolean;
            else if constexpr (std::is_same_v<PureType, Char>)
                return FormatType::Character;
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
            else if constexpr (std::is_convertible_v<PureType, BasicStringView<Char>>)
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
            Char Character;

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
            BasicStringView<Char> String;
            CustomTypeHandle Custom;
        } m_SharedData = { 0 };
    };

    template<Character Char, Usize ArgCount, OutputIterator<const Char&> OutputIter>
    class BasicFormatArgumentPack
    {
    public:
        using FormatArgumentType = BasicFormatArgument<Char, OutputIter>;

    public:
        inline const FormatArgumentType& operator[](Index index) const
        {
            if (index >= GetCount())
                throw OutOfRangeException();

            return m_Arguments[index];
        }

    public:
        [[nodiscard]]
        inline Usize GetCount() const
        {
            return ArgCount;
        }

        [[nodiscard]]
        inline const FormatArgumentType* GetArguments() const
        {
            return m_Arguments;
        }

    public:
        // Keep this public access, we need it for aggregate initialization.
        BasicFormatArgument<Char, OutputIter> m_Arguments[ArgCount];
    };

    template<Character Char, OutputIterator<const Char&> OutputIter>
    class BasicFormatArgumentPack<Char, 0, OutputIter>
    {
    public:
        using FormatArgumentType = BasicFormatArgument<Char, OutputIter>;

    public:
        inline const FormatArgumentType& operator[](Index /* index */) const
        {
            throw OutOfRangeException();
        }

    public:
        [[nodiscard]]
        inline Usize GetCount() const
        {
            return 0;
        }

        [[nodiscard]]
        inline const FormatArgumentType* GetArguments() const
        {
            return nullptr;
        }
    };

    template<OutputIterator<const char&> OutputIter>
    using FormatArgument = BasicFormatArgument<char, OutputIter>;

    template<Usize ArgCount, OutputIterator<const char&> OutputIter>
    using FormatArgumentPack = BasicFormatArgumentPack<char, ArgCount, OutputIter>;
}
