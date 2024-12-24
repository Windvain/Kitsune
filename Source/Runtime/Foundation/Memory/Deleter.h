#pragma once

#include <concepts>
#include "Foundation/Memory/Memory.h"

namespace Kitsune
{
    template<typename T>
    concept Deleter =
        std::default_initializable<T> &&
        std::copy_constructible<T> &&

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
        DefaultDeleter(const DefaultDeleter<U>&)
            requires (std::is_convertible_v<U*, T*>)
        {
        }

    public:
        DefaultDeleter& operator=(const DefaultDeleter&) = default;

    public:
        void operator()(ValueType* ptr)
        {
            Memory::Delete(ptr);
        }
    };
}
