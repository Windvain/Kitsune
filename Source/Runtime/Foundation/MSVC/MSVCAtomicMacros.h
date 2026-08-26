#pragma once

#include <intrin.h>
#include "Foundation/Common/Predefined.h"

#if !defined(KITSUNE_COMPILER_MSVC)
    #error This file should only be included when building with the MSVC compiler.
#endif

namespace Kitsune::Details
{
    template<typename T>
    KITSUNE_FORCEINLINE void InternalAtomicStore(T* dest, const T* source)
    {
        *dest = *source;
    }

    template<typename T>
    KITSUNE_FORCEINLINE void InternalAtomicLoad(T* dest, const T* source)
    {
        *dest = *source;
    }

    template<typename T>
    KITSUNE_FORCEINLINE void InternalAtomicExchange(T* pointer, const T* value, T* ret)
    {
        *ret = *pointer;
        *pointer = *value;
    }

    template<typename T>
    KITSUNE_FORCEINLINE bool InternalAtomicCompareExchange(T* pointer, T* expected, const T* desired)
    {
        bool ret = (*pointer == *expected);
        if (ret)
            *pointer = *desired;
        else
            *expected = *pointer;

        return ret;
    }

    template<typename T>
    KITSUNE_FORCEINLINE T InternalNand(T* source, T value)
    {
        T oldValue = *source;
        *source = ~(*source & value);

        return oldValue;
    }
}

#define KITSUNE_GENERAL_ATOMIC_STORE(type, order, dest, source) \
    ::Kitsune::Details::InternalAtomicStore(dest, source)

#define KITSUNE_GENERAL_ATOMIC_LOAD(type, order, dest, source) \
    ::Kitsune::Details::InternalAtomicLoad(dest, source)

#define KITSUNE_GENERAL_ATOMIC_EXCHANGE(type, order, pointer, value, ret) \
    ::Kitsune::Details::InternalAtomicExchange(pointer, value, ret)

#define KITSUNE_GENERAL_ATOMIC_COMPARE_EXCHANGE(type, order,                      \
                                                pointer, expected, desired)       \
    ::Kitsune::Details::InternalAtomicCompareExchange(pointer, expected, desired)

#define KITSUNE_ATOMIC_STORE_8(type, order, dest, source) \
    (void)::_InterlockedExchange8(reinterpret_cast<volatile char*>(dest), char(*source))

#define KITSUNE_ATOMIC_STORE_16(type, order, dest, source) \
    (void)::_InterlockedExchange16(reinterpret_cast<volatile short*>(dest), short(*source))

#define KITSUNE_ATOMIC_STORE_32(type, order, dest, source) \
    (void)::_InterlockedExchange(reinterpret_cast<volatile long*>(dest), long(*source))

#define KITSUNE_ATOMIC_STORE_64(type, order, dest, source) \
    (void)::_InterlockedExchange64(reinterpret_cast<volatile long long*>(dest), static_cast<long long>(*source))

#define KITSUNE_ATOMIC_LOAD_8(type, order, dest, source) \
    *dest = ::_InterlockedOr8((volatile char*)(source), 0)

#define KITSUNE_ATOMIC_LOAD_16(type, order, dest, source) \
    *dest = ::_InterlockedOr16((volatile short*)(source), 0)

#define KITSUNE_ATOMIC_LOAD_32(type, order, dest, source) \
    *dest = ::_InterlockedOr((volatile long*)(source), 0)

#define KITSUNE_ATOMIC_LOAD_64(type, order, dest, source) \
    *dest = ::_InterlockedOr64((volatile long long*)(source), 0)

#define KITSUNE_ATOMIC_EXCHANGE_8(type, order, pointer, value, ret) \
    *ret = ::_InterlockedExchange8(reinterpret_cast<volatile char*>(pointer), char(*value))

#define KITSUNE_ATOMIC_EXCHANGE_16(type, order, pointer, value, ret) \
    *ret = ::_InterlockedExchange16(reinterpret_cast<volatile short*>(pointer), short(*value))

#define KITSUNE_ATOMIC_EXCHANGE_32(type, order, pointer, value, ret) \
    *ret = ::_InterlockedExchange(reinterpret_cast<volatile long*>(pointer), long(*value))

#define KITSUNE_ATOMIC_EXCHANGE_64(type, order, pointer, value, ret) \
    *ret = ::_InterlockedExchange64(reinterpret_cast<volatile long long*>(pointer), static_cast<long long>(*value))

#define KITSUNE_ATOMIC_COMPARE_EXCHANGE_8(type, order,                  \
                                          pointer, expected, desired)   \
    ::Kitsune::Details::InternalAtomicCompareExchange(pointer, expected, desired)

#define KITSUNE_ATOMIC_COMPARE_EXCHANGE_16(type, order,                  \
                                          pointer, expected, desired)   \
    ::Kitsune::Details::InternalAtomicCompareExchange(pointer, expected, desired)

#define KITSUNE_ATOMIC_COMPARE_EXCHANGE_32(type, order,                  \
                                          pointer, expected, desired)   \
    ::Kitsune::Details::InternalAtomicCompareExchange(pointer, expected, desired)

