#pragma once

#include <cstdint>
#include <type_traits>

#include "Foundation/Common/Predefined.h"

namespace Kitsune
{
    using Int8 = signed char;
    using Int16 = std::int16_t;
    using Int32 = std::int32_t;
    using Int64 = std::int64_t;

    using Uint8 = unsigned char;
    using Uint16 = std::uint16_t;
    using Uint32 = std::uint32_t;
    using Uint64 = std::uint64_t;

    using Usize = std::size_t;
    using Ssize = std::make_signed_t<Usize>;

    using Index = Usize;
    using Ptrdiff = std::ptrdiff_t;

    using Intptr = std::intptr_t;
    using Uintptr = std::uintptr_t;

#if defined(KITSUNE_OS_WINDOWS)
    using NativeChar = wchar_t;
#else
    using NativeChar = char;
#endif

    enum class Byte : Uint8 { /* ... */ };

    template<typename Int>
    inline Byte operator<<(Byte byte, Int offset)
    {
        return Byte(static_cast<unsigned int>(byte) << offset);
    }

    template<typename Int>
    inline Byte operator>>(Byte byte, Int offset)
    {
        return Byte(static_cast<unsigned int>(byte) >> offset);
    }

    template<typename Int>
    inline Byte& operator<<=(Byte& byte, Int offset)
    {
        return (byte = byte << offset);
    }

    template<typename Int>
    inline Byte& operator>>=(Byte& byte, Int offset)
    {
        return (byte = byte >> offset);
    }

    inline Byte operator|(Byte byte1, Byte byte2)
    {
        return Byte(static_cast<unsigned int>(byte1) |
                    static_cast<unsigned int>(byte2));
    }

    inline Byte operator&(Byte byte1, Byte byte2)
    {
        return Byte(static_cast<unsigned int>(byte1) &
                    static_cast<unsigned int>(byte2));
    }

    inline Byte operator^(Byte byte1, Byte byte2)
    {
        return Byte(static_cast<unsigned int>(byte1) ^
                    static_cast<unsigned int>(byte2));
    }

    inline Byte& operator|=(Byte& byte1, Byte byte2)
    {
        return (byte1 = byte1 | byte2);
    }

    inline Byte& operator&=(Byte& byte1, Byte byte2)
    {
        return (byte1 = byte1 & byte2);
    }

    inline Byte& operator^=(Byte& byte1, Byte byte2)
    {
        return (byte1 = byte1 ^ byte2);
    }

    inline Byte operator~(Byte byte)
    {
        return Byte(~static_cast<unsigned int>(byte));
    }
}
