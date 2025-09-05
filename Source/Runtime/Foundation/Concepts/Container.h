#pragma once

#include <concepts>
#include <type_traits>

#include "Foundation/Concepts/Swappable.h"
#include "Foundation/Concepts/Comparable.h"

namespace Kitsune
{
    template<typename T>
    concept Container =
        std::default_initializable<T> &&
        std::copy_constructible<T> &&
        std::is_copy_assignable_v<T> &&

        Equatable<const T, const T> &&
        Swappable<T> &&
        requires (T container, const T constCont)
        {
            typename T::ValueType;

            typename T::Iterator;
            typename T::ConstIterator;

            { container.GetBegin() } -> std::same_as<typename T::Iterator>;
            { constCont.GetBegin() } -> std::same_as<typename T::ConstIterator>;

            { container.GetEnd() } -> std::same_as<typename T::Iterator>;
            { constCont.GetEnd() } -> std::same_as<typename T::ConstIterator>;
        };
}
