#pragma once

#include <intrin.h>

#include "Foundation/Common/Types.h"
#include "Foundation/Common/Predefined.h"

#if defined(KITSUNE_ARCH_X86_32)
    #error x86_32 is not supported due to 64-bit intrinsic requirements not being met.
#endif

namespace Kitsune
{
    Int8 Interlocked::Add(volatile Int8* dest, Int8 value)
    {
        return (Int8)::_InterlockedExchangeAdd8((volatile char*)dest, (char)value);
    }

    Int16 Interlocked::Add(volatile Int16* dest, Int16 value)
    {
        return (Int16)::_InterlockedExchangeAdd16((volatile short*)dest, (short)value);
    }

    Int32 Interlocked::Add(volatile Int32* dest, Int32 value)
    {
        return (Int32)::_InterlockedExchangeAdd((volatile long*)dest, (long)value);
    }

    Int64 Interlocked::Add(volatile Int64* dest, Int64 value)
    {
        return (Int64)::_InterlockedExchangeAdd64((volatile __int64*)dest, (__int64)value);
    }

    Int8 Interlocked::Increment(volatile Int8* dest)
    {
        // _InterlockedIncrement8() doesn't exist.
        return (Interlocked::Add(dest, 1) + 1);
    }

    Int16 Interlocked::Increment(volatile Int16* dest)
    {
        return (Int16)::_InterlockedIncrement16((volatile short*)dest);
    }

    Int32 Interlocked::Increment(volatile Int32* dest)
    {
        return (Int32)::_InterlockedIncrement((volatile long*)dest);
    }

    Int64 Interlocked::Increment(volatile Int64* dest)
    {
        return (Int64)::_InterlockedIncrement64((volatile Int64*)dest);
    }

    Int8 Interlocked::Decrement(volatile Int8* dest)
    {
        // _InterlockedDecrement8() doesn't exist.
        return (Interlocked::Add(dest, -1) - 1);
    }

    Int16 Interlocked::Decrement(volatile Int16* dest)
    {
        return (Int16)::_InterlockedDecrement16((volatile short*)dest);
    }

    Int32 Interlocked::Decrement(volatile Int32* dest)
    {
        return (Int32)::_InterlockedDecrement((volatile long*)dest);
    }

    Int64 Interlocked::Decrement(volatile Int64* dest)
    {
        return (Int64)::_InterlockedDecrement64((volatile __int64*)dest);
    }

    Int8 Interlocked::And(volatile Int8* dest, Int8 value)
    {
        return (Int8)::_InterlockedAnd8((volatile char*)dest, (char)value);
    }

    Int16 Interlocked::And(volatile Int16* dest, Int16 value)
    {
        return (Int16)::_InterlockedAnd16((volatile short*)dest, (short)value);
    }

    Int32 Interlocked::And(volatile Int32* dest, Int32 value)
    {
        return (Int32)::_InterlockedAnd((volatile long*)dest, (long)value);
    }

    Int64 Interlocked::And(volatile Int64* dest, Int64 value)
    {
        return (Int32)::_InterlockedAnd64((volatile __int64*)dest, (__int64)value);
    }

    Int8 Interlocked::Or(volatile Int8* dest, Int8 value)
    {
        return (Int8)::_InterlockedOr8((volatile char*)dest, (char)value);
    }

    Int16 Interlocked::Or(volatile Int16* dest, Int16 value)
    {
        return (Int16)::_InterlockedOr16((volatile short*)dest, (short)value);
    }

    Int32 Interlocked::Or(volatile Int32* dest, Int32 value)
    {
        return (Int32)::_InterlockedOr((volatile long*)dest, (long)value);
    }

    Int64 Interlocked::Or(volatile Int64* dest, Int64 value)
    {
        return (Int32)::_InterlockedOr64((volatile __int64*)dest, (__int64)value);
    }

    Int8 Interlocked::Xor(volatile Int8* dest, Int8 value)
    {
        return (Int8)::_InterlockedXor8((volatile char*)dest, (char)value);
    }

    Int16 Interlocked::Xor(volatile Int16* dest, Int16 value)
    {
        return (Int16)::_InterlockedXor16((volatile short*)dest, (short)value);
    }

    Int32 Interlocked::Xor(volatile Int32* dest, Int32 value)
    {
        return (Int32)::_InterlockedXor((volatile long*)dest, (long)value);
    }

    Int64 Interlocked::Xor(volatile Int64* dest, Int64 value)
    {
        return (Int32)::_InterlockedXor64((volatile __int64*)dest, (__int64)value);
    }

    Int8 Interlocked::Load(volatile const Int8* ptr)
    {
        return (Int8)::_InterlockedCompareExchange8((char*)ptr, 0, 0);
    }

    Int16 Interlocked::Load(volatile const Int16* ptr)
    {
        return (Int16)::_InterlockedCompareExchange16((volatile short*)ptr, 0, 0);
    }

    Int32 Interlocked::Load(volatile const Int32* ptr)
    {
        return (Int32)::_InterlockedCompareExchange((volatile long*)ptr, 0, 0);
    }

    Int64 Interlocked::Load(volatile const Int64* ptr)
    {
        return (Int64)::_InterlockedCompareExchange64((volatile __int64*)ptr, 0, 0);
    }

    void Interlocked::Store(volatile Int8* ptr, Int8 value)
    {
        ::_InterlockedExchange8((char*)ptr, value);
    }

    void Interlocked::Store(volatile Int16* ptr, Int16 value)
    {
        ::_InterlockedExchange16((short*)ptr, value);
    }

    void Interlocked::Store(volatile Int32* ptr, Int32 value)
    {
        ::_InterlockedExchange((long*)ptr, value);
    }

    void Interlocked::Store(volatile Int64* ptr, Int64 value)
    {
        ::_InterlockedExchange64((long long*)ptr, value);
    }
}
