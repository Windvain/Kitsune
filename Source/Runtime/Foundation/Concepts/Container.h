#pragma once

#include <concepts>
#include "Foundation/Common/Types.h"

#include "Foundation/Concepts/Swappable.h"
#include "Foundation/Concepts/Comparable.h"

namespace Kitsune
{
    // The concept `Container<T>` specifies that the type `T`
    // is used to store other objects. It owns the objects that
    // it stores and manages the lifetime of the memory.
    template<typename T>
    concept Container =
        std::default_initializable<T> &&
        std::copyable<T> &&

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

            { constCont.Size() } -> std::same_as<Usize>;
            { constCont.IsEmpty() } -> std::same_as<bool>;
        };
}
