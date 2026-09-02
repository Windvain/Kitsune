#pragma once

#include "Foundation/Threading/Atomic.h"        // IWYU pragma: keep
#include "Foundation/Threading/ThreadSafety.h"

#include "Foundation/Memory/ScopedPtr.h"
#include "Foundation/Memory/Allocator.h"
#include "Foundation/Memory/GlobalAllocator.h"

#include "Foundation/Memory/BadWeakPtrException.h"

namespace Kitsune
{
    namespace Details
    {
        template<ThreadSafety Mode>
        class SharedControlBlockBase
        {
        public:
            using CountBaseType = Int32;
            using CountType = std::conditional_t<
                Mode == ThreadSafety::ThreadSafe,
                Atomic<CountBaseType>,
                CountBaseType>;

        public:
            inline SharedControlBlockBase() = default;
            virtual ~SharedControlBlockBase() = default;

        public:
            inline void IncrementCount()
            {
                m_SharedCount++;
            }

            inline void IncrementWeakCount()
            {
                m_WeakCount++;
            }

            inline CountBaseType DecrementCount()
            {
                return m_SharedCount--;
            }

            inline CountBaseType DecrementWeakCount()
            {
                return m_WeakCount--;
            }

        public:
            inline bool CompareExchange(CountBaseType& expected, CountBaseType desired)
            {
                if constexpr (Mode == ThreadSafety::ThreadSafe)
                    return m_SharedCount.CompareExchange(expected, desired);
                else
                {
                    bool compare = (m_SharedCount == expected);
                    if (compare)
                        m_SharedCount = desired;
                    else
                        expected = m_SharedCount;

                    return compare;
                }
            }

        public:
            [[nodiscard]]
            inline CountBaseType GetCount() const
            {
                return CountBaseType(m_SharedCount);
            }

        public:
            virtual void DeleteValue() = 0;
            virtual void DeleteControlBlock() = 0;

        protected:
            CountType m_SharedCount = 0;
            CountType m_WeakCount = 1;          // Set this to 1 by default.
        };

        template<typename T, ThreadSafety Mode, Deleter Del, Allocator Alloc>
        class SharedControlBlock : public SharedControlBlockBase<Mode>
        {
        public:
            template<typename DelRef>
            inline SharedControlBlock(
                T* pointer, DelRef&& deleter, const Alloc& allocator)
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

            inline void DeleteControlBlock()
            {
                Alloc alloc = Move(m_Allocator);

                this->~SharedControlBlock();
                alloc.Free(this, sizeof(*this));
            }

        private:
            T* m_Pointer;

            KITSUNE_MAYBE_OVERLAPPING Del m_Deleter;
            KITSUNE_MAYBE_OVERLAPPING Alloc m_Allocator;
        };

        template<typename To, typename From>
        concept StaticCastable = requires { static_cast<To>(std::declval<From>()); };

        template<typename To, typename From>
        concept DynamicCastable = requires { dynamic_cast<To>(std::declval<From>()); };

        template<typename To, typename From>
        concept ConstCastable = requires { const_cast<To>(std::declval<From>()); };

