#pragma once

// IWYU pragma: private, include "Threading/AtomicMacros.h"
#include <type_traits>                          // IWYU pragma: keep
#include "Foundation/Threading/MemoryOrder.h"

// Checks to make sure that calling the macros with MemoryOrder enumerations will
// work.
namespace
{
    using namespace Kitsune;

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
}

#define KITSUNE_GENERAL_ATOMIC_STORE(type, order, dest, source) \
    __atomic_store(dest, source, int(order))

#define KITSUNE_GENERAL_ATOMIC_LOAD(type, order, dest, source) \
    __atomic_load(source, dest, int(order))

#define KITSUNE_GENERAL_ATOMIC_EXCHANGE(type, order, pointer, value, ret) \
    __atomic_exchange(pointer, value, ret, int(order))

#define KITSUNE_GENERAL_ATOMIC_COMPARE_EXCHANGE(type, order,                      \
                                                pointer, expected, desired)       \
    __atomic_compare_exchange(pointer, expected, desired, false, int(order), int(order))

// __atomic_store_n and __atomic_load_n only works for integral or pointer types of
// sizes 1, 2, 4, or 8 bytes.
#define KITSUNE_INTERNAL_ATOMIC_STORE(type, order, dest, source)    \
    ((std::is_integral_v<type> || std::is_pointer_v<type>) &&       \
     (sizeof(type) < 8)) ?                                          \
        __atomic_store_n(dest, *(source), int(order)) :             \
        __atomic_store(dest, const_cast<type*>(source), int(order))

#define KITSUNE_INTERNAL_ATOMIC_LOAD(type, order, dest, source)               \
    if constexpr ((std::is_integral_v<type> || std::is_pointer_v<type>) &&    \
                  (sizeof(type) < 8))                                         \
    {                                                                         \
        *(dest) = __atomic_load_n(source, int(order));                        \
    }                                                                         \
    else                                                                      \
    {                                                                         \
        __atomic_load(source, dest, int(order));                              \
    }

#define KITSUNE_INTERNAL_ATOMIC_EXCHANGE(type, order, pointer, value, ret)     \
    if constexpr ((std::is_integral_v<type> || std::is_pointer_v<type>) &&     \
                  (sizeof(type) < 8))                                          \
    {                                                                          \
        *(ret) = __atomic_exchange_n(pointer, *(value), int(order));           \
    }                                                                          \
    else                                                                       \
    {                                                                          \
        __atomic_exchange(pointer, const_cast<type*>(value), ret, int(order)); \
    }

#define KITSUNE_INTERNAL_ATOMIC_COMPARE_EXCHANGE(type, order,                          \
                                                 pointer, expected, desired)           \
    ((std::is_integral_v<type> || std::is_pointer_v<type>) && (sizeof(type) < 8)) ?    \
        __atomic_compare_exchange_n(pointer, expected, *(desired), false,              \
                                    int(order), int(order)) :                          \
        __atomic_compare_exchange(pointer, expected, desired, false,                   \
                                  int(order), int(order))

#define KITSUNE_ATOMIC_STORE_8(type, order, dest, source) \
    KITSUNE_INTERNAL_ATOMIC_STORE(type, order, dest, source)

#define KITSUNE_ATOMIC_STORE_16(type, order, dest, source) \
    KITSUNE_INTERNAL_ATOMIC_STORE(type, order, dest, source)

#define KITSUNE_ATOMIC_STORE_32(type, order, dest, source) \
    KITSUNE_INTERNAL_ATOMIC_STORE(type, order, dest, source)

#define KITSUNE_ATOMIC_STORE_64(type, order, dest, source) \
    KITSUNE_INTERNAL_ATOMIC_STORE(type, order, dest, source)

#define KITSUNE_ATOMIC_LOAD_8(type, order, dest, source) \
    KITSUNE_INTERNAL_ATOMIC_LOAD(type, order, dest, source)

#define KITSUNE_ATOMIC_LOAD_16(type, order, dest, source) \
    KITSUNE_INTERNAL_ATOMIC_LOAD(type, order, dest, source)

