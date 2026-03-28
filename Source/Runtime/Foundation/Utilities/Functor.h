#pragma once

#include <type_traits>

#include "Foundation/Common/Types.h"
#include "Foundation/Common/Macros.h"
#include "Foundation/Concepts/Invocable.h"

#include "Foundation/Templates/Move.h"
#include "Foundation/Templates/Swap.h"

#include "Foundation/Memory/Memory.h"
#include "Foundation/Utilities/BadCallException.h"

namespace Kitsune
{
    namespace Details
    {
        template<typename Ret, typename... Args>
        class CallableStorageBase
        {
        public:
            virtual ~CallableStorageBase() { /* ... */ }

        public:
            virtual Ret Invoke(Args&&... args) const = 0;

            // A pointer to an array.. This is utter blasphemy.
            // But at least it compiles ¯\_(ツ)_/¯
            virtual CallableStorageBase* Clone(Uint8 (*smallStorage)[]) const = 0;
            virtual CallableStorageBase* MoveTo(Uint8 (*smallStorage)[]) = 0;
        };

        template<typename T, typename Ret, typename... Args>
        class CallableStorage : public CallableStorageBase<Ret, Args...>
        {
        private:
            using BaseType_ = CallableStorageBase<Ret, Args...>;
            using ThisType_ = CallableStorage<T, Ret, Args...>;

        public:
            inline CallableStorage(const T& callable)
                : m_Callable(callable)
            {
            }

            inline CallableStorage(T&& callable)
                : m_Callable(Move(callable))
            {
            }

        public:
            inline Ret Invoke(Args&&... args) const override
            {
                return m_Callable(Forward<Args>(args)...);
            }

            inline BaseType_* Clone(Uint8 (*smallStorage)[]) const override
            {
                if constexpr (sizeof(*this) > sizeof(void*))
                    return Memory::New<ThisType_>(m_Callable);
                else
                    return Memory::ConstructAt<ThisType_>(*smallStorage, m_Callable);
            }

            inline BaseType_* MoveTo(Uint8 (*smallStorage)[]) override
            {
                if constexpr (sizeof(*this) <= sizeof(void*))
                {
                    return Memory::ConstructAt<ThisType_>(
                        *smallStorage,
                        Move(m_Callable));
                }

                // MoveTo() should only be called when the functor is stored
                // locally/small. If the callable type is stored on the heap,
                // then the pointers are swapped instead.
                KITSUNE_UNREACHABLE();
            }

        private:
            T m_Callable;
        };

        template<typename Ret, typename... Args>
        class FunctorStorage
        {
        public:
            inline FunctorStorage() = default;

            template<typename T>
                requires InvocableReturn<std::decay_t<T>, Ret, Args...>
            inline FunctorStorage(T&& callable)
            {
                using LargeStorage = CallableStorage<std::decay_t<T>, Ret, Args...>;
                using SmallStorage = decltype(m_SmallStorage);

                if constexpr (sizeof(LargeStorage) > sizeof(SmallStorage))
                    m_Pointer = Memory::New<LargeStorage>(Forward<T>(callable));
                else
                {
                    m_Pointer = Memory::ConstructAt<LargeStorage>(
                        m_SmallStorage,
                        Forward<T>(callable));
                }
            }

            inline FunctorStorage(const FunctorStorage& storage)
                : m_Pointer(storage.m_Pointer->Clone(&m_SmallStorage))
            {
            }

            inline FunctorStorage(FunctorStorage&& storage)
                : FunctorStorage()
            {
                // Assign_() will check whether (*this) functor is empty. Make sure
                // to construct it first, so we don't get UB.
                Assign_(Move(storage));
            }

            inline ~FunctorStorage()
            {
                Destroy();
            }

        public:
            inline FunctorStorage& operator=(const FunctorStorage& storage)
            {
                Destroy();
                m_Pointer = storage.m_Pointer->Clone(&m_SmallStorage);

                return *this;
            }

            inline FunctorStorage& operator=(FunctorStorage&& storage)
            {
                Assign_(Move(storage));
                return *this;
            }

        public:
            void Destroy()
            {
                if (m_Pointer == nullptr)
                    return;

                if (!IsLocal_())
                    Memory::Delete(m_Pointer);
                else
                    Memory::DestroyAt(m_Pointer);

                m_Pointer = nullptr;
            }

        public:
            inline CallableStorageBase<Ret, Args...>* Get()
            {
                return m_Pointer;
            }

            inline const CallableStorageBase<Ret, Args...>* Get() const
            {
                return m_Pointer;
            }

        private:
            [[nodiscard]]
            inline bool IsLocal_() const
            {
                return (m_Pointer == static_cast<const void*>(m_SmallStorage));
            }

            inline void Assign_(FunctorStorage&& storage)
            {
                Destroy();
                if (!storage.IsLocal_())
                    m_Pointer = storage.m_Pointer;
                else
                {
                    m_Pointer = storage.m_Pointer->MoveTo(&m_SmallStorage);
                    Memory::DestroyAt(storage.m_Pointer);
                }

                storage.m_Pointer = nullptr;
            }

        private:
            CallableStorageBase<Ret, Args...>* m_Pointer = nullptr;
            Uint8 m_SmallStorage[8] = { 0 };
        };
    }

    template<typename T>
    class Functor;

    template<typename Ret, typename... Args>
    class Functor<Ret(Args...)>
    {
    private:
        using StorageType_ = Details::FunctorStorage<Ret, Args...>;

    public:
        Functor() = default;
        inline Functor(std::nullptr_t)
        {
        }

        template<InvocableReturn<Ret, Args...> T>
        inline Functor(T&& callable)
            : m_Storage(Forward<T>(callable))
        {
        }

        Functor(const Functor& functor) = default;
        Functor(Functor&& functor) = default;

        ~Functor() = default;

    public:
        Functor& operator=(const Functor& functor) = default;
        Functor& operator=(Functor&& functor) = default;

        inline Functor& operator=(std::nullptr_t)
        {
            m_Storage.Destroy();
            return *this;
        }

        template<InvocableReturn<Ret, Args...> T>
        inline Functor& operator=(T&& callable)
        {
            Functor(Forward<T>(callable)).Swap(*this);
            return *this;
        }

    public:
        inline explicit operator bool() const
        {
            return !IsEmpty();
        }

        inline Ret operator()(Args... args) const
        {
            if (IsEmpty())
                throw BadCallException();

            return m_Storage.Get()->Invoke(Forward<Args>(args)...);
        }

    public:
        [[nodiscard]]
        inline bool IsEmpty() const
        {
            return (m_Storage.Get() == nullptr);
        }

        [[nodiscard]] inline void* Get() { return m_Storage.Get(); }
        [[nodiscard]] inline const void* Get() const { return m_Storage.Get(); }

    public:
        inline void Swap(Functor& functor)
        {
            Kitsune::Swap(m_Storage, functor.m_Storage);
        }

    private:
        StorageType_ m_Storage;
    };

    template<typename Ret, typename... Args>
    inline bool operator==(const Functor<Ret(Args...)>& functor, std::nullptr_t)
    {
        return functor.IsEmpty();
    }

    template<typename Ret, typename... Args>
    inline bool operator==(std::nullptr_t, const Functor<Ret(Args...)>& functor)
    {
        return functor.IsEmpty();
    }
}
