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
            inline ReferenceCountBase(Int32 sharedCount = 1, Int32 weakCount = 1)
                : m_SharedCount(sharedCount), m_WeakCount(weakCount)
            {
            }

            virtual ~ReferenceCountBase() { /* ... */ }

        public:
            virtual void DeleteValue() = 0;
            virtual void DeleteReferenceCount() = 0;

        public:
            inline Int32 GetCount() const
            {
                if constexpr (Mode == ThreadSafety::NotThreadSafe)
                    return m_SharedCount;
                else
                    return Interlocked::Load(&m_SharedCount);
            }

            inline void IncrementReferenceCount()
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

            inline void IncrementWeakRefCount()
            {
                if constexpr (Mode == ThreadSafety::NotThreadSafe)
                    ++m_WeakCount;
                else
                    Interlocked::Increment(&m_WeakCount);
            }

            inline void ReleaseOwnership()
            {
                if (Decrement(&m_SharedCount) == 0)
                    DeleteValue();

                ReleaseWeakOwnership();
            }

            inline void ReleaseWeakOwnership()
            {
                if (Decrement(&m_WeakCount) == 0)
                    DeleteReferenceCount();
            }

        private:
            inline Int32 Decrement(Int32* ptr)
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
            inline TypedReferenceCount(T* ptr, const Alloc& alloc, const Del& del)
                : m_Pointer(ptr), m_Allocator(alloc), m_Deleter(del) { /* ... */ }

            inline TypedReferenceCount(T* ptr, const Alloc& alloc, Del&& del)
                : m_Pointer(ptr), m_Allocator(alloc), m_Deleter(Move(del)) { /* ... */ }

            inline TypedReferenceCount(T* ptr, Alloc&& alloc, const Del& del)
                : m_Pointer(ptr), m_Allocator(Move(alloc)), m_Deleter(del) { /* ... */ }

            inline TypedReferenceCount(T* ptr, Alloc&& alloc, Del&& del)
                : m_Pointer(ptr), m_Allocator(Move(alloc)), m_Deleter(Move(del)) { /* ... */ }

        public:
            inline void DeleteValue() { if (m_Pointer) m_Deleter(m_Pointer); }
            inline void DeleteReferenceCount()
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
        inline SharedPtr()               : m_Pointer(nullptr), m_Data(nullptr) { /* ... */ }
        inline SharedPtr(std::nullptr_t) : m_Pointer(nullptr), m_Data(nullptr) { /* ... */ }

        template<typename U>
            requires std::is_convertible_v<U*, T*>
        inline explicit SharedPtr(U* ptr)
            : m_Pointer(ptr)
        {
            AllocateInternalData(ptr, GlobalAllocator(), DefaultDeleter<U>());
        }

        template<typename U, Internal::IsDeleterOrRef Del>
            requires std::is_convertible_v<U*, T*>
        inline SharedPtr(U* ptr, Del&& del)
            : m_Pointer(ptr)
        {
            AllocateInternalData(ptr, GlobalAllocator(), Forward<Del>(del));
        }

        template<Internal::IsDeleterOrRef Del>
        inline SharedPtr(std::nullptr_t, Del&& del)
            : SharedPtr(static_cast<T*>(nullptr), Forward<Del>(del))
        {
        }

        template<typename U, Internal::IsDeleterOrRef Del, Internal::IsAllocatorOrRef Alloc>
            requires std::is_convertible_v<U*, T*>
        inline SharedPtr(U* ptr, Del&& del, Alloc&& alloc)
            : m_Pointer(ptr)
        {
            AllocateInternalData<T>(ptr, Forward<Alloc>(alloc), Forward<Del>(del));
        }

        template<Internal::IsDeleterOrRef Del, Internal::IsAllocatorOrRef Alloc>
        inline SharedPtr(std::nullptr_t, Del&& del, Alloc&& alloc)
            : SharedPtr(static_cast<T*>(nullptr), Forward<Del>(del), Forward<Alloc>(alloc))
        {
        }

        inline SharedPtr(const SharedPtr& ptr)
            : m_Pointer(ptr.m_Pointer), m_Data(ptr.m_Data)
        {
            if (m_Data != nullptr)
                m_Data->IncrementReferenceCount();
        }

        template<typename U>
            requires std::is_convertible_v<U*, T*>
        inline SharedPtr(const SharedPtr<U, Mode>& ptr)
            : m_Pointer(ptr.m_Pointer), m_Data(ptr.m_Data)
        {
            if (m_Data != nullptr)
                m_Data->IncrementReferenceCount();
        }

        inline SharedPtr(SharedPtr&& ptr)
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
        inline explicit SharedPtr(const WeakPtr<U, Mode>& ptr)
            : m_Pointer(ptr.m_Pointer), m_Data(ptr.m_Data)
        {
            if (m_Data != nullptr)
                m_Data->IncrementReferenceCount();
            else
                throw BadWeakPtrException();

        }

        template<typename U, Deleter Del>
            requires std::is_convertible_v<U*, T*>
        inline SharedPtr(ScopedPtr<U, Del>&& ptr)
            : m_Pointer(ptr.Get())
        {
            AllocateInternalData(ptr.Release(), GlobalAllocator(), Move(ptr.GetDeleter()));
        }

        inline ~SharedPtr()
        {
            if (m_Data != nullptr)
                m_Data->ReleaseOwnership();
        }

    public:
        inline SharedPtr& operator=(const SharedPtr& ptr) { return Assign(ptr); }
        inline SharedPtr& operator=(SharedPtr&& ptr) { return Assign(Move(ptr)); }

        template<typename U>
            requires std::is_convertible_v<U*, T*>
        inline SharedPtr& operator=(const SharedPtr<U, Mode>& ptr) { return Assign(ptr); }

        template<typename U>
            requires std::is_convertible_v<U*, T*>
        inline SharedPtr& operator=(SharedPtr<U, Mode>&& ptr) { return Assign(Move(ptr)); }

        template<typename U, Deleter UDel>
            requires std::is_convertible_v<U*, T*>
        inline SharedPtr& operator=(ScopedPtr<U, UDel>&& ptr)
        {
            SharedPtr(Move(ptr)).Swap(*this);
            return *this;
        }

    public:
        inline T& operator*() const { return *m_Pointer; }
        inline T* operator->() const { return m_Pointer; }

        inline explicit operator bool() const
        {
            return m_Pointer != nullptr;
        }

    public:
        [[nodiscard]] inline T* Get() const
        {
            return m_Pointer;
        }

        [[nodiscard]]
        inline Int32 GetCount() const
        {
            return (m_Data != nullptr) ? m_Data->GetCount() : 0;
        }

    public:
        inline void Swap(SharedPtr<T, Mode>& ptr)
        {
            Algorithms::Swap(m_Pointer, ptr.m_Pointer);
            Algorithms::Swap(m_Data, ptr.m_Data);
        }

    private:
        template<typename U>
        inline SharedPtr& Assign(const SharedPtr<U, Mode>& ptr)
        {
            if (m_Pointer != ptr.m_Pointer)
                SharedPtr(ptr).Swap(*this);

            return *this;
        }

        template<typename U>
        inline SharedPtr& Assign(SharedPtr<U, Mode>&& ptr)
        {
            if (m_Pointer != ptr.m_Pointer)
                SharedPtr(Move(ptr)).Swap(*this);

            return *this;
        }

        template<typename U, Internal::IsAllocatorOrRef Alloc, Internal::IsDeleterOrRef Del>
        inline void AllocateInternalData(U* ptr, Alloc&& alloc, Del&& del)
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
    [[nodiscard]] inline SharedPtr<T> MakeShared(Args&&... args)
    {
        return SharedPtr<T>(Memory::New<T>(Forward<Args>(args)...));
    }

    namespace Algorithms
    {
        template<typename T, ThreadSafety Mode>
        inline void Swap(SharedPtr<T, Mode>& ptr1, SharedPtr<T, Mode>& ptr2)
        {
            return ptr1.Swap(ptr2);
        }
    }

    template<typename T, ThreadSafety Mode, typename U, ThreadSafety UMode>
    inline bool operator==(const SharedPtr<T, Mode>& ptr1, const SharedPtr<U, UMode>& ptr2)
    {
        return (ptr1.Get() == ptr2.Get());
    }

    template<typename T, ThreadSafety Mode, typename U, ThreadSafety UMode>
    inline bool operator!=(const SharedPtr<T, Mode>& ptr1, const SharedPtr<U, UMode>& ptr2)
    {
        return (ptr1.Get() != ptr2.Get());
    }

    template<typename T, ThreadSafety Mode, typename U, ThreadSafety UMode>
    inline bool operator>=(const SharedPtr<T, Mode>& ptr1, const SharedPtr<U, UMode>& ptr2)
    {
        return (ptr1.Get() >= ptr2.Get());
    }

    template<typename T, ThreadSafety Mode, typename U, ThreadSafety UMode>
    inline bool operator<=(const SharedPtr<T, Mode>& ptr1, const SharedPtr<U, UMode>& ptr2)
    {
        return (ptr1.Get() <= ptr2.Get());
    }

    template<typename T, ThreadSafety Mode, typename U, ThreadSafety UMode>
    inline bool operator>(const SharedPtr<T, Mode>& ptr1, const SharedPtr<U, UMode>& ptr2)
    {
        return (ptr1.Get() > ptr2.Get());
    }

    template<typename T, ThreadSafety Mode, typename U, ThreadSafety UMode>
    inline bool operator<(const SharedPtr<T, Mode>& ptr1, const SharedPtr<U, UMode>& ptr2)
    {
        return (ptr1.Get() < ptr2.Get());
    }

    template<typename T, ThreadSafety Mode>
    inline bool operator==(std::nullptr_t, const SharedPtr<T, Mode>& ptr)
    {
        return (ptr.Get() == nullptr);
    }

    template<typename T, ThreadSafety Mode>
    inline bool operator!=(std::nullptr_t, const SharedPtr<T, Mode>& ptr)
    {
        return (ptr.Get() != nullptr);
    }

    template<typename T, ThreadSafety Mode>
    inline bool operator>=(std::nullptr_t, const SharedPtr<T, Mode>& ptr)
    {
        return !(nullptr < ptr);
    }

    template<typename T, ThreadSafety Mode>
    inline bool operator<=(std::nullptr_t, const SharedPtr<T, Mode>& ptr)
    {
        return !(nullptr > ptr);
    }

    template<typename T, ThreadSafety Mode>
    inline bool operator>(std::nullptr_t, const SharedPtr<T, Mode>& ptr)
    {
        return (static_cast<T*>(nullptr) > ptr.Get());
    }

    template<typename T, ThreadSafety Mode>
    inline bool operator<(std::nullptr_t, const SharedPtr<T, Mode>& ptr)
    {
        return (static_cast<T*>(nullptr) < ptr.Get());
    }

    template<typename T, ThreadSafety Mode>
    inline bool operator==(const SharedPtr<T, Mode>& ptr, std::nullptr_t)
    {
        return (ptr.Get() == nullptr);
    }

    template<typename T, ThreadSafety Mode>
    inline bool operator!=(const SharedPtr<T, Mode>& ptr, std::nullptr_t)
    {
        return (ptr.Get() != nullptr);
    }

    template<typename T, ThreadSafety Mode>
    inline bool operator>=(const SharedPtr<T, Mode>& ptr, std::nullptr_t)
    {
        return !(ptr < nullptr);
    }

    template<typename T, ThreadSafety Mode>
    inline bool operator<=(const SharedPtr<T, Mode>& ptr, std::nullptr_t)
    {
        return !(ptr > nullptr);
    }

    template<typename T, ThreadSafety Mode>
    inline bool operator>(const SharedPtr<T, Mode>& ptr, std::nullptr_t)
    {
        return (ptr.Get() > static_cast<T*>(nullptr));
    }

    template<typename T, ThreadSafety Mode>
    inline bool operator<(const SharedPtr<T, Mode>& ptr, std::nullptr_t)
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
        inline WeakPtr() : m_Pointer(nullptr), m_Data(nullptr) { /* ... */ }

        template<typename U>
        inline WeakPtr(const SharedPtr<U, Mode>& ptr)
            : m_Pointer(ptr.m_Pointer), m_Data(ptr.m_Data)
        {
            if (m_Data != nullptr)
                m_Data->IncrementWeakRefCount();
        }

        inline WeakPtr(const WeakPtr& ptr)
            : m_Pointer(ptr.m_Pointer), m_Data(ptr.m_Data)
        {
            if (m_Data != nullptr)
                m_Data->IncrementWeakRefCount();
        }

        template<typename U>
        inline WeakPtr(const WeakPtr<U, Mode>& ptr)
            : m_Pointer(ptr.m_Pointer), m_Data(ptr.m_Data)
        {
            if (m_Data != nullptr)
                m_Data->IncrementWeakRefCount();
        }

        inline WeakPtr(WeakPtr&& ptr)
            : m_Pointer(ptr.m_Pointer), m_Data(Exchange(ptr.m_Data, nullptr)) { /* ... */ }

        template<typename U>
        inline WeakPtr(WeakPtr<U, Mode>&& ptr)
            : m_Pointer(ptr.m_Pointer), m_Data(Exchange(ptr.m_Data, nullptr)) { /* ... */ }

        inline ~WeakPtr()
        {
            if (m_Data != nullptr)
                m_Data->ReleaseWeakOwnership();
        }

    public:
        inline WeakPtr& operator=(const WeakPtr& ptr) { return Assign(ptr); }
        inline WeakPtr& operator=(WeakPtr&& ptr) { return Assign(Move(ptr)); }

        template<typename U>
        inline WeakPtr& operator=(const WeakPtr<U, Mode>& ptr) { return Assign(ptr); }

        template<typename U>
        inline WeakPtr& operator=(WeakPtr<U, Mode>&& ptr) { return Assign(Move(ptr)); }

        template<typename U>
        inline WeakPtr& operator=(const SharedPtr<U>& ptr)
        {
            if (m_Pointer != ptr.m_Pointer)
                WeakPtr(ptr).Swap(*this);

            return *this;
        }

    public:
        inline void Reset()
        {
            if (m_Data != nullptr)
                m_Data->ReleaseWeakOwnership();

            m_Data = nullptr;
        }

        inline void Swap(WeakPtr<T, Mode>& ptr)
        {
            Algorithms::Swap(m_Pointer, ptr.m_Pointer);
            Algorithms::Swap(m_Data, ptr.m_Data);
        }

    public:
        inline Int32 GetCount() const { return (m_Data != nullptr) ? m_Data->GetCount() : 0; }
        inline bool IsExpired() const { return (GetCount() == 0); }

        inline SharedPtr<T> Lock() const { return IsExpired() ? SharedPtr<T>() : SharedPtr<T>(*this); }

    private:
        template<typename U>
        inline WeakPtr& Assign(const WeakPtr<U, Mode>& ptr)
        {
            if (m_Pointer != ptr.m_Pointer)
                WeakPtr(ptr).Swap(*this);

            return *this;
        }

        template<typename U>
        inline WeakPtr& Assign(WeakPtr<U, Mode>&& ptr)
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

    namespace Algorithms
    {
        template<typename T, ThreadSafety Mode>
        inline void Swap(WeakPtr<T, Mode>& ptr1, WeakPtr<T, Mode>& ptr2)
        {
            return ptr1.Swap(ptr2);
        }
    }
}