        template<typename To, typename From>
        concept ReinterpretCastable = requires
        {
            reinterpret_cast<To>(std::declval<From>());
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
            : SharedPtr(nullptr, nullptr)
        {
        }

        inline SharedPtr(std::nullptr_t)
            : SharedPtr(nullptr, nullptr)
        {
        }

        template<typename U>
            requires std::is_convertible_v<U*, T*>
        inline explicit SharedPtr(U* pointer)
            : SharedPtr(pointer, DefaultDeleter<U>(), GlobalAllocator())
        {
        }

        template<typename U, typename DelRef>
            requires (std::is_convertible_v<U*, T*> &&
                      Deleter<std::remove_reference_t<DelRef>>)
        inline SharedPtr(U* pointer, DelRef&& deleter)
            : SharedPtr(pointer, Forward<DelRef>(deleter), GlobalAllocator())
        {
        }

        template<typename DelRef>
            requires Deleter<std::remove_reference_t<DelRef>>
        inline SharedPtr(std::nullptr_t, DelRef&& deleter)
            : SharedPtr(nullptr, Forward<DelRef>(deleter), GlobalAllocator())
        {
        }

        template<typename U, typename DelRef, Allocator Alloc>
            requires (std::is_convertible_v<U*, T*> &&
                      Deleter<std::remove_reference_t<DelRef>>)
        inline SharedPtr(U* pointer, DelRef&& deleter, const Alloc& allocator)
            : m_Pointer(pointer)
        {
            using ControlBlockType = Details::SharedControlBlock<
                U, Mode,
                std::remove_reference_t<DelRef>,
                Alloc>;

            try
            {
                Alloc copy = allocator;
                auto* data = copy.Allocate(
                    sizeof(ControlBlockType),
                    alignof(ControlBlockType));

                // Increment the weak count. (1)
                m_ControlBlock = Memory::ConstructAt<ControlBlockType>(
                    data,
                    pointer, Forward<DelRef>(deleter), Move(copy));
            }
            catch (...)
            {
                if (pointer)
                    deleter(pointer);

                throw;
            }

            // Increment the shared count. (1)
            m_ControlBlock->IncrementCount();
        }

        template<typename DelRef, Allocator Alloc>
            requires Deleter<std::remove_reference_t<DelRef>>
        inline SharedPtr(std::nullptr_t, DelRef&& deleter, const Alloc& allocator)
            : SharedPtr(static_cast<T*>(nullptr), Forward<DelRef>(deleter),
                        allocator)
        {
        }

        template<typename U>
        inline SharedPtr(const SharedPtr<U, Mode>& shared, T* pointer)
            : SharedPtr(pointer, shared.m_ControlBlock)
        {
            if (m_ControlBlock != nullptr)
                m_ControlBlock->IncrementCount();
        }

        template<typename U>
        inline SharedPtr(SharedPtr<U, Mode>&& shared, T* pointer)
            : SharedPtr(pointer, Exchange(shared.m_ControlBlock, nullptr))
        {
            shared.m_Pointer = nullptr;
        }

        inline SharedPtr(const SharedPtr& pointer)
            : SharedPtr(pointer, pointer.Get())
        {
        }

        inline SharedPtr(SharedPtr&& pointer)
            : SharedPtr(
                Exchange(pointer.m_Pointer, nullptr),
                Exchange(pointer.m_ControlBlock, nullptr))
        {
        }

        template<typename U>
            requires std::is_convertible_v<U*, T*>
        inline SharedPtr(const SharedPtr<U, Mode>& pointer)
            : SharedPtr(pointer, pointer.Get())
        {
        }

        template<typename U>
            requires std::is_convertible_v<U*, T*>
        inline SharedPtr(SharedPtr<U, Mode>&& pointer)
            : SharedPtr(
                Exchange(pointer.m_Pointer, nullptr),
                Exchange(pointer.m_ControlBlock, nullptr))
        {
        }

        template<typename U>
            requires std::is_convertible_v<U*, T*>
        inline explicit SharedPtr(const WeakPtr<U, Mode>& pointer)
            : SharedPtr(pointer.m_Pointer, pointer.m_ControlBlock)
        {
            if (pointer.IsExpired())
                throw BadWeakPtrException();

            m_ControlBlock->IncrementCount();
        }

        template<typename U, Deleter Deleter>
            requires std::is_convertible_v<U*, T*>
        inline SharedPtr(ScopedPtr<U, Deleter>&& pointer)
            : SharedPtr(pointer.Release(), Move(pointer.GetDeleter()))
        {
        }

        inline ~SharedPtr()
        {
            if ((m_ControlBlock != nullptr) && (m_ControlBlock->DecrementCount() == 1))
            {
                m_ControlBlock->DeleteValue();
                if (m_ControlBlock->DecrementWeakCount() == 1)
                {
                    m_ControlBlock->DeleteControlBlock();
                }
            }
        }

    public:
        inline SharedPtr& operator=(const SharedPtr& pointer)
        {
            SharedPtr(pointer).Swap(*this);
            return *this;
        }

        inline SharedPtr& operator=(SharedPtr&& pointer)
        {
            SharedPtr(Move(pointer)).Swap(*this);
            return *this;
        }

        template<typename U>
            requires std::is_convertible_v<U*, T*>
        inline SharedPtr& operator=(const SharedPtr<U>& pointer)
        {
            SharedPtr(pointer).Swap(*this);
            return *this;
        }

        template<typename U>
            requires std::is_convertible_v<U*, T*>
        inline SharedPtr& operator=(SharedPtr<U>&& pointer)
        {
            SharedPtr(Move(pointer)).Swap(*this);
            return *this;
        }

        template<class U, Deleter Del>
        inline SharedPtr& operator=(ScopedPtr<U, Del>&& pointer)
        {
            SharedPtr(Move(pointer)).Swap(*this);
            return *this;
        }

    public:
        inline void Reset()
        {
            SharedPtr().Swap(*this);
        }

        template<typename U>
            requires std::is_convertible_v<U*, T*>
        inline void Reset(U* pointer)
        {
            SharedPtr(pointer).Swap(*this);
        }

        template<typename U, typename DelRef>
            requires (std::is_convertible_v<U*, T*> &&
                      Deleter<std::remove_reference_t<DelRef>>)
        inline void Reset(U* pointer, DelRef&& deleter)
        {
            SharedPtr(pointer, Forward<DelRef>(deleter)).Swap(*this);
        }

        template<typename U, typename DelRef, Allocator Alloc>
            requires (std::is_convertible_v<U*, T*> &&
                      Deleter<std::remove_reference_t<DelRef>>)
        inline void Reset(U* pointer, DelRef&& deleter, const Alloc& allocator)
        {
            SharedPtr(
                pointer,
                Forward<DelRef>(deleter),
                allocator).Swap(*this);
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
            if (m_ControlBlock == nullptr)
                return 0;

            return m_ControlBlock->GetCount();
        }

    public:
        inline void Swap(SharedPtr<T, Mode>& pointer)
        {
            Kitsune::Swap(m_Pointer, pointer.m_Pointer);
            Kitsune::Swap(m_ControlBlock, pointer.m_ControlBlock);
        }

    private:
        template<typename U, ThreadSafety OtherMode> friend class SharedPtr;
        template<typename U, ThreadSafety OtherMode> friend class WeakPtr;

        // Private constructor used for WeakPtr<T>::Lock().
        inline SharedPtr(T* pointer, Details::SharedControlBlockBase<Mode>* controlBlock)
            : m_Pointer(pointer), m_ControlBlock(controlBlock)
        {
        }

    private:
        T* m_Pointer;
        Details::SharedControlBlockBase<Mode>* m_ControlBlock;
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
            : m_Pointer(nullptr), m_ControlBlock(nullptr)
        {
        }

        template<typename U>
        inline WeakPtr(const SharedPtr<U, Mode>& pointer)
            : m_Pointer(pointer.m_Pointer), m_ControlBlock(pointer.m_ControlBlock)
        {
            if (m_ControlBlock != nullptr)
                m_ControlBlock->IncrementWeakCount();
        }

        inline WeakPtr(const WeakPtr& pointer)
            : m_Pointer(pointer.m_Pointer), m_ControlBlock(pointer.m_ControlBlock)
        {
            if (m_ControlBlock != nullptr)
                m_ControlBlock->IncrementWeakCount();
        }

        template<typename U>
        inline WeakPtr(const WeakPtr<U, Mode>& pointer)
            : m_Pointer(pointer.m_Pointer), m_ControlBlock(pointer.m_ControlBlock)
        {
            if (m_ControlBlock != nullptr)
                m_ControlBlock->IncrementWeakCount();
        }

        inline WeakPtr(WeakPtr&& pointer)
            : m_Pointer(Exchange(pointer.m_Pointer, nullptr)),
              m_ControlBlock(Exchange(pointer.m_ControlBlock, nullptr))
        {
        }

        template<typename U>
        inline WeakPtr(WeakPtr<U, Mode>&& pointer)
            : m_Pointer(Exchange(pointer.m_Pointer, nullptr)),
              m_ControlBlock(Exchange(pointer.m_ControlBlock, nullptr))
        {
        }

        inline ~WeakPtr()
        {
            if (m_ControlBlock != nullptr)
                m_ControlBlock->DecrementWeakCount();
        }

    public:
        inline WeakPtr& operator=(const WeakPtr& pointer)
        {
            WeakPtr(pointer).Swap(*this);
            return *this;
        }

        inline WeakPtr& operator=(WeakPtr&& pointer)
        {
            WeakPtr(Move(pointer)).Swap(*this);
            return *this;
        }

        template<typename U>
        inline WeakPtr& operator=(const WeakPtr<U, Mode>& pointer)
        {
            WeakPtr(pointer).Swap(*this);
            return *this;
        }

        template<typename U>
        inline WeakPtr& operator=(WeakPtr<U, Mode>&& pointer)
        {
            WeakPtr(Move(pointer)).Swap(*this);
            return *this;
        }

        template<typename U>
        inline WeakPtr& operator=(const SharedPtr<U>& pointer)
        {
            WeakPtr(pointer).Swap(*this);
            return *this;
        }

    public:
        inline void Reset()
        {
            WeakPtr().Swap(*this);
        }

        inline void Swap(WeakPtr<T, Mode>& pointer)
        {
            Kitsune::Swap(m_Pointer, pointer.m_Pointer);
            Kitsune::Swap(m_ControlBlock, pointer.m_ControlBlock);
        }

    public:
        [[nodiscard]]
        inline Int32 GetCount() const
        {
            if (m_ControlBlock == nullptr)
                return 0;

            return m_ControlBlock->GetCount();
        }

        [[nodiscard]]
        inline bool IsExpired() const
        {
            return (GetCount() == 0);
        }

        [[nodiscard]]
        inline SharedPtr<T, Mode> Lock() const
        {
            if (m_ControlBlock == nullptr)
                return SharedPtr<T, Mode>();

            Int32 current = m_ControlBlock->GetCount();
            while (current > 0)
            {
                if (m_ControlBlock->CompareExchange(current, current + 1))
                    return SharedPtr<T, Mode>(m_Pointer, m_ControlBlock);
            }

            return SharedPtr<T, Mode>();
        }

    private:
        template<typename U, ThreadSafety ModeU> friend class WeakPtr;
        template<typename U, ThreadSafety ModeU> friend class SharedPtr;

    private:
        T* m_Pointer;
        Details::SharedControlBlockBase<Mode>* m_ControlBlock;
    };

