#pragma once

#include <concepts>

#include "Foundation/Common/Types.h"
#include "Foundation/Concepts/Comparable.h"

namespace Kitsune
{
    // Specifies an interface to a memory resource.
    template<typename T>
    concept Allocator =
        std::default_initializable<T> &&
        std::copy_constructible<T> &&

        Equatable<const T, const T> &&
        requires (T& alloc, void* pointer, Usize size, Usize align)
        {
            { alloc.Allocate(size) } -> std::convertible_to<void*>;
            { alloc.Allocate(size, align) } -> std::convertible_to<void*>;

            alloc.Free(pointer, size);
        };
}
