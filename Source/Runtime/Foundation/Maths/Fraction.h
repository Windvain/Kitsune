#pragma once

#include <concepts>

namespace Kitsune
{
    template<std::integral T>
    class Fraction
    {
    public:
        using FloatType = std::conditional_t<
            (sizeof(T) > sizeof(float)),
            double,
            float>;

    public:
        inline Fraction() : Numerator(T()), Denominator(T()) { /* ... */ }
        inline Fraction(T numerator, T denominator)
            : Numerator(numerator), Denominator(denominator)
        {
        }

    public:
        inline FloatType Value() const
        {
            return Numerator / Denominator;
        }

    public:
        T Numerator;
        T Denominator;
    };
}
