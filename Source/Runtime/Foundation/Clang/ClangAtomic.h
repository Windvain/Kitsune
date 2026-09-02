#pragma once

// IWYU pragma: private, include "Threading/Atomic.h"
#include <concepts>

#include "Foundation/Memory/AddressOf.h"
#include "Foundation/Diagnostics/Assert.h"

#include "Foundation/Threading/MemoryOrder.h"
#include "Foundation/Utilities/NonCopyable.h"

#if !defined(KITSUNE_COMPILER_CLANG)
    #error This file should only be included when building with the LLVM-Clang compiler.
#endif

// Checks to make sure that calling the macros with MemoryOrder enumerations will
// work.
namespace Kitsune
{
    static_assert(
        int(MemoryOrder::Relaxed) == __ATOMIC_RELAXED,
        "MemoryOrder::Relaxed does not match the value of __ATOMIC_RELAXED.");

    static_assert(
        int(MemoryOrder::Consume) == __ATOMIC_CONSUME,
        "MemoryOrder::Consume does not match the value of __ATOMIC_CONSUME.");

    static_assert(
        int(MemoryOrder::Acquire) == __ATOMIC_ACQUIRE,
        "MemoryOrder::Acquire does not match the value of __ATOMIC_ACQUIRE.");

    static_assert(
        int(MemoryOrder::Release) == __ATOMIC_RELEASE,
        "MemoryOrder::Release does not match the value of __ATOMIC_RELEASE.");

    static_assert(
        int(MemoryOrder::AcqRel) == __ATOMIC_ACQ_REL,
        "MemoryOrder::AcquireRelease does not match the value of __ATOMIC_ACQ_REL.");

    static_assert(
        int(MemoryOrder::SeqCst) == __ATOMIC_SEQ_CST,
        "MemoryOrder::SequentiallyConsistent does not match the value "
        "of __ATOMIC_SEQ_CST.");

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

            __atomic_store(AddressOf(m_Value), AddressOf(value), int(order));
        }

        inline T Load(MemoryOrder order = MemoryOrder::SeqCst) const noexcept
        {
            KITSUNE_ASSERT(
                (order != MemoryOrder::Release) && (order != MemoryOrder::AcqRel),
                "MemoryOrder::Release and MemoryOrder::AcqRel are not valid memory "
                "order variants.");

            T value;
            __atomic_load(AddressOf(m_Value), AddressOf(value), int(order));

            return value;
        }

        inline T Exchange(T value, MemoryOrder order = MemoryOrder::SeqCst) noexcept
        {
            T ret;
            __atomic_exchange(
                AddressOf(m_Value),
                AddressOf(value),
                AddressOf(ret),
                int(order));

            return ret;
        }

        inline bool CompareExchange(
            T& expected, T desired, MemoryOrder order = MemoryOrder::SeqCst) noexcept
        {
            return __atomic_compare_exchange(
                AddressOf(m_Value),
                AddressOf(expected),
                AddressOf(desired),
                false,
                int(order), int(order));
        }

    private:
        T m_Value;
    };

    template<typename T>
        requires ((std::integral<T> || std::is_pointer_v<T>) && !std::is_same_v<T, bool>)
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
            KITSUNE_ASSERT(
                (order != MemoryOrder::Consume) && (order != MemoryOrder::Acquire) ||
                (order != MemoryOrder::AcqRel),
                "MemoryOrder::Consume, MemoryOrder::Acquire, and MemoryOrder::AcqRel "
                "are not valid memory order variants.");

            __atomic_store_n(AddressOf(m_Value), value, int(order));
        }

        inline T Load(MemoryOrder order = MemoryOrder::SeqCst) const noexcept
        {
            KITSUNE_ASSERT(
                (order != MemoryOrder::Release) && (order != MemoryOrder::AcqRel),
                "MemoryOrder::Release and MemoryOrder::AcqRel are not valid memory "
                "order variants.");

            return __atomic_load_n(AddressOf(m_Value), int(order));
        }

        inline T Exchange(T value, MemoryOrder order = MemoryOrder::SeqCst) noexcept
        {
            return __atomic_exchange_n(AddressOf(m_Value), value, int(order));
        }

        inline bool CompareExchange(
            T& expected, T desired,
            MemoryOrder order = MemoryOrder::SeqCst) noexcept
        {
            return __atomic_compare_exchange_n(
                AddressOf(m_Value),
                AddressOf(expected),
                desired, false, int(order), int(order));
        }

    public:
        inline T FetchAdd(T value, MemoryOrder order = MemoryOrder::SeqCst) noexcept
        {
            return __atomic_fetch_add(AddressOf(m_Value), value, int(order));
        }

        inline T FetchSub(T value, MemoryOrder order = MemoryOrder::SeqCst) noexcept
        {
            return __atomic_fetch_sub(AddressOf(m_Value), value, int(order));
        }

        inline T FetchAnd(T value, MemoryOrder order = MemoryOrder::SeqCst) noexcept
        {
            return __atomic_fetch_and(AddressOf(m_Value), value, int(order));
        }

        inline T FetchOr(T value, MemoryOrder order = MemoryOrder::SeqCst) noexcept
        {
            return __atomic_fetch_or(AddressOf(m_Value), value, int(order));
        }

        inline T FetchXor(T value, MemoryOrder order = MemoryOrder::SeqCst) noexcept
        {
            return __atomic_fetch_xor(AddressOf(m_Value), value, int(order));
        }

    public:
        T m_Value;
    };
}
