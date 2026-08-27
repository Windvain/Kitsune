#pragma once

#include "Foundation/Common/Types.h"

#if defined(KITSUNE_COMPILER_CLANG)
    #include "Foundation/Clang/ClangAtomic.h"
#elif defined(KITSUNE_COMPILER_MSVC)
    #include "Foundation/MSVC/MSVCAtomic.h"
#else
    #error No implementations for atomic macros were found.
#endif

namespace Kitsune
{
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