#define KITSUNE_ATOMIC_LOAD_32(type, order, dest, source) \
    KITSUNE_INTERNAL_ATOMIC_LOAD(type, order, dest, source)

#define KITSUNE_ATOMIC_LOAD_64(type, order, dest, source) \
    KITSUNE_INTERNAL_ATOMIC_LOAD(type, order, dest, source)

#define KITSUNE_ATOMIC_EXCHANGE_8(type, order, pointer, value, ret) \
    KITSUNE_INTERNAL_ATOMIC_EXCHANGE(type, order, pointer, value, ret)

#define KITSUNE_ATOMIC_EXCHANGE_16(type, order, pointer, value, ret) \
    KITSUNE_INTERNAL_ATOMIC_EXCHANGE(type, order, pointer, value, ret)

#define KITSUNE_ATOMIC_EXCHANGE_32(type, order, pointer, value, ret) \
    KITSUNE_INTERNAL_ATOMIC_EXCHANGE(type, order, pointer, value, ret)

#define KITSUNE_ATOMIC_EXCHANGE_64(type, order, pointer, value, ret) \
    KITSUNE_INTERNAL_ATOMIC_EXCHANGE(type, order, pointer, value, ret)

#define KITSUNE_ATOMIC_COMPARE_EXCHANGE_8(type, order,                  \
                                          pointer, expected, desired)   \
    KITSUNE_INTERNAL_ATOMIC_COMPARE_EXCHANGE(type, order,        \
                                             pointer, expected, desired)

#define KITSUNE_ATOMIC_COMPARE_EXCHANGE_16(type, order,                 \
                                          pointer, expected, desired)   \
    KITSUNE_INTERNAL_ATOMIC_COMPARE_EXCHANGE(type, order,        \
                                             pointer, expected, desired)

#define KITSUNE_ATOMIC_COMPARE_EXCHANGE_32(type, order,                 \
                                          pointer, expected, desired)   \
    KITSUNE_INTERNAL_ATOMIC_COMPARE_EXCHANGE(type, order,        \
                                             pointer, expected, desired)

#define KITSUNE_ATOMIC_COMPARE_EXCHANGE_64(type, order,                 \
                                          pointer, expected, desired)   \
    KITSUNE_INTERNAL_ATOMIC_COMPARE_EXCHANGE(type, order,        \
                                             pointer, expected, desired)

// Only one general function for these arithmetic functions.
#define KITSUNE_INTERNAL_ATOMIC_FETCH_ADD(type, order, source, value) \
    __atomic_fetch_add(source, value, int(order))

#define KITSUNE_INTERNAL_ATOMIC_FETCH_SUB(type, order, source, value) \
    __atomic_fetch_sub(source, value, int(order))

#define KITSUNE_INTERNAL_ATOMIC_FETCH_AND(type, order, source, value) \
    __atomic_fetch_and(source, value, int(order))

#define KITSUNE_INTERNAL_ATOMIC_FETCH_XOR(type, order, source, value) \
    __atomic_fetch_xor(source, value, int(order))

#define KITSUNE_INTERNAL_ATOMIC_FETCH_OR(type, order, source, value) \
    __atomic_fetch_or(source, value, int(order))

#define KITSUNE_INTERNAL_ATOMIC_FETCH_NAND(type, order, source, value) \
    __atomic_fetch_nand(source, value, int(order))

#define KITSUNE_ATOMIC_FETCH_ADD_8(type, order, source, value) \
    KITSUNE_INTERNAL_ATOMIC_FETCH_ADD(type, order, source, value)

#define KITSUNE_ATOMIC_FETCH_ADD_16(type, order, source, value) \
    KITSUNE_INTERNAL_ATOMIC_FETCH_ADD(type, order, source, value)

#define KITSUNE_ATOMIC_FETCH_ADD_32(type, order, source, value) \
    KITSUNE_INTERNAL_ATOMIC_FETCH_ADD(type, order, source, value)

