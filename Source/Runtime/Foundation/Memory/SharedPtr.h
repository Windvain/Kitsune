#pragma once

#include "Foundation/Memory/Allocator.h"

#include "Foundation/Memory/ScopedPtr.h"
#include "Foundation/Memory/GlobalAllocator.h"

#include "Foundation/Threading/Interlocked.h"
#include "Foundation/Threading/ThreadSafety.h"

#include "Foundation/Memory/BadWeakPtrException.h"

namespace Kitsune
{
    namespace Details
    {
        template<ThreadSafety Mode>
        class ReferenceCountBase_
        {
        public:
            inline ReferenceCountBase_(Int32 sharedCount = 1, Int32 weakCount = 1)
                : m_SharedCount(sharedCount), m_WeakCount(weakCount)
            {
            }

            virtual ~ReferenceCountBase_() { /* ... */ }

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
                if (DecrementValue_(&m_SharedCount) == 0)
                    DeleteValue();

                ReleaseWeakOwnership();
            }

            inline void ReleaseWeakOwnership()
            {
                if (DecrementValue_(&m_WeakCount) == 0)
                    DeleteReferenceCount();
            }

        private:
            inline static Int32 DecrementValue_(Int32* pointer)
            {
                if constexpr (Mode == ThreadSafety::NotThreadSafe)
                    return (--*pointer);
                else
                    return Interlocked::Decrement(pointer);
            }

        protected:
            Int32 m_SharedCount;
            Int32 m_WeakCount;
        };

        template<typename T, ThreadSafety Mode, Deleter Del, Allocator Alloc>
        class TypedReferenceCount_ : public ReferenceCountBase_<Mode>
        {
        public:
            template<typename DelRef>
            inline TypedReferenceCount_(T* pointer, DelRef&& deleter,
                                       const Alloc& allocator)
                : m_Pointer(pointer),
                  m_Deleter(Forward<DelRef>(deleter)),
                  m_Allocator(allocator)
            {
            }

        public:
            inline void DeleteValue()
            {
                if (m_Pointer != nullptr)
                    m_Deleter(m_Pointer);
            }

            inline void DeleteReferenceCount()
            {
                Alloc alloc = Move(m_Allocator);

                this->~TypedReferenceCount_();
                alloc.Free(this, sizeof(*this));
            }

        private:
            T* m_Pointer;

            KITSUNE_MAYBE_OVERLAPPING Del m_Deleter;
            KITSUNE_MAYBE_OVERLAPPING Alloc m_Allocator;
        };

        template<typename To, typename From>
        concept StaticPointerCastable_ = requires
        {
            static_cast<To*>(std::declval<From*>());
        };

        template<typename To, typename From>
        concept DynamicPointerCastable_ = requires
        {
            dynamic_cast<To*>(std::declval<From*>());
        };

        template<typename To, typename From>
        concept ConstPointerCastable_ = requires
        {
            const_cast<To*>(std::declval<From*>());
        };

