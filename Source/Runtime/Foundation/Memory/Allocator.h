#pragma once

#include <concepts>

#include "Foundation/Common/Types.h"
#include "Foundation/Concepts/Comparable.h"
#include "Foundation/Concepts/Swappable.h"

namespace Kitsune
{
    // Specifies an interface to a memory resource.
    // Copies of an allocator instance is implicitly assumed to be equivalent to the
    // source object.
    template<typename T>
    concept Allocator =
        std::default_initializable<T> &&
        std::copy_constructible<T> &&
        std::assignable_from<T&, const T&> &&

        Equatable<const T, const T> &&
        Swappable<T> &&
        requires (T& alloc, void* pointer, Usize size, Usize align)
        {
            { alloc.Allocate(size) } -> std::convertible_to<void*>;
            { alloc.Allocate(size, align) } -> std::convertible_to<void*>;

            alloc.Free(pointer, size);
        };
}
