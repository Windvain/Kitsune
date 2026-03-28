#pragma once

#include <type_traits>
#include "Foundation/Common/Macros.h"

#include "Foundation/Memory/Memory.h"
#include "Foundation/Memory/Deleter.h"

#include "Foundation/Templates/Move.h"
#include "Foundation/Templates/Forward.h"

#include "Foundation/Templates/Swap.h"
#include "Foundation/Templates/Exchange.h"

namespace Kitsune
{
    // Smart pointer for managing an object that only has one owner.
    template<typename T, Deleter Del = DefaultDeleter<T>>
    class ScopedPtr
    {
    public:
        using ValueType = T;
        using PointerType = T*;

        using DeleterType = Del;

        static_assert(
            !std::is_reference_v<T>,
            "The type of the pointer must be a valid type. "
            "A reference type cannot be pointed to and therefore is not valid.");

        static_assert(
            std::is_convertible_v<T*, typename Del::ValueType*>,
            "The specified deleter cannot be used to delete an object of "
            "type T, because T* is not implicitly convertible to a pointer "
            "to the deleter's value type.");

    public:
        inline ScopedPtr()
            : m_Pointer()
        {
        }

        inline ScopedPtr(std::nullptr_t)
            : m_Pointer()
        {
        }

        inline explicit ScopedPtr(T* pointer)
            : m_Pointer(pointer)
        {
        }

        template<typename DeleterRef>
            requires std::same_as<Del, std::remove_reference_t<DeleterRef>>
        inline ScopedPtr(T* pointer, DeleterRef&& deleter)
            : m_Pointer(pointer), m_Deleter(Forward<DeleterRef>(deleter))
        {
        }

        inline ScopedPtr(ScopedPtr&& pointer)
            : m_Pointer(pointer.Release()),
              m_Deleter(Move(pointer.GetDeleter()))
        {
        }

        template<typename U, Deleter OtherDel>
            requires std::is_convertible_v<U*, T*>
        inline ScopedPtr(ScopedPtr<U, OtherDel>&& pointer)
            : m_Pointer(pointer.Release()),
              m_Deleter(Move(pointer.GetDeleter()))
        {
        }

        inline ~ScopedPtr()
        {
            if (m_Pointer)
                GetDeleter()(m_Pointer);
        }

    public:
        inline ScopedPtr& operator=(ScopedPtr&& pointer)
        {
            Reset(pointer.Release());
            GetDeleter() = Move(pointer.GetDeleter());

            return *this;
        }

        inline ScopedPtr& operator=(std::nullptr_t)
        {
            Reset();
            return *this;
        }

        template<typename U, Deleter OtherDel>
        inline ScopedPtr& operator=(ScopedPtr<U, OtherDel>&& pointer)
            requires std::is_convertible_v<U*, T*>
        {
            Reset(pointer.Release());
            GetDeleter() = Move(pointer.GetDeleter());

            return *this;
        }

    public:
        ScopedPtr(const ScopedPtr&) = delete;
        ScopedPtr& operator=(const ScopedPtr&) = delete;

    public:
        inline T& operator*() const
            requires (!std::is_void_v<T>)
        {
            return *m_Pointer;
        }

        inline T* operator->() const
        {
            return m_Pointer;
        }

        inline explicit operator bool() const
        {
            return (m_Pointer != nullptr);
        }

    public:
        [[nodiscard]]
        inline T* Release()
        {
            return Exchange(m_Pointer, nullptr);
        }

        inline void Reset(T* pointer = PointerType())
        {
            T* old = m_Pointer;
            m_Pointer = pointer;

            if (old != nullptr)
                GetDeleter()(old);
        }

        inline void Swap(ScopedPtr& other)
        {
            Kitsune::Swap(m_Pointer, other.m_Pointer);
            Kitsune::Swap(m_Deleter, other.m_Deleter);
        }

    public:
        [[nodiscard]]
        inline T* Get() const
        {
            return m_Pointer;
        }

        [[nodiscard]] inline Del& GetDeleter() { return m_Deleter; }
        [[nodiscard]] inline const Del& GetDeleter() const { return m_Deleter; }

    private:
        T* m_Pointer;
        KITSUNE_MAYBE_OVERLAPPING Del m_Deleter;
    };

    // Creates an object of type T whose lifetime is managed by the returned
    // scoped pointer.
    template<typename T, typename... Args>
    [[nodiscard]]
    inline ScopedPtr<T> MakeScoped(Args&&... args)
    {
        return ScopedPtr(Memory::New<T>(Forward<Args>(args)...));
    }

    template<typename T, Deleter Del, typename U>
    inline bool operator==(const ScopedPtr<T, Del>& pointer1, U* pointer2)
    {
        return (pointer1.Get() == pointer2);
    }

    template<typename T, Deleter Del, typename U>
    inline bool operator>=(const ScopedPtr<T, Del>& pointer1, U* pointer2)
    {
        return (pointer1.Get() >= pointer2);
    }

