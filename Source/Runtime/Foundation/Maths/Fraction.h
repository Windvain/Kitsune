#pragma once

#include <concepts>
#include "Foundation/Common/Types.h"

namespace Kitsune
{
    namespace Details
    {
        template<Usize Size>
        struct PickFractionFloat { /* ... */ };

        template<> struct PickFractionFloat<1> { using Type = float; };
        template<> struct PickFractionFloat<2> { using Type = float; };
        template<> struct PickFractionFloat<4> { using Type = float; };
        template<> struct PickFractionFloat<8> { using Type = double; };
    }

    template<std::integral T,
             std::floating_point Float = typename Details::PickFractionFloat<sizeof(T)>::Type>
    class Fraction
    {
    public:
        inline Fraction() : Numerator(T()), Denominator(T()) { /* ... */ }
        inline Fraction(T numerator, T denominator)
            : Numerator(numerator), Denominator(denominator)
        {
        }

    public:
        inline Float Value() const
        {
            return Numerator / Denominator;
        }

    public:
        T Numerator;
        T Denominator;
    };
}