        template<typename To, typename From>
        concept ReinterpretPointerCastable_ = requires
        {
            reinterpret_cast<To*>(std::declval<From*>());
        };
    }

    template<typename T, ThreadSafety Mode>
    class WeakPtr;

    // A smart pointer that shares ownership of a single resource.
    template<typename T, ThreadSafety Mode = ThreadSafety::ThreadSafe>
    class SharedPtr
    {
    public:
        using ValueType = T;
        using PointerType = T*;

        using WeakType = WeakPtr<T, Mode>;

    public:
        inline SharedPtr()
            : m_Pointer(nullptr), m_Data(nullptr)
        {
        }

        inline SharedPtr(std::nullptr_t)
            : m_Pointer(nullptr), m_Data(nullptr)
        {
        }

        template<typename U>
            requires std::is_convertible_v<U*, T*>
        inline explicit SharedPtr(U* pointer)
            : SharedPtr(pointer, DefaultDeleter<U>(), GlobalAllocator())
        {
        }

        template<typename U, typename Del>
            requires (std::is_convertible_v<U*, T*> &&
                      Deleter<std::remove_reference_t<Del>>)
        inline SharedPtr(U* pointer, Del&& deleter)
            : SharedPtr(pointer, Forward<Del>(deleter), GlobalAllocator())
        {
        }

        template<typename Del>
            requires Deleter<std::remove_reference_t<Del>>
        inline SharedPtr(std::nullptr_t, Del&& deleter)
            : SharedPtr(static_cast<T*>(nullptr), Forward<Del>(deleter))
        {
        }

        template<typename U, typename Del, Allocator Alloc>
            requires (std::is_convertible_v<U*, T*> &&
                      Deleter<std::remove_reference_t<Del>>)
        inline SharedPtr(U* pointer, Del deleter, const Alloc& allocator)
            : m_Pointer(pointer)
        {
            using InternalDataType = Details::TypedReferenceCount_<
                U, Mode,
                std::remove_reference_t<Del>,
                Alloc>;

            try
            {
                Alloc internalAllocator = allocator;
                auto* data = internalAllocator.Allocate(
                    sizeof(InternalDataType), alignof(InternalDataType));

                m_Data = Memory::ConstructAt(
                    static_cast<InternalDataType*>(data),
                    pointer,
                    Forward<Del>(deleter),
                    Move(internalAllocator));
            }
            catch (...)
            {
                if (pointer)
                    deleter(pointer);

                throw;
            }
        }

        template<typename Del, Allocator Alloc>
            requires Deleter<std::remove_reference_t<Del>>
        inline SharedPtr(std::nullptr_t, Del deleter, const Alloc& allocator)
            : SharedPtr(static_cast<T*>(nullptr), Forward<Del>(deleter), allocator)
        {
        }

        inline SharedPtr(const SharedPtr& pointer)
            : m_Pointer(pointer.m_Pointer), m_Data(pointer.m_Data)
        {
            if (m_Data != nullptr)
                m_Data->IncrementReferenceCount();
        }

        template<typename U>
            requires std::is_convertible_v<U*, T*>
        inline SharedPtr(const SharedPtr<U, Mode>& pointer)
            : m_Pointer(pointer.m_Pointer), m_Data(pointer.m_Data)
        {
            if (m_Data != nullptr)
                m_Data->IncrementReferenceCount();
        }

        template<typename U>
        inline SharedPtr(const SharedPtr<U, Mode>& shared, T* pointer)
            : m_Pointer(pointer), m_Data(shared.m_Data)
        {
            if (m_Data != nullptr)
                m_Data->IncrementReferenceCount();
        }

        inline SharedPtr(SharedPtr&& pointer)
            : m_Pointer(pointer.m_Pointer),
              m_Data(Exchange(pointer.m_Data, nullptr))
        {
        }

        template<typename U>
            requires std::is_convertible_v<U*, T*>
        SharedPtr(SharedPtr<U, Mode>&& pointer)
            : m_Pointer(pointer.m_Pointer),
              m_Data(Exchange(pointer.m_Data, nullptr))
        {
        }

        template<typename U>
        inline SharedPtr(SharedPtr<U, Mode>&& shared, T* pointer)
            : m_Pointer(pointer),
              m_Data(Exchange(shared.m_Data, nullptr))
        {
        }

        template<typename U>
            requires std::is_convertible_v<U*, T*>
        inline explicit SharedPtr(const WeakPtr<U, Mode>& pointer)
            : m_Pointer(pointer.m_Pointer), m_Data(pointer.m_Data)
        {
            if (m_Data == nullptr)
                throw BadWeakPtrException();

            m_Data->IncrementReferenceCount();
        }

        template<typename U, Deleter Del>
            requires std::is_convertible_v<U*, T*>
        inline SharedPtr(ScopedPtr<U, Del>&& pointer)
            : SharedPtr(pointer.Release(), Move(pointer.GetDeleter()),
                        GlobalAllocator())
        {
        }

        inline ~SharedPtr()
        {
            if (m_Data != nullptr)
                m_Data->ReleaseOwnership();
        }

    public:
        inline SharedPtr& operator=(const SharedPtr& pointer)
        {
            return InternalAssign_(pointer);
        }

        inline SharedPtr& operator=(SharedPtr&& pointer)
        {
            return InternalAssign_(Move(pointer));
        }

        template<typename U>
            requires std::is_convertible_v<U*, T*>
        inline SharedPtr& operator=(const SharedPtr<U, Mode>& pointer)
        {
            return InternalAssign_(pointer);
        }

        template<typename U>
            requires std::is_convertible_v<U*, T*>
        inline SharedPtr& operator=(SharedPtr<U, Mode>&& pointer)
        {
            return InternalAssign_(Move(pointer));
        }

        template<typename U, Deleter Del>
            requires std::is_convertible_v<U*, T*>
        inline SharedPtr& operator=(ScopedPtr<U, Del>&& pointer)
        {
            SharedPtr(Move(pointer)).Swap(*this);
            return *this;
        }

    public:
        inline std::add_lvalue_reference_t<T> operator*() const
        {
            if constexpr (!std::is_void_v<T>)
                return *m_Pointer;

            /* Turns into a void function, returns nothing. */
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
        inline T* Get() const
        {
            return m_Pointer;
        }

        [[nodiscard]]
        inline Int32 GetCount() const
        {
            if (m_Data == nullptr)
                return 0;

            return m_Data->GetCount();
        }

    public:
        inline void Swap(SharedPtr<T, Mode>& pointer)
        {
            Kitsune::Swap(m_Pointer, pointer.m_Pointer);
            Kitsune::Swap(m_Data, pointer.m_Data);
        }

    private:
        template<typename U>
        inline SharedPtr& InternalAssign_(const SharedPtr<U, Mode>& pointer)
        {
            if (m_Pointer != pointer.m_Pointer)
                SharedPtr(pointer).Swap(*this);

            return *this;
        }

        template<typename U>
        inline SharedPtr& InternalAssign_(SharedPtr<U, Mode>&& pointer)
        {
            if (m_Pointer != pointer.m_Pointer)
                SharedPtr(Move(pointer)).Swap(*this);

            return *this;
        }

    private:
        template<typename U, ThreadSafety OtherMode> friend class SharedPtr;
        template<typename U, ThreadSafety OtherMode> friend class WeakPtr;

    private:
        T* m_Pointer;
        Details::ReferenceCountBase_<Mode>* m_Data;
    };

    template<typename T, typename... Args>
    [[nodiscard]]
    inline SharedPtr<T> MakeShared(Args&&... args)
    {
        return SharedPtr<T>(Memory::New<T>(Forward<Args>(args)...));
    }

    template<typename T, ThreadSafety Mode, typename U, ThreadSafety UMode>
    inline bool operator==(const SharedPtr<T, Mode>& pointer1,
                           const SharedPtr<U, UMode>& pointer2)
    {
        return (pointer1.Get() == pointer2.Get());
    }

    template<typename T, ThreadSafety Mode, typename U, ThreadSafety UMode>
    inline bool operator!=(const SharedPtr<T, Mode>& pointer1,
                           const SharedPtr<U, UMode>& pointer2)
    {
        return (pointer1.Get() != pointer2.Get());
    }

    template<typename T, ThreadSafety Mode, typename U, ThreadSafety UMode>
    inline bool operator>=(const SharedPtr<T, Mode>& pointer1,
                           const SharedPtr<U, UMode>& pointer2)
    {
        return (pointer1.Get() >= pointer2.Get());
    }

    template<typename T, ThreadSafety Mode, typename U, ThreadSafety UMode>
    inline bool operator<=(const SharedPtr<T, Mode>& pointer1,
                           const SharedPtr<U, UMode>& pointer2)
    {
        return (pointer1.Get() <= pointer2.Get());
    }

    template<typename T, ThreadSafety Mode, typename U, ThreadSafety UMode>
    inline bool operator>(const SharedPtr<T, Mode>& pointer1,
                          const SharedPtr<U, UMode>& pointer2)
    {
        return (pointer1.Get() > pointer2.Get());
    }

    template<typename T, ThreadSafety Mode, typename U, ThreadSafety UMode>
    inline bool operator<(const SharedPtr<T, Mode>& pointer1,
                          const SharedPtr<U, UMode>& pointer2)
    {
        return (pointer1.Get() < pointer2.Get());
    }

    template<typename T, ThreadSafety Mode>
    inline bool operator==(std::nullptr_t, const SharedPtr<T, Mode>& pointer)
    {
        return (pointer.Get() == nullptr);
    }

    template<typename T, ThreadSafety Mode>
    inline bool operator!=(std::nullptr_t, const SharedPtr<T, Mode>& pointer)
    {
        return (pointer.Get() != nullptr);
    }

    template<typename T, ThreadSafety Mode>
    inline bool operator>=(std::nullptr_t, const SharedPtr<T, Mode>& pointer)
    {
        return !(nullptr < pointer);
    }

    template<typename T, ThreadSafety Mode>
    inline bool operator<=(std::nullptr_t, const SharedPtr<T, Mode>& pointer)
    {
        return !(nullptr > pointer);
    }

    template<typename T, ThreadSafety Mode>
    inline bool operator>(std::nullptr_t, const SharedPtr<T, Mode>& pointer)
    {
        return (static_cast<T*>(nullptr) > pointer.Get());
    }

    template<typename T, ThreadSafety Mode>
    inline bool operator<(std::nullptr_t, const SharedPtr<T, Mode>& pointer)
    {
        return (static_cast<T*>(nullptr) < pointer.Get());
    }

    template<typename T, ThreadSafety Mode>
    inline bool operator==(const SharedPtr<T, Mode>& pointer, std::nullptr_t)
    {
        return (pointer.Get() == nullptr);
    }

    template<typename T, ThreadSafety Mode>
    inline bool operator!=(const SharedPtr<T, Mode>& pointer, std::nullptr_t)
    {
        return (pointer.Get() != nullptr);
    }

    template<typename T, ThreadSafety Mode>
    inline bool operator>=(const SharedPtr<T, Mode>& pointer, std::nullptr_t)
    {
        return !(pointer < nullptr);
    }

    template<typename T, ThreadSafety Mode>
    inline bool operator<=(const SharedPtr<T, Mode>& pointer, std::nullptr_t)
    {
        return !(pointer > nullptr);
    }

    template<typename T, ThreadSafety Mode>
    inline bool operator>(const SharedPtr<T, Mode>& pointer, std::nullptr_t)
    {
        return (pointer.Get() > static_cast<T*>(nullptr));
    }

    template<typename T, ThreadSafety Mode>
    inline bool operator<(const SharedPtr<T, Mode>& pointer, std::nullptr_t)
    {
        return (pointer.Get() < static_cast<T*>(nullptr));
    }

    // A smart pointer that holds a non-owning reference to a resource.
    template<typename T, ThreadSafety Mode = ThreadSafety::ThreadSafe>
    class WeakPtr
    {
    public:
        using ValueType = T;
        using PointerType = T*;

    public:
        inline WeakPtr()
            : m_Pointer(nullptr), m_Data(nullptr)
        {
        }

        template<typename U>
        inline WeakPtr(const SharedPtr<U, Mode>& pointer)
            : m_Pointer(pointer.m_Pointer), m_Data(pointer.m_Data)
        {
            if (m_Data != nullptr)
                m_Data->IncrementWeakRefCount();
        }

        inline WeakPtr(const WeakPtr& pointer)
            : m_Pointer(pointer.m_Pointer), m_Data(pointer.m_Data)
        {
            if (m_Data != nullptr)
                m_Data->IncrementWeakRefCount();
        }

        template<typename U>
        inline WeakPtr(const WeakPtr<U, Mode>& pointer)
            : m_Pointer(pointer.m_Pointer), m_Data(pointer.m_Data)
        {
            if (m_Data != nullptr)
                m_Data->IncrementWeakRefCount();
        }

        inline WeakPtr(WeakPtr&& pointer)
            : m_Pointer(pointer.m_Pointer),
              m_Data(Exchange(pointer.m_Data, nullptr))
        {
        }

        template<typename U>
        inline WeakPtr(WeakPtr<U, Mode>&& pointer)
            : m_Pointer(pointer.m_Pointer),
              m_Data(Exchange(pointer.m_Data, nullptr))
        {
        }

        inline ~WeakPtr()
        {
            if (m_Data != nullptr)
                m_Data->ReleaseWeakOwnership();
        }

    public:
        inline WeakPtr& operator=(const WeakPtr& pointer)
        {
            return InternalAssign_(pointer);
        }

        inline WeakPtr& operator=(WeakPtr&& pointer)
        {
            return InternalAssign_(Move(pointer));
        }

        template<typename U>
        inline WeakPtr& operator=(const WeakPtr<U, Mode>& pointer)
        {
            return InternalAssign_(pointer);
        }

        template<typename U>
        inline WeakPtr& operator=(WeakPtr<U, Mode>&& pointer)
        {
            return InternalAssign_(Move(pointer));
        }

        template<typename U>
        inline WeakPtr& operator=(const SharedPtr<U>& pointer)
        {
            if (m_Pointer != pointer.m_Pointer)
                WeakPtr(pointer).Swap(*this);

            return *this;
        }

    public:
        inline void Reset()
        {
            if (m_Data != nullptr)
                m_Data->ReleaseWeakOwnership();

            m_Data = nullptr;
        }

        inline void Swap(WeakPtr<T, Mode>& pointer)
        {
            Kitsune::Swap(m_Pointer, pointer.m_Pointer);
            Kitsune::Swap(m_Data, pointer.m_Data);
        }

    public:
        inline Int32 GetCount() const
        {
            if (m_Data == nullptr)
                return 0;

            return m_Data->GetCount();
        }

        inline bool IsExpired() const
        {
            return (GetCount() == 0);
        }

        inline SharedPtr<T> Lock() const
        {
            if (IsExpired())
                return SharedPtr<T>();

            return SharedPtr<T>(*this);
        }

    private:
        template<typename U>
        inline WeakPtr& InternalAssign_(const WeakPtr<U, Mode>& pointer)
        {
            if (m_Pointer != pointer.m_Pointer)
                WeakPtr(pointer).Swap(*this);

            return *this;
        }

        template<typename U>
        inline WeakPtr& InternalAssign_(WeakPtr<U, Mode>&& pointer)
        {
            if (m_Pointer != pointer.m_Pointer)
                WeakPtr(Move(pointer)).Swap(*this);

            return *this;
        }

    private:
        template<typename U, ThreadSafety ModeU> friend class WeakPtr;
        template<typename U, ThreadSafety ModeU> friend class SharedPtr;

    private:
        T* m_Pointer;
        Details::ReferenceCountBase_<Mode>* m_Data;
    };

    template<typename T, ThreadSafety Mode, typename U>
        requires Details::StaticPointerCastable_<T, U>
    SharedPtr<T, Mode> StaticPointerCast(const SharedPtr<U, Mode>& pointer)
    {
        auto castPointer = static_cast<T*>(pointer.Get());
        return SharedPtr<T, Mode>(pointer, castPointer);
    }

    template<typename T, ThreadSafety Mode, typename U>
        requires Details::StaticPointerCastable_<T, U>
    SharedPtr<T, Mode> StaticPointerCast(SharedPtr<U, Mode>&& pointer)
    {
        auto castPointer = static_cast<T*>(pointer.Get());
        return SharedPtr<T, Mode>(Move(pointer), castPointer);
    }

    template<typename T, ThreadSafety Mode, typename U>
        requires Details::DynamicPointerCastable_<T, U>
    SharedPtr<T, Mode> DynamicPointerCast(const SharedPtr<U, Mode>& pointer)
    {
        auto* castPointer = dynamic_cast<T*>(pointer.Get());
        return SharedPtr<T, Mode>(pointer, castPointer);
    }

    template<typename T, ThreadSafety Mode, typename U>
        requires Details::DynamicPointerCastable_<T, U>
    SharedPtr<T, Mode> DynamicPointerCast(SharedPtr<U, Mode>&& pointer)
    {
        auto* castPointer = dynamic_cast<T*>(pointer.Get());
        return SharedPtr<T, Mode>(Move(pointer), castPointer);
    }

    template<typename T, ThreadSafety Mode, typename U>
        requires Details::ConstPointerCastable_<T, U>
    SharedPtr<T, Mode> ConstPointerCast(const SharedPtr<U, Mode>& pointer)
    {
        auto* castPointer = const_cast<T*>(pointer.Get());
        return SharedPtr<T, Mode>(pointer, castPointer);
    }

    template<typename T, ThreadSafety Mode, typename U>
        requires Details::ConstPointerCastable_<T, U>
    SharedPtr<T, Mode> ConstPointerCast(SharedPtr<U, Mode>&& pointer)
    {
        auto* castPointer = const_cast<T*>(pointer.Get());
        return SharedPtr<T, Mode>(Move(pointer), castPointer);
    }

    template<typename T, ThreadSafety Mode, typename U>
        requires Details::ReinterpretPointerCastable_<T, U>
    SharedPtr<T, Mode> ReinterpretPointerCast(const SharedPtr<U, Mode>& pointer)
    {
        auto* castPointer = reinterpret_cast<T*>(pointer.Get());
        return SharedPtr<T, Mode>(pointer, castPointer);
    }

    template<typename T, ThreadSafety Mode, typename U>
        requires Details::ReinterpretPointerCastable_<T, U>
    SharedPtr<T, Mode> ReinterpretPointerCast(SharedPtr<U, Mode>&& pointer)
    {
        auto* castPointer = reinterpret_cast<T*>(pointer.Get());
        return SharedPtr<T, Mode>(Move(pointer), castPointer);
    }
}
