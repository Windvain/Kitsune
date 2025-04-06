#pragma once

#include <type_traits>

#define KITSUNE_OVERLOAD_FLAGS_OPERATORS(Flags)                                  \
    static_assert(std::is_enum_v<Flags> &&                                       \
                  !std::is_convertible_v<Flags, std::underlying_type_t<Flags>>,  \
                  "Flags should be an enum class.");                             \
                                                                                 \
    Flags& operator|=(Flags& lhs, Flags rhs)                                     \
    {                                                                            \
        using UnderlyingType = std::underlying_type_t<Flags>;                    \
                                                                                 \
        lhs = Flags(UnderlyingType(lhs) | UnderlyingType(rhs));                  \
        return lhs;                                                              \
    }                                                                            \
                                                                                 \
    Flags& operator&=(Flags& lhs, Flags rhs)                                     \
    {                                                                            \
        using UnderlyingType = std::underlying_type_t<Flags>;                    \
                                                                                 \
        lhs = Flags(UnderlyingType(lhs) | UnderlyingType(rhs));                  \
        return lhs;                                                              \
    }                                                                            \
                                                                                 \
                                                                                 \
    Flags operator|(Flags lhs, Flags rhs) { return (lhs |= rhs); }               \
    Flags operator&(Flags lhs, Flags rhs) { return (lhs &= rhs); }               \
