#pragma once

#include <concepts>
#include "Foundation/Memory/Memory.h"

namespace Kitsune
{
    template<typename T>
    concept Deleter =
        std::default_initializable<T> &&
        std::copy_constructible<T> &&

        std::is_copy_assignable_v<T> &&
        std::is_move_assignable_v<T> &&

        requires (T deleter)
        {
            typename T::ValueType;
            deleter(std::declval<typename T::ValueType*>());
        };

    template<typename T>
    class DefaultDeleter
    {
    public:
        using ValueType = T;

        DefaultDeleter() = default;
        DefaultDeleter(const DefaultDeleter&) = default;

        template<typename U>
        inline DefaultDeleter(const DefaultDeleter<U>&)
            requires (std::is_convertible_v<U*, T*>)
        {
        }

    public:
        DefaultDeleter& operator=(const DefaultDeleter&) = default;

    public:
        inline void operator()(ValueType* pointer)
        {
            if constexpr (!std::is_void_v<ValueType>)
                Memory::Delete(pointer);

            KITSUNE_UNUSED(pointer);
        }
    };
}
