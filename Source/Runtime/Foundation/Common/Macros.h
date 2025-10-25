#pragma once

#include "Foundation/Common/Predefined.h"

// Helper preprocessor function macros.
#define KITSUNE_STRINGIFY_HELPER_(x) #x
#define KITSUNE_STRINGIFY(x)         KITSUNE_STRINGIFY_HELPER_(x)

#define KITSUNE_CONCAT_HELPER_(x, y) x##y
#define KITSUNE_CONCAT(x, y)         KITSUNE_CONCAT_HELPER_(x, y)

// Compile time checks.
#if defined(__has_builtin)
    #define KITSUNE_HAS_BUILTIN(builtin) __has_builtin(builtin)
#else
    #define KITSUNE_HAS_BUILTIN(builtin) (false)
#endif

#if defined(__has_attribute)
    #define KITSUNE_HAS_GNU_ATTRIBUTE(attrib) __has_attribute(attrib)
#else
    #define KITSUNE_HAS_GNU_ATTRIBUTE(attrib) (false)
#endif

#define KITSUNE_HAS_ATTRIBUTE(attrib) __has_cpp_attribute(attrib)
#define KITSUNE_HAS_INCLUDE(file) __has_include(file)

// Compiler-specific attributes.
#if KITSUNE_HAS_GNU_ATTRIBUTE(noinline) && defined(KITSUNE_COMPILER_CLANG)
    #define KITSUNE_NOINLINE [[clang::noinline]]
#elif KITSUNE_HAS_GNU_ATTRIBUTE(noinline) && defined(KITSUNE_COMPILER_GCC)
    #define KITSUNE_NOINLINE __attribute__((noinline))
#else
    #define KITSUNE_NOINLINE
#endif

#if KITSUNE_HAS_GNU_ATTRIBUTE(always_inline)
    #if defined(KITSUNE_COMPILER_CLANG)
        #define KITSUNE_FORCEINLINE [[clang::always_inline]] inline
    #else
        #define KITSUNE_FORCEINLINE inline __attribute__((always_inline))
    #endif
#elif defined(KITSUNE_COMPILER_MSVC)
    #define KITSUNE_FORCEINLINE __forceinline
#else
    #define KITSUNE_FORCEINLINE inline
#endif

#if defined(KITSUNE_COMPILER_MSVC)
    #define KITSUNE_MAYBE_OVERLAPPING [[msvc::no_unique_address]]
#else
    #define KITSUNE_MAYBE_OVERLAPPING [[no_unique_address]]
#endif

#if __cplusplus >= 202302L
    #define KITSUNE_ASSUME(expression) [[assume(expression)]]
#elif defined(KITSUNE_COMPILER_MSVC)
    #define KITSUNE_ASSUME(expression) __assume(expression)
#elif defined(KITSUNE_COMPILER_CLANG)
    #define KITSUNE_ASSUME(expression) __builtin_assume(expression)
#else
    #define KITSUNE_ASSUME(expression)
#endif

// Compiler-specific debugging utilities.
#if defined(KITSUNE_COMPILER_MSVC)
    #define KITSUNE_DEBUGBREAK() __debugbreak()
#elif KITSUNE_HAS_BUILTIN(__builtin_trap)
    #define KITSUNE_DEBUGBREAK() __builtin_trap()
#else
    #include <csignal>
    #if defined(SIGTRAP)
        #define KITSUNE_DEBUGBREAK() ::std::raise(SIGTRAP)
    #else
        #define KITSUNE_DEBUGBREAK() ::std::raise(SIGABRT)
    #endif
#endif

#if KITSUNE_HAS_BUILTIN(__builtin_unreachable)
    #define KITSUNE_UNREACHABLE() __builtin_unreachable()
#elif defined(KITSUNE_COMPILER_MSVC)
    #define KITSUNE_UNREACHABLE() __assume(false)
#else
    #define KITSUNE_UNREACHABLE()
#endif

// Compile time string literal conversions.
#if defined(KITSUNE_OS_WINDOWS)
    #define KITSUNE_NATIVE_TEXT(text) L##text
#else
    #define KITSUNE_NATIVE_TEXT(text) text
#endif

// Compiler-specific warning disabling/enabling.
#if defined(KITSUNE_COMPILER_MSVC)
    #define KITSUNE_PUSH_COMPILER_WARNINGS() __pragma(warning(push))
    #define KITSUNE_POP_COMPILER_WARNINGS()  __pragma(warning(pop))

    #define KITSUNE_IGNORE_MSVC_WARNING(code) __pragma(warning(disable: code))
#else
    #define KITSUNE_IGNORE_MSVC_WARNING(code)
#endif

#if defined(KITSUNE_COMPILER_CLANG)
    #define KITSUNE_PUSH_COMPILER_WARNINGS() _Pragma("clang diagnostic push")
    #define KITSUNE_POP_COMPILER_WARNINGS() _Pragma("clang diagnostic pop")

    #define KITSUNE_IGNORE_CLANG_WARNING_HELPER_(param) _Pragma(#param)
    #define KITSUNE_IGNORE_CLANG_WARNING(code) KITSUNE_IGNORE_CLANG_WARNING_HELPER_(clang diagnostic ignored #code)
#else
    #define KITSUNE_IGNORE_CLANG_WARNING(code)
#endif

// Basic maths function macros.
#define KITSUNE_MIN(x, y) (((x) < (y)) ? (x) : (y))
#define KITSUNE_MAX(x, y) (((x) > (y)) ? (x) : (y))

#define KITSUNE_SIGN(x) (((x) == 0) ? 0 : (((x) > 0) ? 1 : -1))

// Miscellaneous macros.
#if defined(KITSUNE_COMPILER_GCC) || defined(KITSUNE_COMPILER_CLANG)
    #define KITSUNE_CURRENT_FUNCTION __PRETTY_FUNCTION__
#elif defined(KITSUNE_COMPILER_MSVC)
    #define KITSUNE_CURRENT_FUNCTION __FUNCSIG__
#else
    #define KITSUNE_CURRENT_FUNCTION __func__
#endif

#define KITSUNE_UNUSED(x) ((void)x)
#define KITSUNE_ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))
