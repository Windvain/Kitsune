#pragma once

#include <cstdio>
#include <concepts>

#include "Foundation/String/String.h"
#include "Foundation/Algorithms/Reverse.h"

namespace Kitsune
{
    namespace Internal
    {
        template<std::unsigned_integral Integer>
        String UnsignedIntToReversedString(Integer value, Integer base)
        {
            const char digits[] = "0123456789ABCDEF";

            String ret;
            Integer remainder;

            do
            {
                remainder = value % base;
                ret += digits[remainder];

                value /= base;
            } while (value != 0);

            return ret;
        }
    }

    template<std::unsigned_integral Integer>
    [[nodiscard]] String ToString(Integer value, Integer base = (Integer)10)
    {
        String ret = Internal::UnsignedIntToReversedString(value, base);
        Algorithms::Reverse(ret.GetBegin(), ret.GetEnd());

        return ret;
    }

    template<std::signed_integral Integer>
    [[nodiscard]] String ToString(Integer value, Integer base = (Integer)10)
    {
        using Unsgn = std::make_unsigned_t<Integer>;
        if (base != 10)
        {
            Unsgn unsgnValue = *(Unsgn*)&value;
            return ToString(unsgnValue, static_cast<Unsgn>(base));
        }

        // I don't know where this implementation comes from, but yeah..
        bool isNegative = (value < 0);
        Unsgn unsgnValue = isNegative ?
            (static_cast<Unsgn>(~value) + (Unsgn)1) :
             static_cast<Unsgn>(value);

        String ret = Internal::UnsignedIntToReversedString(unsgnValue, (Unsgn)base);
        if (isNegative && (base == 10))     // Don't add a negative sign for bases other than decimal.
            ret += '-';

        Algorithms::Reverse(ret.GetBegin(), ret.GetEnd());
        return ret;
    }

    template<std::floating_point Float>
    [[nodiscard]] String ToString(Float value)
    {
        constexpr const char* FormatString =
            std::is_same_v<Float, long double> ? "%Lf" : "%f";

        Usize size = static_cast<Usize>(std::snprintf(nullptr, 0, FormatString, value) - 1);
        String ret(size, '\0');

        std::snprintf(ret.Data(), size + 1, FormatString, value);
        return ret;
    }
}
