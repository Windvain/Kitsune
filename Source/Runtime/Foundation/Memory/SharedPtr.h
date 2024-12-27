#pragma once

#include "Foundation/Common/Types.h"
#include "Foundation/Common/Macros.h"

#include "Foundation/Memory/Deleter.h"
#include "Foundation/Memory/Allocator.h"
#include "Foundation/Memory/ScopedPtr.h"
#include "Foundation/Memory/GlobalAllocator.h"

#include "Foundation/Algorithms/Swap.h"
#include "Foundation/Templates/Exchange.h"

#include "Foundation/Threading/Interlocked.h"
#include "Foundation/Threading/ThreadSafety.h"

#include "Foundation/Memory/BadWeakPtrException.h"

namespace Kitsune
{
    namespace Internal
    {
        template<ThreadSafety Mode>
        class ReferenceCountBase
        {
        public:
            ReferenceCountBase(Int32 sharedCount = 1, Int32 weakCount = 1)
                : m_SharedCount(sharedCount), m_WeakCount(weakCount)
            {
            }

            virtual ~ReferenceCountBase() { /* ... */ }

        public:
            virtual void DeleteValue() = 0;
            virtual void DeleteReferenceCount() = 0;

        public:
            Int32 GetCount() const
            {
                if constexpr (Mode == ThreadSafety::NotThreadSafe)
                    return m_SharedCount;
                else
                    return Interlocked::Load(&m_SharedCount);
            }

            void IncrementReferenceCount()
            {
                if constexpr (Mode == ThreadSafety::NotThreadSafe)
                {
                    ++m_SharedCount;
                    ++m_WeakCount;
                }
                else
                {
                    Interlocked::Increment(&m_SharedCount);
                    Interlocked::Increment(&m_WeakCount);
                }
            }

            void IncrementWeakRefCount()
            {
                if constexpr (Mode == ThreadSafety::NotThreadSafe)
                    ++m_WeakCount;
                else
                    Interlocked::Increment(&m_WeakCount);
            }

            void ReleaseOwnership()
            {
                if (Decrement(&m_SharedCount) == 0)
                    DeleteValue();

                ReleaseWeakOwnership();
            }

            void ReleaseWeakOwnership()
            {
                if (Decrement(&m_WeakCount) == 0)
                    DeleteReferenceCount();
            }

        private:
            Int32 Decrement(Int32* ptr)
            {
                if constexpr (Mode == ThreadSafety::NotThreadSafe)
                    return (--*ptr);
                else
                    return Interlocked::Decrement(ptr);
            }

        protected:
            Int32 m_SharedCount;
            Int32 m_WeakCount;
        };

        template<typename T, ThreadSafety Mode, Allocator Alloc, Deleter Del>
        class TypedReferenceCount : public ReferenceCountBase<Mode>
        {
        public:
            TypedReferenceCount(T* ptr, const Alloc& alloc, const Del& del)
                : m_Pointer(ptr), m_Allocator(alloc), m_Deleter(del) { /* ... */ }

            TypedReferenceCount(T* ptr, const Alloc& alloc, Del&& del)
                : m_Pointer(ptr), m_Allocator(alloc), m_Deleter(Move(del)) { /* ... */ }

            TypedReferenceCount(T* ptr, Alloc&& alloc, const Del& del)
                : m_Pointer(ptr), m_Allocator(Move(alloc)), m_Deleter(del) { /* ... */ }

            TypedReferenceCount(T* ptr, Alloc&& alloc, Del&& del)
                : m_Pointer(ptr), m_Allocator(Move(alloc)), m_Deleter(Move(del)) { /* ... */ }

        public:
            void DeleteValue() { if (m_Pointer) m_Deleter(m_Pointer); }
            void DeleteReferenceCount()
            {
                Alloc alloc = Move(m_Allocator);

                this->~TypedReferenceCount();
                alloc.Free(this);
            }

        private:
            T* m_Pointer;

            KITSUNE_MAYBE_OVERLAPPING Alloc m_Allocator;
            KITSUNE_MAYBE_OVERLAPPING Del m_Deleter;
        };

        template<typename T>
        concept IsDeleterOrRef = Deleter<std::remove_reference_t<T>>;

        template<typename T>
        concept IsAllocatorOrRef = Allocator<std::remove_reference_t<T>>;
    }

    // Forward-declare..
    template<typename T, ThreadSafety Mode> class WeakPtr;

    template<typename T, ThreadSafety Mode = ThreadSafety::ThreadSafe>
    class SharedPtr
    {
    public:
        using ValueType = T;
        using PointerType = T*;

        using WeakType = WeakPtr<T, Mode>;

    public:
        SharedPtr()               : m_Pointer(nullptr), m_Data(nullptr) { /* ... */ }
        SharedPtr(std::nullptr_t) : m_Pointer(nullptr), m_Data(nullptr) { /* ... */ }

