#pragma once

// Detect target operating system.
#if defined(_WIN32) || defined(_WIN64)
    #define KITSUNE_OS_WINDOWS 1

    #if defined(_WIN64)
        #define KITSUNE_OS_WINDOWS_64BIT 1
    #else
        #define KITSUNE_OS_WINDOWS_32BIT 1
    #endif

    #if defined(__MINGW32__) || defined(__MINGW64__)
        #define KITSUNE_OS_WINDOWS_MINGW 1
    #endif

#elif defined(__linux__)
    #define KITSUNE_OS_LINUX 1
#else
    #error Failed to determine operating system.
#endif

// Used for determining other compilers. These compilers define
// __GNUC__ or __clang__ on some platforms. Don't expect support for these
// compilers btw.
#if defined(__INTEL_COMPILER) || defined(__ICC) || defined(_ICL)
    #define KITSUNE_COMPILER_INTEL 1
#elif defined(__ibmxl__)
    #define KITSUNE_COMPILER_XLC 1
#endif

// Detect used compiler.
#if defined(__GNUC__) && !defined(KITSUNE_COMPILER_INTEL) && \
    !defined(__clang__)

    #define KITSUNE_COMPILER_GCC 1

    #define KITSUNE_COMPILER_VERSION_MAJOR __GNUC__
    #define KITSUNE_COMPILER_VERSION_MINOR __GNUC_MINOR__
    #define KITSUNE_COMPILER_VERSION_PATCH __GNUC_PATCHLEVEL__

#elif defined(__clang__) && !defined(KITSUNE_COMPILER_XLC)
    #define KITSUNE_COMPILER_CLANG 1

    #define KITSUNE_COMPILER_VERSION_MAJOR __clang_major__
    #define KITSUNE_COMPILER_VERSION_MINOR __clang_minor__
    #define KITSUNE_COMPILER_VERSION_PATCH __clang_patchlevel__

#elif defined(_MSC_VER) && !defined(KITSUNE_COMPILER_INTEL) && !defined(__clang__)
    #define KITSUNE_COMPILER_MSVC 1

    // Only applies to VC++ 8.0 (Visual C++ 2005) and above.
    #define KITSUNE_COMPILER_VERSION_MAJOR (_MSC_FULL_VER / 10'000'000)
    #define KITSUNE_COMPILER_VERSION_MINOR ((_MSC_FULL_VER % 10'000'000) / 10'000)
    #define KITSUNE_COMPILER_VERSION_PATCH (_MSC_FULL_VER % 100'000)
#else
    #error Failed to determine compiler.
#endif

// Detect target architecture.
#if defined(__arm__) || defined(_M_ARM)
    #define KITSUNE_ARCH_ARM     1
    #define KITSUNE_ARCH_AARCH32 1
#elif defined(__aarch64__) || defined(_M_ARM64)
    #define KITSUNE_ARCH_ARM     1
    #define KITSUNE_ARCH_AARCH64 1
#elif defined(__i386__) || defined(__i486__) || defined(__i586__) || \
    defined(__i686__) || defined(_M_IX86)
    #define KITSUNE_ARCH_X86    1
    #define KITSUNE_ARCH_X86_32 1
#elif defined(__x86_64__) || defined(__amd64__) || defined(_M_X64) || \
    defined(_M_AMD64)
    #define KITSUNE_ARCH_X86    1
    #define KITSUNE_ARCH_X86_64 1
#else
    #error Failed to determine target architecture.
#endif

// Determine pointer size.
#if defined(UINTPTR_MAX)
    #define KITSUNE_POINTER_SIZE (UINTPTR_MAX / 255 % 255)
#elif defined(__UINTPTR_MAX__)
    #define KITSUNE_POINTER_SIZE (__UINTPTR_MAX__ / 255 % 255)

#elif defined(KITSUNE_OS_WINDOWS_64BIT) || defined(KITSUNE_ARCH_AARCH64) || \
      defined(KITSUNE_ARCH_X86_64)
    #define KITSUNE_POINTER_MAX_ 0xffffffffffffffff
    #define KITSUNE_POINTER_SIZE (KITSUNE_POINTER_MAX_ / 255 % 255)

#elif defined(KITSUNE_OS_WINDOWS_32BIT) || defined(KITSUNE_ARCH_AARCH32) || \
      defined(KITSUNE_ARCH_X86_32)
    #define KITSUNE_POINTER_MAX_ 0xffffffff
    #define KITSUNE_POINTER_SIZE (KITSUNE_POINTER_MAX_ / 255 % 255)

#else
    #error Failed to determine pointer size.
#endif
