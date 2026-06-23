#pragma once

#include <concepts>

#include "Foundation/Common/Types.h"
#include "Foundation/Common/Macros.h"

#include "Foundation/Memory/AddressOf.h"
#include "Foundation/Diagnostics/Assert.h"

#include "Foundation/Threading/MemoryOrder.h"
#include "Foundation/Utilities/NonCopyable.h"

#if defined(KITSUNE_COMPILER_CLANG)
    #include "Foundation/Clang/ClangAtomicMacros.h"
#elif defined(KITSUNE_COMPILER_MSVC)
    #include "Foundation/Msvc/MsvcAtomicMacros.h"
#else
    #error No implementations for atomic macros were found.
#endif

namespace Kitsune
{
    namespace Details
    {
        // This class exists because of, yet again, MSVC. Atomic intrinsics in MSVC
        // are not general functions, rather there are individual functions for
        // 8-bit, 16-bit, 32-bit, and 64-bit versions of the intrinsics.
        template<std::integral T, Usize Size = sizeof(T)>
        class IntegralAtomic;

        template<std::integral T>
        class IntegralAtomic<T, 1>
        {
        public:
            KITSUNE_FORCEINLINE static void Store(
                T* pointer, const T* value, MemoryOrder order) noexcept
            {
                KITSUNE_ATOMIC_STORE_8(T, order, pointer, value);
            }

            KITSUNE_FORCEINLINE static void Load(
                T* dest, const T* source, MemoryOrder order) noexcept
            {
                KITSUNE_ATOMIC_LOAD_8(T, order, dest, source);
            }

            KITSUNE_FORCEINLINE static void Exchange(
                T* pointer, const T* value, T* ret, MemoryOrder order) noexcept
            {
                KITSUNE_ATOMIC_EXCHANGE_8(T, order, pointer, value, ret);
            }

        public:
            KITSUNE_FORCEINLINE static T FetchAdd(
                T* pointer, T value, MemoryOrder order) noexcept
            {
                return KITSUNE_ATOMIC_FETCH_ADD_8(T, order, pointer, value);
            }

            KITSUNE_FORCEINLINE static T FetchSub(
                T* pointer, T value, MemoryOrder order) noexcept
            {
                return KITSUNE_ATOMIC_FETCH_SUB_8(T, order, pointer, value);
            }

            KITSUNE_FORCEINLINE static T FetchAnd(
                T* pointer, T value, MemoryOrder order) noexcept
            {
                return KITSUNE_ATOMIC_FETCH_AND_8(T, order, pointer, value);
            }

            KITSUNE_FORCEINLINE static T FetchXor(
                T* pointer, T value, MemoryOrder order) noexcept
            {
                return KITSUNE_ATOMIC_FETCH_XOR_8(T, order, pointer, value);
            }

            KITSUNE_FORCEINLINE static T FetchOr(
                T* pointer, T value, MemoryOrder order) noexcept
            {
                return KITSUNE_ATOMIC_FETCH_OR_8(T, order, pointer, value);
            }

            KITSUNE_FORCEINLINE static T FetchNand(
                T* pointer, T value, MemoryOrder order) noexcept
            {
                return KITSUNE_ATOMIC_FETCH_NAND_8(T, order, pointer, value);
            }
        };

        template<std::integral T>
        class IntegralAtomic<T, 2>
        {
        public:
            KITSUNE_FORCEINLINE static void Store(
                T* pointer, const T* value, MemoryOrder order) noexcept
            {
                KITSUNE_ATOMIC_STORE_16(T, order, pointer, value);
            }

            KITSUNE_FORCEINLINE static void Load(
                T* dest, const T* source, MemoryOrder order) noexcept
            {
                KITSUNE_ATOMIC_LOAD_16(T, order, dest, source);
            }

            KITSUNE_FORCEINLINE static void Exchange(
                T* pointer, const T* value, T* ret, MemoryOrder order) noexcept
            {
                KITSUNE_ATOMIC_EXCHANGE_16(T, order, pointer, value, ret);
            }

        public:
            KITSUNE_FORCEINLINE static T FetchAdd(
                T* pointer, T value, MemoryOrder order) noexcept
            {
                return KITSUNE_ATOMIC_FETCH_ADD_16(T, order, pointer, value);
            }

            KITSUNE_FORCEINLINE static T FetchSub(
                T* pointer, T value, MemoryOrder order) noexcept
            {
                return KITSUNE_ATOMIC_FETCH_SUB_16(T, order, pointer, value);
            }

            KITSUNE_FORCEINLINE static T FetchAnd(
                T* pointer, T value, MemoryOrder order) noexcept
            {
                return KITSUNE_ATOMIC_FETCH_AND_16(T, order, pointer, value);
            }

            KITSUNE_FORCEINLINE static T FetchXor(
                T* pointer, T value, MemoryOrder order) noexcept
            {
                return KITSUNE_ATOMIC_FETCH_XOR_16(T, order, pointer, value);
            }