        template<typename U>
            requires std::is_convertible_v<U*, T*>
        explicit SharedPtr(U* ptr)
            : m_Pointer(ptr)
        {
            AllocateInternalData(ptr, GlobalAllocator(), DefaultDeleter<U>());
        }

        template<typename U, Internal::IsDeleterOrRef Del>
            requires std::is_convertible_v<U*, T*>
        SharedPtr(U* ptr, Del&& del)
            : m_Pointer(ptr)
        {
            AllocateInternalData(ptr, GlobalAllocator(), Forward<Del>(del));
        }

        template<Internal::IsDeleterOrRef Del>
        SharedPtr(std::nullptr_t, Del&& del)
            : SharedPtr(static_cast<T*>(nullptr), Forward<Del>(del))
        {
        }

        template<typename U, Internal::IsDeleterOrRef Del, Internal::IsAllocatorOrRef Alloc>
            requires std::is_convertible_v<U*, T*>
        SharedPtr(U* ptr, Del&& del, Alloc&& alloc)
            : m_Pointer(ptr)
        {
            AllocateInternalData<T>(ptr, Forward<Alloc>(alloc), Forward<Del>(del));
        }

        template<Internal::IsDeleterOrRef Del, Internal::IsAllocatorOrRef Alloc>
        SharedPtr(std::nullptr_t, Del&& del, Alloc&& alloc)
            : SharedPtr(static_cast<T*>(nullptr), Forward<Del>(del), Forward<Alloc>(alloc))
        {
        }

        SharedPtr(const SharedPtr& ptr)
            : m_Pointer(ptr.m_Pointer), m_Data(ptr.m_Data)
        {
            if (m_Data != nullptr)
                m_Data->IncrementReferenceCount();
        }

        template<typename U>
            requires std::is_convertible_v<U*, T*>
        SharedPtr(const SharedPtr<U, Mode>& ptr)
            : m_Pointer(ptr.m_Pointer), m_Data(ptr.m_Data)
        {
            if (m_Data != nullptr)
                m_Data->IncrementReferenceCount();
        }

        SharedPtr(SharedPtr&& ptr)
            : m_Pointer(ptr.m_Pointer), m_Data(Exchange(ptr.m_Data, nullptr))
        {
        }

        template<typename U>
            requires std::is_convertible_v<U*, T*>
        SharedPtr(SharedPtr<U, Mode>&& ptr)
            : m_Pointer(ptr.m_Pointer), m_Data(Exchange(ptr.m_Data, nullptr))
        {
        }

        template<typename U>
            requires std::is_convertible_v<U*, T*>
        explicit SharedPtr(const WeakPtr<U, Mode>& ptr)
            : m_Pointer(ptr.m_Pointer), m_Data(ptr.m_Data)
        {
            if (m_Data != nullptr)
                m_Data->IncrementReferenceCount();
            else
                throw BadWeakPtrException();

        }

        template<typename U, Deleter Del>
            requires std::is_convertible_v<U*, T*>
        SharedPtr(ScopedPtr<U, Del>&& ptr)
            : m_Pointer(ptr.Get())
        {
            AllocateInternalData(ptr.Release(), GlobalAllocator(), Move(ptr.GetDeleter()));
        }

        ~SharedPtr()
        {
            if (m_Data != nullptr)
                m_Data->ReleaseOwnership();
        }

    public:
        SharedPtr& operator=(const SharedPtr& ptr) { return Assign(ptr); }
        SharedPtr& operator=(SharedPtr&& ptr) { return Assign(Move(ptr)); }

        template<typename U>
            requires std::is_convertible_v<U*, T*>
        SharedPtr& operator=(const SharedPtr<U, Mode>& ptr) { return Assign(ptr); }

        template<typename U>
            requires std::is_convertible_v<U*, T*>
        SharedPtr& operator=(SharedPtr<U, Mode>&& ptr) { return Assign(Move(ptr)); }

        template<typename U, Deleter UDel>
            requires std::is_convertible_v<U*, T*>
        SharedPtr& operator=(ScopedPtr<U, UDel>&& ptr)
        {
            SharedPtr(Move(ptr)).Swap(*this);
            return *this;
        }

    public:
        T& operator*() const { return *m_Pointer; }
        T* operator->() const { return m_Pointer; }

        explicit operator bool() const { return m_Pointer != nullptr; }

    public:
        [[nodiscard]] T* Get() const { return m_Pointer; }

        [[nodiscard]]
        Int32 GetCount() const
        {
            return (m_Data != nullptr) ? m_Data->GetCount() : 0;
        }

    private:
        void Swap(SharedPtr<T, Mode>& ptr)
        {
            Algorithms::Swap(m_Pointer, ptr.m_Pointer);
            Algorithms::Swap(m_Data, ptr.m_Data);
        }