#define KITSUNE_ATOMIC_FETCH_ADD_64(type, order, source, value) \
    KITSUNE_INTERNAL_ATOMIC_FETCH_ADD(type, order, source, value)

#define KITSUNE_ATOMIC_FETCH_SUB_8(type, order, source, value) \
    KITSUNE_INTERNAL_ATOMIC_FETCH_SUB(type, order, source, value)

#define KITSUNE_ATOMIC_FETCH_SUB_16(type, order, source, value) \
    KITSUNE_INTERNAL_ATOMIC_FETCH_SUB(type, order, source, value)

#define KITSUNE_ATOMIC_FETCH_SUB_32(type, order, source, value) \
    KITSUNE_INTERNAL_ATOMIC_FETCH_SUB(type, order, source, value)

#define KITSUNE_ATOMIC_FETCH_SUB_64(type, order, source, value) \
    KITSUNE_INTERNAL_ATOMIC_FETCH_SUB(type, order, source, value)

#define KITSUNE_ATOMIC_FETCH_AND_8(type, order, source, value) \
    KITSUNE_INTERNAL_ATOMIC_FETCH_AND(type, order, source, value)

#define KITSUNE_ATOMIC_FETCH_AND_16(type, order, source, value) \
    KITSUNE_INTERNAL_ATOMIC_FETCH_AND(type, order, source, value)

#define KITSUNE_ATOMIC_FETCH_AND_32(type, order, source, value) \
    KITSUNE_INTERNAL_ATOMIC_FETCH_AND(type, order, source, value)

#define KITSUNE_ATOMIC_FETCH_AND_64(type, order, source, value) \
    KITSUNE_INTERNAL_ATOMIC_FETCH_AND(type, order, source, value)

#define KITSUNE_ATOMIC_FETCH_XOR_8(type, order, source, value) \
    KITSUNE_INTERNAL_ATOMIC_FETCH_XOR(type, order, source, value)

#define KITSUNE_ATOMIC_FETCH_XOR_16(type, order, source, value) \
    KITSUNE_INTERNAL_ATOMIC_FETCH_XOR(type, order, source, value)

#define KITSUNE_ATOMIC_FETCH_XOR_32(type, order, source, value) \
    KITSUNE_INTERNAL_ATOMIC_FETCH_XOR(type, order, source, value)

#define KITSUNE_ATOMIC_FETCH_XOR_64(type, order, source, value) \
    KITSUNE_INTERNAL_ATOMIC_FETCH_XOR(type, order, source, value)

#define KITSUNE_ATOMIC_FETCH_OR_8(type, order, source, value) \
    KITSUNE_INTERNAL_ATOMIC_FETCH_OR(type, order, source, value)

#define KITSUNE_ATOMIC_FETCH_OR_16(type, order, source, value) \
    KITSUNE_INTERNAL_ATOMIC_FETCH_OR(type, order, source, value)

#define KITSUNE_ATOMIC_FETCH_OR_32(type, order, source, value) \
    KITSUNE_INTERNAL_ATOMIC_FETCH_OR(type, order, source, value)

#define KITSUNE_ATOMIC_FETCH_OR_64(type, order, source, value) \
    KITSUNE_INTERNAL_ATOMIC_FETCH_OR(type, order, source, value)

#define KITSUNE_ATOMIC_FETCH_NAND_8(type, order, source, value) \
    KITSUNE_INTERNAL_ATOMIC_FETCH_NAND(type, order, source, value)

#define KITSUNE_ATOMIC_FETCH_NAND_16(type, order, source, value) \
    KITSUNE_INTERNAL_ATOMIC_FETCH_NAND(type, order, source, value)

#define KITSUNE_ATOMIC_FETCH_NAND_32(type, order, source, value) \
    KITSUNE_INTERNAL_ATOMIC_FETCH_NAND(type, order, source, value)

#define KITSUNE_ATOMIC_FETCH_NAND_64(type, order, source, value) \
    KITSUNE_INTERNAL_ATOMIC_FETCH_NAND(type, order, source, value)
