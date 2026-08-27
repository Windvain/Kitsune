#pragma once

// IWYU pragma: private, include "Threading/Atomic.h"
#include <intrin.h>
#include <concepts>

#include "Foundation/Common/Predefined.h"
#include "Foundation/Templates/Exchange.h"

#include "Foundation/Threading/LockGuard.h"
#include "Foundation/Threading/MemoryOrder.h"

#include "Foundation/Memory/AddressOf.h"
#include "Foundation/Diagnostics/Assert.h"
#include "Foundation/Utilities/NonCopyable.h"

#if !defined(KITSUNE_COMPILER_MSVC)
    #error This file should only be included when building with the MSVC compiler.
#endif

#if defined(KITSUNE_ARCH_X86_32)
    #error x86 platforms are unsupported when compiling with MSVC.
#endif

#define KITSUNE_INTERNAL_GENERATE_INTRINSIC(name, ...) \
    if constexpr (sizeof(T) == 1)                      \
        return ::name ## 8(__VA_ARGS__);               \
    else if constexpr (sizeof(T) == 2)                 \
        return ::name ## 16(__VA_ARGS__);              \
    else if constexpr (sizeof(T) == 4)                 \
        return ::name(__VA_ARGS__);                    \
    else if constexpr (sizeof(T) == 8)                 \
        return ::name ## 64(__VA_ARGS__);              \
    else                                               \
        KITSUNE_UNREACHABLE()

namespace Kitsune
{
    namespace Details
    {
        template<std::integral T, Usize N = sizeof(T)>
        struct InterlockedType { /* ... */ };

        template<std::integral T>
        struct InterlockedType<T, 1> { using Type = char; };

        template<std::integral T>
        struct InterlockedType<T, 2> { using Type = short; };

        template<std::integral T>
        struct InterlockedType<T, 4> { using Type = long; };

        template<std::integral T>
        struct InterlockedType<T, 8> { using Type = long long; };

        template<std::integral T>
        KITSUNE_FORCEINLINE static T InterlockedExchange(volatile T* pointer, T value)
        {
            using Type = typename InterlockedType<T>::Type;
            KITSUNE_INTERNAL_GENERATE_INTRINSIC(
                _InterlockedExchange,
                reinterpret_cast<volatile Type*>(pointer),
                Type(value));
        }

        // MSVC does not have intrinsics for atomic stores and loads. Forced to use
        // exchange instructions.
        template<std::integral T>
        KITSUNE_FORCEINLINE
        static void InterlockedStore(volatile T* pointer, T value) noexcept
        {
            KITSUNE_UNUSED(InterlockedExchange(pointer, value));
        }

        template<std::integral T>
        KITSUNE_FORCEINLINE static T InterlockedLoad(const volatile T* source) noexcept
        {
            using Type = typename InterlockedType<T>::Type;
            auto pointer = reinterpret_cast<volatile Type*>(
                const_cast<volatile T*>(source));

            KITSUNE_INTERNAL_GENERATE_INTRINSIC(_InterlockedCompareExchange, pointer, 0, 0);
        }

        template<std::integral T>
        KITSUNE_FORCEINLINE static T InterlockedCompareExchangeHelper(
            volatile T* pointer, T expected, T desired)
        {
            KITSUNE_INTERNAL_GENERATE_INTRINSIC(
                _InterlockedCompareExchange, pointer, desired, expected);
        }

        template<std::integral T>
        KITSUNE_FORCEINLINE static bool InterlockedCompareExchange(
            volatile T* pointer, volatile T* expected, T desired)
        {
            using Type = typename InterlockedType<T>::Type;
            Type expectedValue = *reinterpret_cast<volatile Type*>(expected);

            _ReadWriteBarrier();

            Type old = InterlockedCompareExchangeHelper(
                reinterpret_cast<volatile Type*>(pointer),
                expectedValue,
                Type(desired));

            bool success = (old == expectedValue);
            if (!success)
                *expected = old;

            _ReadWriteBarrier();
            return success;
        }

        template<std::integral T>
        KITSUNE_FORCEINLINE static T InterlockedFetchAdd(volatile T* pointer, T value)
        {
            // _InterlockedAdd is only supported on ARM and ARM64 targets as per the
            // documentation.
            // https://learn.microsoft.com/en-us/cpp/intrinsics/interlockedadd-intrinsic-functions?view=msvc-170

            using Type = typename InterlockedType<T>::Type;
            KITSUNE_INTERNAL_GENERATE_INTRINSIC(
                _InterlockedExchangeAdd,
                reinterpret_cast<volatile Type*>(pointer), Type(value));
        }

