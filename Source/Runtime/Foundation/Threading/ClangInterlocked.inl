#pragma once

#include "Foundation/Common/Types.h"
#include "Foundation/Common/Macros.h"

#include "Foundation/Common/Predefined.h"

namespace Kitsune
{
    Int8 Interlocked::Add(volatile Int8* dest, Int8 value)
    {
        return __atomic_fetch_add(dest, value, __ATOMIC_SEQ_CST);
    }

    Int16 Interlocked::Add(volatile Int16* dest, Int16 value)
    {
        return __atomic_fetch_add(dest, value, __ATOMIC_SEQ_CST);
    }

    Int32 Interlocked::Add(volatile Int32* dest, Int32 value)
    {
        return __atomic_fetch_add(dest, value, __ATOMIC_SEQ_CST);
    }

    Int64 Interlocked::Add(volatile Int64* dest, Int64 value)
    {
        return __atomic_fetch_add(dest, value, __ATOMIC_SEQ_CST);
    }

    Int8 Interlocked::Increment(volatile Int8* dest)
    {
        return __atomic_add_fetch(dest, 1, __ATOMIC_SEQ_CST);
    }

    Int16 Interlocked::Increment(volatile Int16* dest)
    {
        return __atomic_add_fetch(dest, 1, __ATOMIC_SEQ_CST);
    }

    Int32 Interlocked::Increment(volatile Int32* dest)
    {
        return __atomic_add_fetch(dest, 1, __ATOMIC_SEQ_CST);
    }

    Int64 Interlocked::Increment(volatile Int64* dest)
    {
        return __atomic_add_fetch(dest, 1, __ATOMIC_SEQ_CST);
    }

    Int8 Interlocked::Decrement(volatile Int8* dest)
    {
        return __atomic_add_fetch(dest, -1, __ATOMIC_SEQ_CST);
    }

    Int16 Interlocked::Decrement(volatile Int16* dest)
    {
        return __atomic_add_fetch(dest, -1, __ATOMIC_SEQ_CST);
    }

    Int32 Interlocked::Decrement(volatile Int32* dest)
    {
        return __atomic_add_fetch(dest, -1, __ATOMIC_SEQ_CST);
    }

    Int64 Interlocked::Decrement(volatile Int64* dest)
    {
        return __atomic_add_fetch(dest, -1, __ATOMIC_SEQ_CST);
    }

    Int8 Interlocked::And(volatile Int8* dest, Int8 value)
    {
        return __atomic_fetch_and(dest, value, __ATOMIC_SEQ_CST);
    }

    Int16 Interlocked::And(volatile Int16* dest, Int16 value)
    {
        return __atomic_fetch_and(dest, value, __ATOMIC_SEQ_CST);
    }

    Int32 Interlocked::And(volatile Int32* dest, Int32 value)
    {
        return __atomic_fetch_and(dest, value, __ATOMIC_SEQ_CST);
    }

    Int64 Interlocked::And(volatile Int64* dest, Int64 value)
    {
        return __atomic_fetch_and(dest, value, __ATOMIC_SEQ_CST);
    }

    Int8 Interlocked::Or(volatile Int8* dest, Int8 value)
    {
        return __atomic_fetch_or(dest, value, __ATOMIC_SEQ_CST);
    }

    Int16 Interlocked::Or(volatile Int16* dest, Int16 value)
    {
        return __atomic_fetch_or(dest, value, __ATOMIC_SEQ_CST);
    }

    Int32 Interlocked::Or(volatile Int32* dest, Int32 value)
    {
        return __atomic_fetch_or(dest, value, __ATOMIC_SEQ_CST);
    }

    Int64 Interlocked::Or(volatile Int64* dest, Int64 value)
    {
        return __atomic_fetch_or(dest, value, __ATOMIC_SEQ_CST);
    }

    Int8 Interlocked::Xor(volatile Int8* dest, Int8 value)
    {
        return __atomic_fetch_xor(dest, value, __ATOMIC_SEQ_CST);
    }

    Int16 Interlocked::Xor(volatile Int16* dest, Int16 value)
    {
        return __atomic_fetch_xor(dest, value, __ATOMIC_SEQ_CST);
    }

    Int32 Interlocked::Xor(volatile Int32* dest, Int32 value)
    {
        return __atomic_fetch_xor(dest, value, __ATOMIC_SEQ_CST);
    }

    Int64 Interlocked::Xor(volatile Int64* dest, Int64 value)
    {
        return __atomic_fetch_xor(dest, value, __ATOMIC_SEQ_CST);
    }

    Int8 Interlocked::Load(volatile const Int8* ptr)
    {
        return __atomic_load_n(ptr, __ATOMIC_SEQ_CST);
    }

    Int16 Interlocked::Load(volatile const Int16* ptr)
    {
        return __atomic_load_n(ptr, __ATOMIC_SEQ_CST);
    }

    Int32 Interlocked::Load(volatile const Int32* ptr)
    {
        return __atomic_load_n(ptr, __ATOMIC_SEQ_CST);
    }

    Int64 Interlocked::Load(volatile const Int64* ptr)
    {
        return __atomic_load_n(ptr, __ATOMIC_SEQ_CST);
    }

    void Interlocked::Store(volatile Int8* ptr, Int8 value)
    {
        __atomic_store_n(ptr, value, __ATOMIC_SEQ_CST);
    }

    void Interlocked::Store(volatile Int16* ptr, Int16 value)
    {
        __atomic_store_n(ptr, value, __ATOMIC_SEQ_CST);
    }

    void Interlocked::Store(volatile Int32* ptr, Int32 value)
    {
        __atomic_store_n(ptr, value, __ATOMIC_SEQ_CST);
    }

    void Interlocked::Store(volatile Int64* ptr, Int64 value)
    {
        __atomic_store_n(ptr, value, __ATOMIC_SEQ_CST);
    }
}