    template<typename T, ThreadSafety Mode, typename U>
        requires Details::StaticCastable<T*, U*>
    SharedPtr<T, Mode> StaticPointerCast(const SharedPtr<U, Mode>& pointer)
    {
        return SharedPtr<T, Mode>(pointer, static_cast<T*>(pointer.Get()));
    }

    template<typename T, ThreadSafety Mode, typename U>
        requires Details::StaticCastable<T*, U*>
    SharedPtr<T, Mode> StaticPointerCast(SharedPtr<U, Mode>&& pointer)
    {
        auto castPointer = static_cast<T*>(pointer.Get());
        return SharedPtr<T, Mode>(Move(pointer), castPointer);
    }

    template<typename T, ThreadSafety Mode, typename U>
        requires Details::DynamicCastable<T*, U*>
    SharedPtr<T, Mode> DynamicPointerCast(const SharedPtr<U, Mode>& pointer)
    {
        auto* castPointer = dynamic_cast<T*>(pointer.Get());
        return SharedPtr<T, Mode>(pointer, castPointer);
    }

    template<typename T, ThreadSafety Mode, typename U>
        requires Details::DynamicCastable<T*, U*>
    SharedPtr<T, Mode> DynamicPointerCast(SharedPtr<U, Mode>&& pointer)
    {
        auto* castPointer = dynamic_cast<T*>(pointer.Get());
        return SharedPtr<T, Mode>(Move(pointer), castPointer);
    }

