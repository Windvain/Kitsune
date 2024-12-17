#pragma once

#include <climits>
#include <type_traits>

#include "Foundation/Common/Predefined.h"

namespace Kitsune
{
    using Int8 = signed char;
    using Uint8 = unsigned char;

#if USHRT_MAX == 0xffff
    using Int16 = signed short;
    using Uint16 = unsigned short;
#else
    #error Failed to determine a type for Int16 and Uint16.
#endif

#if UINT_MAX == 0xffffffff
    using Int32 = signed int;
    using Uint32 = unsigned int;
#elif ULONG_MAX == 0xffffffff
    using Int32 = signed long;
    using Uint32 = unsigned long;
#elif USHRT_MAX == 0xffffffff
    using Int32 = signed short;
    using Uint32 = unsigned short;
#else
    #error Failed to determine a type for Int32 and Uint32.
#endif

#if ULONG_MAX == 0xffffffffffffffff
    using Int64 = signed long;
    using Uint64 = unsigned long;
#elif ULLONG_MAX == 0xffffffffffffffff
    using Int64 = signed long long;
    using Uint64 = unsigned long long;
#elif UINT_MAX == 0xffffffffffffffff
    using Int64 = signed int;
    using Uint64 = unsigned int;
#else
    #error Failed to determine a type for Int64 and Uint64.
#endif

    using Usize = std::size_t;
    using Ssize = std::make_unsigned_t<Usize>;

    using Index = Usize;
    using Ptrdiff = std::ptrdiff_t;

#if KITSUNE_POINTER_SIZE == 8
    using Intptr = Int64;
    using Uintptr = Uint64;
#elif KITSUNE_POINTER_SIZE == 4
    using Intptr = Int32;
    using Uintptr = Uint32;
#else
    #error Failed to determine a type for Intptr and Uintptr.
#endif

#if defined(KITSUNE_OS_WINDOWS) && (defined(_UNICODE) || defined(UNICODE))
    using NativeChar = wchar_t;
#else
    using NativeChar = char;
#endif
}