        template<std::integral T>
        KITSUNE_FORCEINLINE static T InterlockedFetchSub(volatile T* pointer, T value)
        {
            // There is no _InterlockedSub in <intrin.h>. Using _InterlockedExchangeAdd
            // (because there is no _InterlockedAdd) and negating the addend.

            using Type = typename InterlockedType<T>::Type;
            KITSUNE_INTERNAL_GENERATE_INTRINSIC(
                _InterlockedExchangeAdd,
                reinterpret_cast<volatile Type*>(pointer), -Type(value));
        }

        template<std::integral T>
        KITSUNE_FORCEINLINE static T InterlockedFetchAnd(volatile T* pointer, T value)
        {
            using Type = typename InterlockedType<T>::Type;
            KITSUNE_INTERNAL_GENERATE_INTRINSIC(
                _InterlockedAnd,
                reinterpret_cast<volatile Type*>(pointer), Type(value));
        }

        template<std::integral T>
        KITSUNE_FORCEINLINE static T InterlockedFetchOr(volatile T* pointer, T value)
        {
            using Type = typename InterlockedType<T>::Type;
            KITSUNE_INTERNAL_GENERATE_INTRINSIC(
                _InterlockedOr,
                reinterpret_cast<volatile Type*>(pointer), Type(value));
        }

        template<std::integral T>
        KITSUNE_FORCEINLINE static T InterlockedFetchXor(volatile T* pointer, T value)
        {
            using Type = typename InterlockedType<T>::Type;
            KITSUNE_INTERNAL_GENERATE_INTRINSIC(
                _InterlockedXor,
                reinterpret_cast<volatile Type*>(pointer), Type(value));
        }
    }

    template<typename T>
    class Atomic : public NonCopyable
    {
    public:
        using ValueType = T;

    public:
        inline Atomic() noexcept(std::is_nothrow_default_constructible_v<T>)
            : m_Value()
        {
        }

        inline Atomic(T value) noexcept
            : m_Value(value)
        {
        }

    public:
        inline Atomic& operator=(T value) noexcept
        {
            Store(value);
            return *this;
        }

    public:
        inline operator T() const noexcept
        {
            return Load();
        }

    public:
        inline void Store(T value, MemoryOrder order = MemoryOrder::SeqCst) noexcept
        {
            KITSUNE_ASSERT(
                (order != MemoryOrder::Consume) && (order != MemoryOrder::Acquire) ||
                (order != MemoryOrder::AcqRel),
                "MemoryOrder::Consume, MemoryOrder::Acquire, and MemoryOrder::AcqRel "
                "are not valid memory order variants.");

            LockGuard guard(m_Lock);
            m_Value = value;
        }

        inline T Load(MemoryOrder order = MemoryOrder::SeqCst) const noexcept
        {
            KITSUNE_ASSERT(
                (order != MemoryOrder::Release) && (order != MemoryOrder::AcqRel),
                "MemoryOrder::Release and MemoryOrder::AcqRel are not valid memory "
                "order variants.");

            LockGuard guard(m_Lock);
            return m_Value;
        }

        inline T Exchange(T value, MemoryOrder order = MemoryOrder::SeqCst) noexcept
        {
            LockGuard guard(m_Lock);
            return Kitsune::Exchange(m_Value, value);
        }

        inline bool CompareExchange(
            T& expected, T desired, MemoryOrder order = MemoryOrder::SeqCst) noexcept
        {
            LockGuard guard(m_Lock);
            bool success = (m_Value == expected);

            if (success)
                m_Value = desired;
            else
                expected = m_Value;

            return success;
        }

    private:
        T m_Value;
        mutable Mutex m_Lock;
    };

    template<>
    class Atomic<bool> : public NonCopyable
    {
    public:
        using ValueType = bool;

    public:
        inline Atomic() noexcept(std::is_nothrow_default_constructible_v<bool>)
            : m_Value()
        {
        }

        inline Atomic(bool value) noexcept
            : m_Value(value)
        {
        }

    public:
        inline Atomic& operator=(bool value) noexcept
        {
            Store(value);
            return *this;
        }

    public:
        inline operator bool() const noexcept
        {
            return Load();
        }

    public:
        inline void Store(bool value, MemoryOrder order = MemoryOrder::SeqCst) noexcept
        {
            KITSUNE_UNUSED(order);
            KITSUNE_ASSERT(
                (order != MemoryOrder::Consume) && (order != MemoryOrder::Acquire) ||
                (order != MemoryOrder::AcqRel),
                "MemoryOrder::Consume, MemoryOrder::Acquire, and MemoryOrder::AcqRel "
                "are not valid memory order variants.");

            Details::InterlockedStore(AddressOf(m_Value), value);
        }

        inline bool Load(MemoryOrder order = MemoryOrder::SeqCst) const noexcept
        {
            KITSUNE_UNUSED(order);
            KITSUNE_ASSERT(
                (order != MemoryOrder::Release) && (order != MemoryOrder::AcqRel),
                "MemoryOrder::Release and MemoryOrder::AcqRel are not valid memory "
                "order variants.");

            return Details::InterlockedLoad(AddressOf(m_Value));
        }