            KITSUNE_FORCEINLINE static T FetchOr(
                T* pointer, T value, MemoryOrder order) noexcept
            {
                return KITSUNE_ATOMIC_FETCH_OR_16(T, order, pointer, value);
            }

            KITSUNE_FORCEINLINE static T FetchNand(
                T* pointer, T value, MemoryOrder order) noexcept
            {
                return KITSUNE_ATOMIC_FETCH_NAND_16(T, order, pointer, value);
            }
        };

        template<std::integral T>
        class IntegralAtomic<T, 4>
        {
        public:
            KITSUNE_FORCEINLINE static void Store(
                T* pointer, const T* value, MemoryOrder order) noexcept
            {
                KITSUNE_ATOMIC_STORE_32(T, order, pointer, value);
            }

            KITSUNE_FORCEINLINE static void Load(
                T* dest, const T* source, MemoryOrder order) noexcept
            {
                KITSUNE_ATOMIC_LOAD_32(T, order, dest, source);
            }

            KITSUNE_FORCEINLINE static void Exchange(
                T* pointer, const T* value, T* ret, MemoryOrder order) noexcept
            {
                KITSUNE_ATOMIC_EXCHANGE_32(T, order, pointer, value, ret);
            }

        public:
            KITSUNE_FORCEINLINE static T FetchAdd(
                T* pointer, T value, MemoryOrder order) noexcept
            {
                return KITSUNE_ATOMIC_FETCH_ADD_32(T, order, pointer, value);
            }

            KITSUNE_FORCEINLINE static T FetchSub(
                T* pointer, T value, MemoryOrder order) noexcept
            {
                return KITSUNE_ATOMIC_FETCH_SUB_32(T, order, pointer, value);
            }

            KITSUNE_FORCEINLINE static T FetchAnd(
                T* pointer, T value, MemoryOrder order) noexcept
            {
                return KITSUNE_ATOMIC_FETCH_AND_32(T, order, pointer, value);
            }

            KITSUNE_FORCEINLINE static T FetchXor(
                T* pointer, T value, MemoryOrder order) noexcept
            {
                return KITSUNE_ATOMIC_FETCH_XOR_32(T, order, pointer, value);
            }

            KITSUNE_FORCEINLINE static T FetchOr(
                T* pointer, T value, MemoryOrder order) noexcept
            {
                return KITSUNE_ATOMIC_FETCH_OR_32(T, order, pointer, value);
            }

            KITSUNE_FORCEINLINE static T FetchNand(
                T* pointer, T value, MemoryOrder order) noexcept
            {
                return KITSUNE_ATOMIC_FETCH_NAND_32(T, order, pointer, value);
            }
        };

        template<std::integral T>
        class IntegralAtomic<T, 8>
        {
        public:
            KITSUNE_FORCEINLINE static void Store(
                T* pointer, const T* value, MemoryOrder order) noexcept
            {
                KITSUNE_ATOMIC_STORE_64(T, order, pointer, value);
            }

            KITSUNE_FORCEINLINE static void Load(
                T* dest, const T* source, MemoryOrder order) noexcept
            {
                KITSUNE_ATOMIC_LOAD_64(T, order, dest, source);
            }

            KITSUNE_FORCEINLINE static void Exchange(
                T* pointer, const T* value, T* ret, MemoryOrder order) noexcept
            {
                KITSUNE_ATOMIC_EXCHANGE_64(T, order, pointer, value, ret);

            }
        public:
            KITSUNE_FORCEINLINE static T FetchAdd(
                T* pointer, T value, MemoryOrder order) noexcept
            {
                return KITSUNE_ATOMIC_FETCH_ADD_64(T, order, pointer, value);
            }

            KITSUNE_FORCEINLINE static T FetchSub(
                T* pointer, T value, MemoryOrder order) noexcept
            {
                return KITSUNE_ATOMIC_FETCH_SUB_64(T, order, pointer, value);
            }

            KITSUNE_FORCEINLINE static T FetchAnd(
                T* pointer, T value, MemoryOrder order) noexcept
            {
                return KITSUNE_ATOMIC_FETCH_AND_64(T, order, pointer, value);
            }

            KITSUNE_FORCEINLINE static T FetchXor(
                T* pointer, T value, MemoryOrder order) noexcept
            {
                return KITSUNE_ATOMIC_FETCH_XOR_64(T, order, pointer, value);
            }

            KITSUNE_FORCEINLINE static T FetchOr(
                T* pointer, T value, MemoryOrder order) noexcept
            {
                return KITSUNE_ATOMIC_FETCH_OR_64(T, order, pointer, value);
            }

            KITSUNE_FORCEINLINE static T FetchNand(
                T* pointer, T value, MemoryOrder order) noexcept
            {
                return KITSUNE_ATOMIC_FETCH_NAND_64(T, order, pointer, value);
            }
        };
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

            KITSUNE_GENERAL_ATOMIC_STORE(T, order, AddressOf(m_Value), AddressOf(value));
        }

