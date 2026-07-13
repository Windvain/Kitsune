#pragma once

#include "Foundation/Common/Predefined.h"

#define KITSUNE_INTERNAL_STRINGIFY_HELPER(x) #x
#define KITSUNE_STRINGIFY(x) KITSUNE_INTERNAL_STRINGIFY_HELPER(x)

// Checks whether the compiler supports a specific builtin function.
// Defined to `__has_builtin` for compilers which support it.
#if defined(__has_builtin)
    #define KITSUNE_HAS_BUILTIN(builtin) __has_builtin(builtin)
#else
    #define KITSUNE_HAS_BUILTIN(builtin) (false)
#endif

// Checks whether the compiler supports a GNU attribute (`__attribute__((...))`).
#if defined(__has_attribute)
    #define KITSUNE_HAS_GNU_ATTRIBUTE(attrib) __has_attribute(attrib)
#else
    #define KITSUNE_HAS_GNU_ATTRIBUTE(attrib) (false)
#endif

#define KITSUNE_HAS_ATTRIBUTE(attrib) __has_cpp_attribute(attrib)
#define KITSUNE_HAS_INCLUDE(file) __has_include(file)

// Requests to the compiler to not inline a function.
#if KITSUNE_HAS_GNU_ATTRIBUTE(noinline) && defined(KITSUNE_COMPILER_CLANG)
    #define KITSUNE_NOINLINE [[clang::noinline]]
#elif KITSUNE_HAS_GNU_ATTRIBUTE(noinline) && defined(KITSUNE_COMPILER_GCC)
    #define KITSUNE_NOINLINE __attribute__((noinline))
#elif defined(KITSUNE_COMPILER_MSVC)
    #define KITSUNE_NOINLINE __declspec(noinline)
#else
    #define KITSUNE_NOINLINE
#endif

// "Forces" the compiler to inline a function.
// When compiling with Clang or GCC, inlining will always be attempted regardless of
// optimization level. On MSVC, `KITSUNE_FORCEINLINE` in some circumstances, might not
// inline a function.
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

// Marking a variable with this attribute will signal to the compiler that
// this member variable can be overlapped with other data in the parent class.
// Mainly used in implementing EBO (Empty Base Class Optimization).
#if defined(KITSUNE_COMPILER_MSVC)
    #define KITSUNE_MAYBE_OVERLAPPING [[msvc::no_unique_address]]
#else
    #define KITSUNE_MAYBE_OVERLAPPING [[no_unique_address]]
#endif

// Breaks a debugger on this line if a debugger is running; else will terminate
// the program.
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

// Marks a region of a function as "unreachable". This is used to silence compiler
// warnings and for optimizations.
#if KITSUNE_HAS_BUILTIN(__builtin_unreachable)
    #define KITSUNE_UNREACHABLE() __builtin_unreachable()
#elif defined(KITSUNE_COMPILER_MSVC)
    #define KITSUNE_UNREACHABLE() __assume(false)
#else
    #define KITSUNE_UNREACHABLE()
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

    #define KITSUNE_INTERNAL_IGNORE_CLANG_WARNING_HELPER(param) _Pragma(#param)
    #define KITSUNE_IGNORE_CLANG_WARNING(code) \
        KITSUNE_INTERNAL_IGNORE_CLANG_WARNING_HELPER(clang diagnostic ignored #code)
#else
    #define KITSUNE_IGNORE_CLANG_WARNING(code)
#endif

#if defined(KITSUNE_COMPILER_MSVC)
    #define KITSUNE_DLLEXPORT __declspec(dllexport)
    #define KITSUNE_DLLIMPORT __declspec(dllimport)
#elif defined(KITSUNE_COMPILER_CLANG) && defined(KITSUNE_OS_WINDOWS)
    #define KITSUNE_DLLEXPORT [[gnu::dllexport]]
    #define KITSUNE_DLLIMPORT [[gnu::dllimport]]
#else
    #define KITSUNE_DLLEXPORT
    #define KITSUNE_DLLIMPORT
#endif

#if defined(KITSUNE_EXPORTS)
    #define KITSUNE_API KITSUNE_DLLEXPORT
#else
    #define KITSUNE_API KITSUNE_DLLIMPORT
#endif

// Equivalent to the sign() function in mathematics. Returns 1 if the value `x` is
// positive, -1 if it is negative, and 0 if the value is equal to 0.
// This macro expands into a ternary statement, so it might not be suitable
// under certain circumstances.
#define KITSUNE_SIGN(x) (((x) == 0) ? 0 : (((x) > 0) ? 1 : -1))

// Macro for marking variables as "unused". Mainly used to shut the compiler up.
#define KITSUNE_UNUSED(x) ((void)x)

// Macro for getting the size of a C-styled array.
#define KITSUNE_ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))