#define KITSUNE_ATOMIC_COMPARE_EXCHANGE_64(type, order,                  \
                                          pointer, expected, desired)   \
    ::Kitsune::Details::InternalAtomicCompareExchange(pointer, expected, desired)

// _InterlockedAdd is only supported on ARM and ARM64 targets as per the documentation.
// https://learn.microsoft.com/en-us/cpp/intrinsics/interlockedadd-intrinsic-functions?view=msvc-170
#define KITSUNE_ATOMIC_FETCH_ADD_8(type, order, source, value) \
    ::_InterlockedExchangeAdd8(reinterpret_cast<volatile char*>(source), char(value))

#define KITSUNE_ATOMIC_FETCH_ADD_16(type, order, source, value) \
    ::_InterlockedExchangeAdd16(reinterpret_cast<volatile short*>(source), short(value))

#define KITSUNE_ATOMIC_FETCH_ADD_32(type, order, source, value) \
    ::_InterlockedExchangeAdd(reinterpret_cast<volatile long*>(source), long(value))

#define KITSUNE_ATOMIC_FETCH_ADD_64(type, order, source, value) \
    ::_InterlockedExchangeAdd64(reinterpret_cast<volatile long long*>(source), static_cast<long long>(value))

#define KITSUNE_ATOMIC_FETCH_SUB_8(type, order, source, value) \
    ::_InterlockedExchangeAdd8(reinterpret_cast<volatile char*>(source), -char(value))

#define KITSUNE_ATOMIC_FETCH_SUB_16(type, order, source, value) \
    ::_InterlockedExchangeAdd16(reinterpret_cast<volatile short*>(source), -short(value))

#define KITSUNE_ATOMIC_FETCH_SUB_32(type, order, source, value) \
    ::_InterlockedExchangeAdd(reinterpret_cast<volatile long*>(source), -long(value))

#define KITSUNE_ATOMIC_FETCH_SUB_64(type, order, source, value) \
    ::_InterlockedExchangeAdd64(reinterpret_cast<volatile long long*>(source), -static_cast<long long>(value))

#define KITSUNE_ATOMIC_FETCH_OR_8(type, order, source, value) \
    ::_InterlockedOr8(reinterpret_cast<volatile char*>(source), char(value))

#define KITSUNE_ATOMIC_FETCH_OR_16(type, order, source, value) \
    ::_InterlockedOr16(reinterpret_cast<volatile short*>(source), short(value))

#define KITSUNE_ATOMIC_FETCH_OR_32(type, order, source, value) \
    ::_InterlockedOr(reinterpret_cast<volatile long*>(source), long(value))

#define KITSUNE_ATOMIC_FETCH_OR_64(type, order, source, value) \
    ::_InterlockedOr64(reinterpret_cast<volatile long long*>(source), static_cast<long long>(value))

#define KITSUNE_ATOMIC_FETCH_AND_8(type, order, source, value) \
    ::_InterlockedAnd8(reinterpret_cast<volatile char*>(source), char(value))

#define KITSUNE_ATOMIC_FETCH_AND_16(type, order, source, value) \
    ::_InterlockedAnd16(reinterpret_cast<volatile short*>(source), short(value))

#define KITSUNE_ATOMIC_FETCH_AND_32(type, order, source, value) \
    ::_InterlockedAnd(reinterpret_cast<volatile long*>(source), long(value))

#define KITSUNE_ATOMIC_FETCH_AND_64(type, order, source, value) \
    ::_InterlockedAnd64(reinterpret_cast<volatile long long*>(source), static_cast<long long>(value))

#define KITSUNE_ATOMIC_FETCH_XOR_8(type, order, source, value) \
    ::_InterlockedXor8(reinterpret_cast<volatile char*>(source), char(value))

#define KITSUNE_ATOMIC_FETCH_XOR_16(type, order, source, value) \
    ::_InterlockedXor16(reinterpret_cast<volatile short*>(source), short(value))

#define KITSUNE_ATOMIC_FETCH_XOR_32(type, order, source, value) \
    ::_InterlockedXor(reinterpret_cast<volatile long*>(source), long(value))

#define KITSUNE_ATOMIC_FETCH_XOR_64(type, order, source, value) \
    ::_InterlockedXor64(reinterpret_cast<volatile long long*>(source), static_cast<long long>(value))

#define KITSUNE_ATOMIC_FETCH_NAND_8(type, order, source, value) \
    ::Kitsune::Details::InternalNand(reinterpret_cast<char*>(source), char(value))

#define KITSUNE_ATOMIC_FETCH_NAND_16(type, order, source, value) \
    ::Kitsune::Details::InternalNand(reinterpret_cast<short*>(source), short(value))

#define KITSUNE_ATOMIC_FETCH_NAND_32(type, order, source, value) \
    ::Kitsune::Details::InternalNand(reinterpret_cast<long*>(source), long(value))

#define KITSUNE_ATOMIC_FETCH_NAND_64(type, order, source, value) \
    ::Kitsune::Details::InternalNand(reinterpret_cast<long long*>(source), static_cast<long long>(value))
