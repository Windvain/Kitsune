#pragma once

#include <concepts>
#include "Foundation/Memory/Memory.h"

namespace Kitsune
{
    // Specifies a state(less) type which can be used to delete an object of type
    // `T::ValueType`.
    template<typename T>
    concept Deleter =
        std::default_initializable<T> &&
        std::copyable<T> &&

        requires (T deleter)
        {
            typename T::ValueType;
            deleter(std::declval<typename T::ValueType*>());
        };

    // The default deleter used for object management in smart pointers, if none
    // were specified.
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
            Memory::Delete(pointer);
        }
    };
}