    template<typename T, Deleter Del, typename U>
    inline bool operator<=(const ScopedPtr<T, Del>& pointer1, U* pointer2)
    {
        return (pointer1.Get() <= pointer2);
    }

    template<typename T, Deleter Del, typename U>
    inline bool operator>(const ScopedPtr<T, Del>& pointer1, U* pointer2)
    {
        return (pointer1.Get() > pointer2);
    }

    template<typename T, Deleter Del, typename U>
    inline bool operator<(const ScopedPtr<T, Del>& pointer1, U* pointer2)
    {
        return (pointer1.Get() < pointer2);
    }

    template<typename T, Deleter Del, typename U>
    inline bool operator==(T* pointer1, const ScopedPtr<U, Del>& pointer2)
    {
        return (pointer1 == pointer2.Get());
    }

    template<typename T, Deleter Del, typename U>
    inline bool operator>=(T* pointer1, const ScopedPtr<U, Del>& pointer2)
    {
        return (pointer1 >= pointer2.Get());
    }

    template<typename T, Deleter Del, typename U>
    inline bool operator<=(T* pointer1, const ScopedPtr<U, Del>& pointer2)
    {
        return (pointer1 <= pointer2.Get());
    }

    template<typename T, Deleter Del, typename U>
    inline bool operator>(T* pointer1, const ScopedPtr<U, Del>& pointer2)
    {
        return (pointer1 > pointer2.Get());
    }

    template<typename T, Deleter Del, typename U>
    inline bool operator<(T* pointer1, const ScopedPtr<U, Del>& pointer2)
    {
        return (pointer1 < pointer2.Get());
    }

    template<typename T, Deleter Del, typename U, Deleter UDel>
    inline bool operator==(const ScopedPtr<T, Del>& pointer1,
                           const ScopedPtr<U, UDel>& pointer2)
    {
        return (pointer1.Get() == pointer2.Get());
    }

    template<typename T, Deleter Del, typename U, Deleter UDel>
    inline bool operator>=(const ScopedPtr<T, Del>& pointer1,
                           const ScopedPtr<U, UDel>& pointer2)
    {
        return (pointer1.Get() >= pointer2.Get());
    }

    template<typename T, Deleter Del, typename U, Deleter UDel>
    inline bool operator<=(const ScopedPtr<T, Del>& pointer1,
                           const ScopedPtr<U, UDel>& pointer2)
    {
        return (pointer1.Get() <= pointer2.Get());
    }

    template<typename T, Deleter Del, typename U, Deleter UDel>
    inline bool operator>(const ScopedPtr<T, Del>& pointer1,
                          const ScopedPtr<U, UDel>& pointer2)
    {
        return (pointer1.Get() > pointer2.Get());
    }

    template<typename T, Deleter Del, typename U, Deleter UDel>
    inline bool operator<(const ScopedPtr<T, Del>& pointer1,
                          const ScopedPtr<U, UDel>& pointer2)
    {
        return (pointer1.Get() < pointer2.Get());
    }

    template<typename T, Deleter Del>
    inline bool operator==(std::nullptr_t, const ScopedPtr<T, Del>& pointer)
    {
        return (pointer.Get() == nullptr);
    }

    template<typename T, Deleter Del>
    inline bool operator>=(std::nullptr_t, const ScopedPtr<T, Del>& pointer)
    {
        return !(nullptr < pointer);
    }

    template<typename T, Deleter Del>
    inline bool operator<=(std::nullptr_t, const ScopedPtr<T, Del>& pointer)
    {
        return !(nullptr > pointer);
    }

    template<typename T, Deleter Del>
    inline bool operator>(std::nullptr_t, const ScopedPtr<T, Del>& pointer)
    {
        return (static_cast<T*>(nullptr) > pointer.Get());
    }

    template<typename T, Deleter Del>
    inline bool operator<(std::nullptr_t, const ScopedPtr<T, Del>& pointer)
    {
        return (static_cast<T*>(nullptr) < pointer.Get());
    }

    template<typename T, Deleter Del>
    inline bool operator==(const ScopedPtr<T, Del>& pointer, std::nullptr_t)
    {
        return (pointer.Get() == nullptr);
    }

    template<typename T, Deleter Del>
    inline bool operator>=(const ScopedPtr<T, Del>& pointer, std::nullptr_t)
    {
        return !(pointer < nullptr);
    }

    template<typename T, Deleter Del>
    inline bool operator<=(const ScopedPtr<T, Del>& pointer, std::nullptr_t)
    {
        return !(pointer > nullptr);
    }

    template<typename T, Deleter Del>
    inline bool operator>(const ScopedPtr<T, Del>& pointer, std::nullptr_t)
    {
        return (pointer.Get() > static_cast<T*>(nullptr));
    }

    template<typename T, Deleter Del>
    inline bool operator<(const ScopedPtr<T, Del>& pointer, std::nullptr_t)
    {
        return (pointer.Get() < static_cast<T*>(nullptr));
    }
}