    template<typename T, ThreadSafety Mode, typename U>
        requires Details::ConstCastable<T*, U*>
    SharedPtr<T, Mode> ConstPointerCast(const SharedPtr<U, Mode>& pointer)
    {
        auto* castPointer = const_cast<T*>(pointer.Get());
        return SharedPtr<T, Mode>(pointer, castPointer);
    }

    template<typename T, ThreadSafety Mode, typename U>
        requires Details::ConstCastable<T*, U*>
    SharedPtr<T, Mode> ConstPointerCast(SharedPtr<U, Mode>&& pointer)
    {
        auto* castPointer = const_cast<T*>(pointer.Get());
        return SharedPtr<T, Mode>(Move(pointer), castPointer);
    }

    template<typename T, ThreadSafety Mode, typename U>
        requires Details::ReinterpretCastable<T*, U*>
    SharedPtr<T, Mode> ReinterpretPointerCast(const SharedPtr<U, Mode>& pointer)
    {
        auto* castPointer = reinterpret_cast<T*>(pointer.Get());
        return SharedPtr<T, Mode>(pointer, castPointer);
    }

    template<typename T, ThreadSafety Mode, typename U>
        requires Details::ReinterpretCastable<T*, U*>
    SharedPtr<T, Mode> ReinterpretPointerCast(SharedPtr<U, Mode>&& pointer)
    {
        auto* castPointer = reinterpret_cast<T*>(pointer.Get());
        return SharedPtr<T, Mode>(Move(pointer), castPointer);
    }
}
