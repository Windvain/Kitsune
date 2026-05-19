#pragma once

#include "Foundation/Templates/Swap.h"
#include "Foundation/Templates/Forward.h"

#include "Foundation/Concepts/Comparable.h"

namespace Kitsune
{
    template<typename T, typename U>
    class Pair
    {
    public:
        using FirstType = T;
        using SecondType = U;

    public:
        inline Pair()
            : First(), Second()
        {
        }

        inline Pair(const T& value1, const U& value2)
            : First(value1), Second(value2)
        {
        }

        template<typename T2, typename U2>
            requires std::constructible_from<T, T2> && std::constructible_from<U, U2>
        inline Pair(T2&& value1, U2&& value2)
            : First(Forward<T2>(value1)), Second(Forward<U2>(value2))
        {
        }

        template<typename T2, typename U2>
            requires std::constructible_from<T, T2> && std::constructible_from<U, U2>
        inline Pair(const Pair<T2, U2>& pair)
            : First(pair.First), Second(pair.Second)
        {
        }

        template<typename T2, typename U2>
            requires std::constructible_from<T, T2> && std::constructible_from<U, U2>
        inline Pair(Pair<T2, U2>&& pair)
            : First(Move(pair.First)), Second(Move(pair.Second))
        {
        }

        Pair(const Pair&) = default;
        Pair(Pair&&) = default;

    public:
        Pair& operator=(const Pair&) = default;
        Pair& operator=(Pair&&) = default;

        template<typename T2, typename U2>
            requires std::constructible_from<T, T2> && std::constructible_from<U, U2>
        inline Pair& operator=(const Pair<T2, U2>& pair)
        {
            First = pair.First;
            Second = pair.Second;

            return *this;
        }

        template<typename T2, typename U2>
            requires std::constructible_from<T, T2> && std::constructible_from<U, U2>
        inline Pair& operator=(Pair<T2, U2>&& pair)
        {
            First = Move(pair.First);
            Second = Move(pair.Second);

            return *this;
        }

    public:
        inline void Swap(Pair& pair)
        {
            Kitsune::Swap(First, pair.First);
            Kitsune::Swap(Second, pair.Second);
        }

    public:
        T First;
        U Second;
    };

    template<typename T1, typename U1, typename T2, typename U2>
        requires Equatable<T1, T2> && Equatable<U1, U2>
    inline bool operator==(const Pair<T1, U1>& pair1, const Pair<T2, U2>& pair2)
    {
        return (pair1.First == pair2.First) && (pair1.Second == pair2.Second);
    }
}
