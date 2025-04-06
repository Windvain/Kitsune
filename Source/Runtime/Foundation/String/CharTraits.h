#pragma once

#include <cwchar>
#include <cstring>

#include "Foundation/Common/Types.h"
#include "Foundation/Common/Macros.h"
#include "Foundation/Concepts/Character.h"

namespace Kitsune
{
    template<Character T>
    struct CharTraits
    {
        [[nodiscard]]
        static inline Usize Length(const T* str)
        {
            Usize len = 0;
            for (; *str != T(); ++str, ++len);

            return len;
        }

        static inline T* Copy(T* dest, const T* source, Usize count)
        {
            if constexpr (std::is_trivial_v<T>)     // Just in case..
            {
                std::memcpy(dest, source, count * sizeof(T));
                return dest;
            }
            else
            {
                T* tmp = dest;
                for (; *source != T(); ++dest, ++source)
                    *dest = *source;

                *dest = *source;
                return tmp;
            }
        }

        [[nodiscard]]
        static inline int Compare(const T* str1, const T* str2, Usize count)
        {
            for (; count > 0; --count, ++str1, ++str2)
            {
                if (*str1 != *str2)
                    return static_cast<int>(*str1 - *str2);
            }

            return 0;
        }

        [[nodiscard]]
        static inline const T* Find(const T* haystack, Usize count, T needle)
        {
            for (; count > 0; --count, ++haystack)
            {
                if (*haystack == needle)
                    return haystack;
            }

            return nullptr;
        }
    };

    template<>
    struct CharTraits<char>
    {
        [[nodiscard]]
        KITSUNE_FORCEINLINE static Usize Length(const char* str)
        {
            return std::strlen(str);
        }

        KITSUNE_FORCEINLINE
        static char* Copy(char* dest, const char* source, Usize count)
        {
            std::memcpy(dest, source, count * sizeof(char));
            return dest;
        }

        [[nodiscard]]
        KITSUNE_FORCEINLINE static int Compare(const char* str1, const char* str2,
                                               Usize count)
        {
            return std::memcmp(str1, str2, count * sizeof(char));
        }

        [[nodiscard]]
        KITSUNE_FORCEINLINE static const char* Find(const char* str, Usize count,
                                                    char ch)
        {
            return static_cast<const char*>(std::memchr(str, ch, count * sizeof(char)));
        }
    };

    template<>
    struct CharTraits<wchar_t>
    {
        [[nodiscard]]
        KITSUNE_FORCEINLINE static Usize Length(const wchar_t* str)
        {
            return std::wcslen(str);
        }

        KITSUNE_FORCEINLINE
        static wchar_t* Copy(wchar_t* dest, const wchar_t* source, Usize count)
        {
            return std::wmemcpy(dest, source, count);
        }

        [[nodiscard]]
        KITSUNE_FORCEINLINE static int Compare(const wchar_t* str1, const wchar_t* str2,
                                               Usize count)
        {
            return std::wmemcmp(str1, str2, count);
        }

        [[nodiscard]]
        KITSUNE_FORCEINLINE static const wchar_t* Find(const wchar_t* str, Usize count,
                                                       wchar_t ch)
        {
            return std::wmemchr(str, ch, count);
        }
    };
}