        inline bool Exchange(
            bool value,
            MemoryOrder order = MemoryOrder::SeqCst) noexcept
        {
            return Details::InterlockedExchange(AddressOf(m_Value), value);
        }

        inline bool CompareExchange(
            bool& expected,
            bool desired,
            MemoryOrder order = MemoryOrder::SeqCst) noexcept
        {
            return Details::InterlockedCompareExchange(
                AddressOf(m_Value), AddressOf(expected), desired);
        }

    private:
        bool m_Value;
    };

    template<typename T>
        requires (std::integral<T> && !std::is_same_v<T, bool>)
    class Atomic<T> : public NonCopyable
    {
    public:
        using ValueType = T;

    public:
        inline Atomic() noexcept(std::is_nothrow_default_constructible_v<T>)
            : m_Value()
        {
        }

        inline Atomic(T value) noexcept
            : m_Value(value)
        {
        }

    public:
        inline Atomic& operator=(T value) noexcept
        {
            Store(value);
            return *this;
        }

    public:
        inline T operator+=(T value) noexcept { return (FetchAdd(value) + value); }
        inline T operator-=(T value) noexcept { return (FetchSub(value) - value); }
        inline T operator&=(T value) noexcept { return (FetchAnd(value) & value); }
        inline T operator|=(T value) noexcept { return (FetchOr(value) | value); }
        inline T operator^=(T value) noexcept { return (FetchXor(value) ^ value); }

        inline operator T() const noexcept
        {
            return Load();
        }

    public:
        inline T operator++() noexcept { return FetchAdd(1) + 1; }
        inline T operator--() noexcept { return FetchSub(1) - 1; }

        inline T operator++(int) noexcept { return FetchAdd(1); }
        inline T operator--(int) noexcept { return FetchSub(1); }

    public:
        inline void Store(T value, MemoryOrder order = MemoryOrder::SeqCst) noexcept
        {
            KITSUNE_UNUSED(order);
            KITSUNE_ASSERT(
                (order != MemoryOrder::Consume) && (order != MemoryOrder::Acquire) ||
                (order != MemoryOrder::AcqRel),
                "MemoryOrder::Consume, MemoryOrder::Acquire, and MemoryOrder::AcqRel "
                "are not valid memory order variants.");

            Details::InterlockedStore(AddressOf(m_Value), value);
        }

        inline T Load(MemoryOrder order = MemoryOrder::SeqCst) const noexcept
        {
            KITSUNE_UNUSED(order);
            KITSUNE_ASSERT(
                (order != MemoryOrder::Release) && (order != MemoryOrder::AcqRel),
                "MemoryOrder::Release and MemoryOrder::AcqRel are not valid memory "
                "order variants.");

            return Details::InterlockedLoad(AddressOf(m_Value));
        }

        inline T Exchange(T value, MemoryOrder order = MemoryOrder::SeqCst) noexcept
        {
            KITSUNE_UNUSED(order);
            return Details::InterlockedExchange(AddressOf(m_Value), value);
        }

        inline bool CompareExchange(
            T& expected, T desired,
            MemoryOrder order = MemoryOrder::SeqCst) noexcept
        {
            KITSUNE_UNUSED(order);
            return Details::InterlockedCompareExchange(
                AddressOf(m_Value), AddressOf(expected), desired);
        }

    public:
        inline T FetchAdd(T value, MemoryOrder order = MemoryOrder::SeqCst) noexcept
        {
            KITSUNE_UNUSED(order);
            return Details::InterlockedFetchAdd(AddressOf(m_Value), value);
        }

        inline T FetchSub(T value, MemoryOrder order = MemoryOrder::SeqCst) noexcept
        {
            KITSUNE_UNUSED(order);
            return Details::InterlockedFetchSub(AddressOf(m_Value), value);
        }

        inline T FetchAnd(T value, MemoryOrder order = MemoryOrder::SeqCst) noexcept
        {
            KITSUNE_UNUSED(order);
            return Details::InterlockedFetchAnd(AddressOf(m_Value), value);
        }

        inline T FetchOr(T value, MemoryOrder order = MemoryOrder::SeqCst) noexcept
        {
            KITSUNE_UNUSED(order);
            return Details::InterlockedFetchOr(AddressOf(m_Value), value);
        }

        inline T FetchXor(T value, MemoryOrder order = MemoryOrder::SeqCst) noexcept
        {
            KITSUNE_UNUSED(order);
            return Details::InterlockedFetchXor(AddressOf(m_Value), value);
        }

    public:
        T m_Value;
    };
}

#undef KITSUNE_INTERNAL_GENERATE_INTRINSIC
