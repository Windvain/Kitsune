#pragma once

#include <concepts>
#include "Foundation/Templates/Move.h"

namespace Kitsune::Algorithms::Internal
{
    template<typename It, typename Pred>
    concept IsPredicateSingular = requires (It it, Pred pred)
    {
        { pred(*it) } -> std::convertible_to<bool>;
    };

    template<typename It1, typename It2, typename Pred>
    concept IsPredicateBinary = requires (It1 it1, It2 it2, Pred pred)
    {
        { pred(*it1, *it2) } -> std::convertible_to<bool>;
    };

    template<typename It, typename Fn>
    concept IsInvocableWithIterator = requires (It it, Fn fn)
    {
        fn(*it);
    };

    template<typename It1, typename It2>
    concept IsIterIterComparable = requires (It1 it1, It2 it2) { *it1 == *it2; };

    template<typename It, typename Val>
    concept IsIterValueComparable = requires (It it, Val val) { *it == val; };

    template<typename LIt, typename RIt>
    concept IsIterCopyAssignable = requires (LIt lit, RIt rit) { *lit = *rit; };

    template<typename LIt, typename RIt>
    concept IsIterMoveAssignable = requires (LIt lit, RIt rit)
    {
        *lit = Move(*rit);
    };
}