        template<typename U>
        SharedPtr& Assign(const SharedPtr<U, Mode>& ptr)
        {
            if (m_Pointer != ptr.m_Pointer)
                SharedPtr(ptr).Swap(*this);

            return *this;
        }

        template<typename U>
        SharedPtr& Assign(SharedPtr<U, Mode>&& ptr)
        {
            if (m_Pointer != ptr.m_Pointer)
                SharedPtr(Move(ptr)).Swap(*this);

            return *this;
        }

        template<typename U, Internal::IsAllocatorOrRef Alloc, Internal::IsDeleterOrRef Del>
        void AllocateInternalData(U* ptr, Alloc&& alloc, Del&& del)
        {
            using PureAlloc = std::remove_reference_t<Alloc>;
            using PureDel = std::remove_reference_t<Del>;

            using InternalData = Internal::TypedReferenceCount<U, Mode, PureAlloc, PureDel>;

            try
            {
                InternalData* cnt = static_cast<InternalData*>(alloc.Allocate(sizeof(InternalData)));
                m_Data = Memory::ConstructAt(cnt, ptr, Forward<Alloc>(alloc), Forward<Del>(del));
            }
            catch (const IException&)
            {
                if (ptr) del(ptr);
                throw;
            }
        }

    private:
        template<typename U, ThreadSafety UMode> friend class SharedPtr;
        template<typename U, ThreadSafety UMode> friend class WeakPtr;

    private:
        T* m_Pointer;
        Internal::ReferenceCountBase<Mode>* m_Data;
    };

    template<typename T, typename... Args>
    [[nodiscard]] SharedPtr<T> MakeShared(Args&&... args)
    {
        return SharedPtr<T>(Memory::New<T>(Forward<Args>(args)...));
    }

    template<typename T, ThreadSafety Mode, typename U, ThreadSafety UMode>
    bool operator==(const SharedPtr<T, Mode>& ptr1, const SharedPtr<U, UMode>& ptr2)
    {
        return (ptr1.Get() == ptr2.Get());
    }

    template<typename T, ThreadSafety Mode, typename U, ThreadSafety UMode>
    bool operator!=(const SharedPtr<T, Mode>& ptr1, const SharedPtr<U, UMode>& ptr2)
    {
        return (ptr1.Get() != ptr2.Get());
    }

    template<typename T, ThreadSafety Mode, typename U, ThreadSafety UMode>
    bool operator>=(const SharedPtr<T, Mode>& ptr1, const SharedPtr<U, UMode>& ptr2)
    {
        return (ptr1.Get() >= ptr2.Get());
    }

    template<typename T, ThreadSafety Mode, typename U, ThreadSafety UMode>
    bool operator<=(const SharedPtr<T, Mode>& ptr1, const SharedPtr<U, UMode>& ptr2)
    {
        return (ptr1.Get() <= ptr2.Get());
    }

    template<typename T, ThreadSafety Mode, typename U, ThreadSafety UMode>
    bool operator>(const SharedPtr<T, Mode>& ptr1, const SharedPtr<U, UMode>& ptr2)
    {
        return (ptr1.Get() > ptr2.Get());
    }

    template<typename T, ThreadSafety Mode, typename U, ThreadSafety UMode>
    bool operator<(const SharedPtr<T, Mode>& ptr1, const SharedPtr<U, UMode>& ptr2)
    {
        return (ptr1.Get() < ptr2.Get());
    }

    template<typename T, ThreadSafety Mode>
    bool operator==(std::nullptr_t, const SharedPtr<T, Mode>& ptr)
    {
        return (ptr.Get() == nullptr);
    }

    template<typename T, ThreadSafety Mode>
    bool operator!=(std::nullptr_t, const SharedPtr<T, Mode>& ptr)
    {
        return (ptr.Get() != nullptr);
    }

    template<typename T, ThreadSafety Mode>
    bool operator>=(std::nullptr_t, const SharedPtr<T, Mode>& ptr)
    {
        return !(nullptr < ptr);
    }

    template<typename T, ThreadSafety Mode>
    bool operator<=(std::nullptr_t, const SharedPtr<T, Mode>& ptr)
    {
        return !(nullptr > ptr);
    }

    template<typename T, ThreadSafety Mode>
    bool operator>(std::nullptr_t, const SharedPtr<T, Mode>& ptr)
    {
        return (static_cast<T*>(nullptr) > ptr.Get());
    }

    template<typename T, ThreadSafety Mode>
    bool operator<(std::nullptr_t, const SharedPtr<T, Mode>& ptr)
    {
        return (static_cast<T*>(nullptr) < ptr.Get());
    }

    template<typename T, ThreadSafety Mode>
    bool operator==(const SharedPtr<T, Mode>& ptr, std::nullptr_t)
    {
        return (ptr.Get() == nullptr);
    }