        inline T Load(MemoryOrder order = MemoryOrder::SeqCst) const noexcept
        {
            KITSUNE_ASSERT(
                (order != MemoryOrder::Release) && (order != MemoryOrder::AcqRel),
                "MemoryOrder::Release and MemoryOrder::AcqRel are not valid memory "
                "order variants.");

            T value;
            KITSUNE_GENERAL_ATOMIC_LOAD(T, order, AddressOf(value), AddressOf(m_Value));

            return value;
        }

        inline T Exchange(T value, MemoryOrder order = MemoryOrder::SeqCst) noexcept
        {
            T ret;
            KITSUNE_GENERAL_ATOMIC_EXCHANGE(
                T, order,
                AddressOf(m_Value), AddressOf(value), AddressOf(ret));

            return ret;
        }

    private:
        T m_Value;
    };

    template<std::integral T>
    class Atomic<T> : public NonCopyable
    {
    private:
        // Not actual base, just contains the relevant functions.
        using Base = Details::IntegralAtomic<T>;

    public:
        using ValueType = T;
        using DifferenceType = T;

    public:
        inline Atomic() noexcept
            : m_Value()
        {
        }

        inline Atomic(T value) noexcept
            : m_Value(value)
        {
        }

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
                (order != MemoryOrder::Consume) && (order != MemoryOrder::Acquire) &&
                (order != MemoryOrder::AcqRel),
                "MemoryOrder::Consume, MemoryOrder::Acquire, and MemoryOrder::AcqRel "
                "are not valid memory order variants.");

            Base::Store(&m_Value, &value, order);
        }

        inline T Load(MemoryOrder order = MemoryOrder::SeqCst) const noexcept
        {
            KITSUNE_ASSERT(
                (order != MemoryOrder::Release) && (order != MemoryOrder::AcqRel),
                "MemoryOrder::Release and MemoryOrder::AcqRel are not valid memory "
                "order variants.");

            T value;
            Base::Load(&value, &m_Value, order);

            return value;
        }

        inline T Exchange(T value, MemoryOrder order = MemoryOrder::SeqCst) noexcept
        {
            T ret;
            Base::Exchange(AddressOf(m_Value), AddressOf(value), AddressOf(ret), order);

            return ret;
        }

    public:
        inline T FetchAdd(T value, MemoryOrder order = MemoryOrder::SeqCst) noexcept
        {
            return Base::FetchAdd(&m_Value, value, order);
        }

        inline T FetchSub(T value, MemoryOrder order = MemoryOrder::SeqCst) noexcept
        {
            return Base::FetchSub(&m_Value, value, order);
        }

        inline T FetchAnd(T value, MemoryOrder order = MemoryOrder::SeqCst) noexcept
        {
            return Base::FetchAnd(&m_Value, value, order);
        }

        inline T FetchXor(T value, MemoryOrder order = MemoryOrder::SeqCst) noexcept
        {
            return Base::FetchXor(&m_Value, value, order);
        }

        inline T FetchOr(T value, MemoryOrder order = MemoryOrder::SeqCst) noexcept
        {
            return Base::FetchOr(&m_Value, value, order);
        }

        inline T FetchNand(T value, MemoryOrder order = MemoryOrder::SeqCst) noexcept
        {
            return Base::FetchNand(&m_Value, value, order);
        }

    private:
        T m_Value;
    };

    using AtomicBool = Atomic<bool>;
    using AtomicSchar = Atomic<signed char>;
    using AtomicUchar = Atomic<unsigned char>;

    using AtomicShort = Atomic<short>;
    using AtomicUshort = Atomic<unsigned short>;
    using AtomicInt = Atomic<int>;
    using AtomicUint = Atomic<unsigned int>;

    using AtomicLong = Atomic<long>;
    using AtomicUlong = Atomic<unsigned long>;
    using AtomicLlong = Atomic<long long>;
    using AtomicUllong = Atomic<unsigned long long>;

    using AtomicChar = Atomic<char>;
    using AtomicWchar = Atomic<wchar_t>;
    using AtomicChar8 = Atomic<char8_t>;
    using AtomicChar16 = Atomic<char16_t>;
    using AtomicChar32 = Atomic<char32_t>;

    using AtomicInt8 = Atomic<Int8>;
    using AtomicInt16 = Atomic<Int16>;
    using AtomicInt32 = Atomic<Int32>;
    using AtomicInt64 = Atomic<Int64>;

    using AtomicUint8 = Atomic<Uint8>;
    using AtomicUint16 = Atomic<Uint16>;
    using AtomicUint32 = Atomic<Uint32>;
    using AtomicUint64 = Atomic<Uint64>;

    using AtomicUsize = Atomic<Usize>;
    using AtomicSsize = Atomic<Ssize>;
    using AtomicPtrdiff = Atomic<Ptrdiff>;

    using AtomicIntptr = Atomic<Intptr>;
    using AtomicUintptr = Atomic<Uintptr>;
}
