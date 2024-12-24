#pragma once

#include <type_traits>

#include "Foundation/Memory/Memory.h"
#include "Foundation/Memory/Deleter.h"

#include "Foundation/Templates/Move.h"
#include "Foundation/Templates/Forward.h"

#include "Foundation/Templates/Exchange.h"

namespace Kitsune
{
    namespace Internal
    {
        template<typename Del, typename T>
        concept IsValidDeleter = requires (Del del, T* ptr) { del(ptr); };
    }

    template<typename T, Deleter Del = DefaultDeleter<T>>
    class ScopedPtr
    {
    public:
        using ValueType = T;
        using PointerType = T*;

        using DeleterType = Del;

        static_assert(Internal::IsValidDeleter<Del, T>, "Invalid deleter used in ScopedPtr<T>.");

    public:
        ScopedPtr() : m_Pointer() { /* ... */ }
        ScopedPtr(std::nullptr_t) : m_Pointer() { /* ... */ }

        explicit ScopedPtr(T* ptr) : m_Pointer(ptr) { /* ... */ }
        ScopedPtr(T* ptr, const Del& del)
            : m_Pointer(ptr), m_Deleter(del)
        {
        }

        ScopedPtr(T* ptr, Del&& del)
            : m_Pointer(ptr), m_Deleter(Move(del))
        {
        }

        ScopedPtr(ScopedPtr&& ptr)
            : m_Pointer(ptr.Release()),
              m_Deleter(Move(ptr.GetDeleter()))
        {
        }

        template<typename U, Deleter UDel>
            requires std::is_convertible_v<U*, T*>
        ScopedPtr(ScopedPtr<U, UDel>&& ptr)
            : m_Pointer(ptr.Release()),
              m_Deleter(Move(ptr.GetDeleter()))
        {
        }

        ~ScopedPtr()
        {
            if (m_Pointer)
                GetDeleter()(m_Pointer);
        }

    public:
        ScopedPtr& operator=(ScopedPtr&& ptr)
        {
            Reset(ptr.Release());
            GetDeleter() = Move(ptr.GetDeleter());

            return *this;
        }

        ScopedPtr& operator=(std::nullptr_t) { Reset(); return *this; }

        template<typename U, Deleter UDel>
        ScopedPtr& operator=(ScopedPtr<U, UDel>&& ptr)
            requires std::is_convertible_v<U*, T*>
        {
            Reset(ptr.Release());
            GetDeleter() = Move(ptr.GetDeleter());

            return *this;
        }

    public:
        ScopedPtr(const ScopedPtr&) = delete;
        ScopedPtr& operator=(const ScopedPtr&) = delete;

    public:
        T& operator*() const requires (!std::is_void_v<T>) { return *m_Pointer; }
        T* operator->() const { return m_Pointer; }

        explicit operator bool() const { return (m_Pointer != nullptr); }

    public:
        [[nodiscard]]
        T* Release() { return Exchange(m_Pointer, nullptr); }

        void Reset(T* ptr = PointerType())
        {
            T* old = m_Pointer;
            m_Pointer = ptr;

            if (old) GetDeleter()(old);
        }

    public:
        [[nodiscard]]
        T* Get() const { return m_Pointer; }

        [[nodiscard]] Del& GetDeleter() { return m_Deleter; }
        [[nodiscard]] const Del& GetDeleter() const { return m_Deleter; }

    private:
        T* m_Pointer;
        KITSUNE_MAYBE_OVERLAPPING Del m_Deleter;
    };

    template<typename T, typename... Args>
    [[nodiscard]] ScopedPtr<T> MakeScoped(Args&&... args)
    {
        return ScopedPtr(Memory::New<T>(Forward<Args>(args)...));
    }

    template<typename T, Deleter Del, typename U, Deleter UDel>
    bool operator==(const ScopedPtr<T, Del>& ptr1, const ScopedPtr<U, UDel>& ptr2)
    {
        return (ptr1.Get() == ptr2.Get());
    }

    template<typename T, Deleter Del, typename U, Deleter UDel>
    bool operator!=(const ScopedPtr<T, Del>& ptr1, const ScopedPtr<U, UDel>& ptr2)
    {
        return (ptr1.Get() != ptr2.Get());
    }

    template<typename T, Deleter Del, typename U, Deleter UDel>
    bool operator>=(const ScopedPtr<T, Del>& ptr1, const ScopedPtr<U, UDel>& ptr2)
    {
        return (ptr1.Get() >= ptr2.Get());
    }

    template<typename T, Deleter Del, typename U, Deleter UDel>
    bool operator<=(const ScopedPtr<T, Del>& ptr1, const ScopedPtr<U, UDel>& ptr2)
    {
        return (ptr1.Get() <= ptr2.Get());
    }

    template<typename T, Deleter Del, typename U, Deleter UDel>
    bool operator>(const ScopedPtr<T, Del>& ptr1, const ScopedPtr<U, UDel>& ptr2)
    {
        return (ptr1.Get() > ptr2.Get());
    }

    template<typename T, Deleter Del, typename U, Deleter UDel>
    bool operator<(const ScopedPtr<T, Del>& ptr1, const ScopedPtr<U, UDel>& ptr2)
    {
        return (ptr1.Get() < ptr2.Get());
    }

    template<typename T, Deleter Del>
    bool operator==(std::nullptr_t, const ScopedPtr<T, Del>& ptr)
    {
        return (ptr.Get() == nullptr);
    }

    template<typename T, Deleter Del>
    bool operator!=(std::nullptr_t, const ScopedPtr<T, Del>& ptr)
    {
        return (ptr.Get() != nullptr);
    }

    template<typename T, Deleter Del>
    bool operator>=(std::nullptr_t, const ScopedPtr<T, Del>& ptr)
    {
        return !(nullptr < ptr);
    }

    template<typename T, Deleter Del>
    bool operator<=(std::nullptr_t, const ScopedPtr<T, Del>& ptr)
    {
        return !(nullptr > ptr);
    }

    template<typename T, Deleter Del>
    bool operator>(std::nullptr_t, const ScopedPtr<T, Del>& ptr)
    {
        return (static_cast<T*>(nullptr) > ptr.Get());
    }

    template<typename T, Deleter Del>
    bool operator<(std::nullptr_t, const ScopedPtr<T, Del>& ptr)
    {
        return (static_cast<T*>(nullptr) < ptr.Get());
    }

    template<typename T, Deleter Del>
    bool operator==(const ScopedPtr<T, Del>& ptr, std::nullptr_t)
    {
        return (ptr.Get() == nullptr);
    }

    template<typename T, Deleter Del>
    bool operator!=(const ScopedPtr<T, Del>& ptr, std::nullptr_t)
    {
        return (ptr.Get() != nullptr);
    }

    template<typename T, Deleter Del>
    bool operator>=(const ScopedPtr<T, Del>& ptr, std::nullptr_t)
    {
        return !(ptr < nullptr);
    }

    template<typename T, Deleter Del>
    bool operator<=(const ScopedPtr<T, Del>& ptr, std::nullptr_t)
    {
        return !(ptr > nullptr);
    }

    template<typename T, Deleter Del>
    bool operator>(const ScopedPtr<T, Del>& ptr, std::nullptr_t)
    {
        return (ptr.Get() > static_cast<T*>(nullptr));
    }

    template<typename T, Deleter Del>
    bool operator<(const ScopedPtr<T, Del>& ptr, std::nullptr_t)
    {
        return (ptr.Get() < static_cast<T*>(nullptr));
    }
}