    template<typename T, ThreadSafety Mode>
    bool operator!=(const SharedPtr<T, Mode>& ptr, std::nullptr_t)
    {
        return (ptr.Get() != nullptr);
    }

    template<typename T, ThreadSafety Mode>
    bool operator>=(const SharedPtr<T, Mode>& ptr, std::nullptr_t)
    {
        return !(ptr < nullptr);
    }

    template<typename T, ThreadSafety Mode>
    bool operator<=(const SharedPtr<T, Mode>& ptr, std::nullptr_t)
    {
        return !(ptr > nullptr);
    }

    template<typename T, ThreadSafety Mode>
    bool operator>(const SharedPtr<T, Mode>& ptr, std::nullptr_t)
    {
        return (ptr.Get() > static_cast<T*>(nullptr));
    }

    template<typename T, ThreadSafety Mode>
    bool operator<(const SharedPtr<T, Mode>& ptr, std::nullptr_t)
    {
        return (ptr.Get() < static_cast<T*>(nullptr));
    }

    template<typename T, ThreadSafety Mode = ThreadSafety::ThreadSafe>
    class WeakPtr
    {
    public:
        using ValueType = T;
        using PointerType = T*;

    public:
        WeakPtr() : m_Pointer(nullptr), m_Data(nullptr) { /* ... */ }

        template<typename U>
        WeakPtr(const SharedPtr<U, Mode>& ptr)
            : m_Pointer(ptr.m_Pointer), m_Data(ptr.m_Data)
        {
            if (m_Data != nullptr)
                m_Data->IncrementWeakRefCount();
        }

        WeakPtr(const WeakPtr& ptr)
            : m_Pointer(ptr.m_Pointer), m_Data(ptr.m_Data)
        {
            if (m_Data != nullptr)
                m_Data->IncrementWeakRefCount();
        }

        template<typename U>
        WeakPtr(const WeakPtr<U, Mode>& ptr)
            : m_Pointer(ptr.m_Pointer), m_Data(ptr.m_Data)
        {
            if (m_Data != nullptr)
                m_Data->IncrementWeakRefCount();
        }

        WeakPtr(WeakPtr&& ptr)
            : m_Pointer(ptr.m_Pointer), m_Data(Exchange(ptr.m_Data, nullptr)) { /* ... */ }

        template<typename U>
        WeakPtr(WeakPtr<U, Mode>&& ptr)
            : m_Pointer(ptr.m_Pointer), m_Data(Exchange(ptr.m_Data, nullptr)) { /* ... */ }

        ~WeakPtr()
        {
            if (m_Data != nullptr)
                m_Data->ReleaseWeakOwnership();
        }

    public:
        WeakPtr& operator=(const WeakPtr& ptr) { return Assign(ptr); }
        WeakPtr& operator=(WeakPtr&& ptr) { return Assign(Move(ptr)); }

        template<typename U>
        WeakPtr& operator=(const WeakPtr<U, Mode>& ptr) { return Assign(ptr); }

        template<typename U>
        WeakPtr& operator=(WeakPtr<U, Mode>&& ptr) { return Assign(Move(ptr)); }

        template<typename U>
        WeakPtr& operator=(const SharedPtr<U>& ptr)
        {
            if (m_Pointer != ptr.m_Pointer)
                WeakPtr(ptr).Swap(*this);

            return *this;
        }

    public:
        void Reset()
        {
            if (m_Data != nullptr)
                m_Data->ReleaseWeakOwnership();

            m_Data = nullptr;
        }

    public:
        Int32 GetCount() const { return (m_Data != nullptr) ? m_Data->GetCount() : 0; }
        bool IsExpired() const { return (GetCount() == 0); }

        SharedPtr<T> Lock() const { return IsExpired() ? SharedPtr<T>() : SharedPtr<T>(*this); }

    private:
        void Swap(WeakPtr<T, Mode>& ptr)
        {
            Algorithms::Swap(m_Pointer, ptr.m_Pointer);
            Algorithms::Swap(m_Data, ptr.m_Data);
        }

        template<typename U>
        WeakPtr& Assign(const WeakPtr<U, Mode>& ptr)
        {
            if (m_Pointer != ptr.m_Pointer)
                WeakPtr(ptr).Swap(*this);

            return *this;
        }

        template<typename U>
        WeakPtr& Assign(WeakPtr<U, Mode>&& ptr)
        {
            if (m_Pointer != ptr.m_Pointer)
                WeakPtr(Move(ptr)).Swap(*this);

            return *this;
        }

    private:
        template<typename U, ThreadSafety UMode> friend class WeakPtr;
        template<typename U, ThreadSafety UMode> friend class SharedPtr;

    private:
        T* m_Pointer;
        Internal::ReferenceCountBase<Mode>* m_Data;
    };
}
