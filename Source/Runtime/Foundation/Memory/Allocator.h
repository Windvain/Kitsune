#pragma once

#include <concepts>
#include "Foundation/Common/Types.h"

namespace Kitsune
{
    template<typename T>
    concept Allocator =
        std::default_initializable<T> &&
        std::copy_constructible<T> &&
        requires (T& alloc, void* ptr, Usize size, Usize align)
        {
            { alloc.Allocate(size) }        -> std::convertible_to<void*>;
            { alloc.Allocate(size, align) } -> std::convertible_to<void*>;

            alloc.Free(ptr);
        };
}
