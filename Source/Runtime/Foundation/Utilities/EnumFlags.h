#pragma once

#include <type_traits>

// Helper macro for overloading bitwise operators on enum classes.
#define KITSUNE_OVERLOAD_FLAGS_OPERATORS(Flags)                                  \
    static_assert(std::is_enum_v<Flags> &&                                       \
                  !std::is_convertible_v<Flags, std::underlying_type_t<Flags>>,  \
                  "Flags should be an enum class.");                             \
                                                                                 \
    constexpr Flags& operator|=(Flags& lhs, Flags rhs)                           \
    {                                                                            \
        using UnderlyingType = std::underlying_type_t<Flags>;                    \
                                                                                 \
        lhs = Flags(UnderlyingType(lhs) | UnderlyingType(rhs));                  \
        return lhs;                                                              \
    }                                                                            \
                                                                                 \
    constexpr Flags& operator&=(Flags& lhs, Flags rhs)                           \
    {                                                                            \
        using UnderlyingType = std::underlying_type_t<Flags>;                    \
                                                                                 \
        lhs = Flags(UnderlyingType(lhs) & UnderlyingType(rhs));                  \
        return lhs;                                                              \
    }                                                                            \
                                                                                 \
                                                                                 \
    constexpr Flags operator|(Flags lhs, Flags rhs) { return (lhs |= rhs); }     \
    constexpr Flags operator&(Flags lhs, Flags rhs) { return (lhs &= rhs); }     \
