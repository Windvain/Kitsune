#pragma once

#include "Foundation/Common/Types.h"
#include "Foundation/Common/Macros.h"

#include "Foundation/Common/Predefined.h"

namespace Kitsune
{
    class Interlocked
    {
    public:
        KITSUNE_FORCEINLINE static Int8 Add(volatile Int8* dest, Int8 value);
        KITSUNE_FORCEINLINE static Int16 Add(volatile Int16* dest, Int16 value);
        KITSUNE_FORCEINLINE static Int32 Add(volatile Int32* dest, Int32 value);
        KITSUNE_FORCEINLINE static Int64 Add(volatile Int64* dest, Int64 value);

    public:
        KITSUNE_FORCEINLINE static Int8 Increment(volatile Int8* dest);
        KITSUNE_FORCEINLINE static Int16 Increment(volatile Int16* dest);
        KITSUNE_FORCEINLINE static Int32 Increment(volatile Int32* dest);
        KITSUNE_FORCEINLINE static Int64 Increment(volatile Int64* dest);

        KITSUNE_FORCEINLINE static Int8 Decrement(volatile Int8* dest);
        KITSUNE_FORCEINLINE static Int16 Decrement(volatile Int16* dest);
        KITSUNE_FORCEINLINE static Int32 Decrement(volatile Int32* dest);
        KITSUNE_FORCEINLINE static Int64 Decrement(volatile Int64* dest);

    public:
        KITSUNE_FORCEINLINE static Int8 And(volatile Int8* dest, Int8 value);
        KITSUNE_FORCEINLINE static Int16 And(volatile Int16* dest, Int16 value);
        KITSUNE_FORCEINLINE static Int32 And(volatile Int32* dest, Int32 value);
        KITSUNE_FORCEINLINE static Int64 And(volatile Int64* dest, Int64 value);

        KITSUNE_FORCEINLINE static Int8 Or(volatile Int8* dest, Int8 value);
        KITSUNE_FORCEINLINE static Int16 Or(volatile Int16* dest, Int16 value);
        KITSUNE_FORCEINLINE static Int32 Or(volatile Int32* dest, Int32 value);
        KITSUNE_FORCEINLINE static Int64 Or(volatile Int64* dest, Int64 value);

        KITSUNE_FORCEINLINE static Int8 Xor(volatile Int8* dest, Int8 value);
        KITSUNE_FORCEINLINE static Int16 Xor(volatile Int16* dest, Int16 value);
        KITSUNE_FORCEINLINE static Int32 Xor(volatile Int32* dest, Int32 value);
        KITSUNE_FORCEINLINE static Int64 Xor(volatile Int64* dest, Int64 value);

    public:
        KITSUNE_FORCEINLINE static Int8 Load(volatile const Int8* ptr);
        KITSUNE_FORCEINLINE static Int16 Load(volatile const Int16* ptr);
        KITSUNE_FORCEINLINE static Int32 Load(volatile const Int32* ptr);
        KITSUNE_FORCEINLINE static Int64 Load(volatile const Int64* ptr);

        KITSUNE_FORCEINLINE static void Store(volatile Int8* ptr, Int8 value);
        KITSUNE_FORCEINLINE static void Store(volatile Int16* ptr, Int16 value);
        KITSUNE_FORCEINLINE static void Store(volatile Int32* ptr, Int32 value);
        KITSUNE_FORCEINLINE static void Store(volatile Int64* ptr, Int64 value);
    };
}

// Try to find OS-specific implementations first.
#if defined(KITSUNE_COMPILER_MSVC)
    #include "Foundation/MSVC/MSVCInterlocked.inl"
#elif defined(KITSUNE_COMPILER_CLANG)
    #include "Foundation/Clang/ClangInterlocked.inl"
#else
    #error Could not fild implementation for interlocked functions.
#endif
