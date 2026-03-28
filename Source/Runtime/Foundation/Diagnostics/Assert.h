#pragma once

#include "Foundation/Common/Macros.h"
#include "Foundation/Diagnostics/SourceLocation.h"

namespace Kitsune::Details
{
    bool HandleAssertionFailure(
        const char* expression, const char* message,
        SourceLocation location = SourceLocation::Current());
}

#if !defined(KITSUNE_BUILD_PRODUCTION)
    #define KITSUNE_ALWAYS_ASSERT_(expr, message)                           \
        do                                                                  \
        {                                                                   \
            if (!(expr) &&                                                  \
                ::Kitsune::Details::HandleAssertionFailure(#expr, message)) \
            {                                                               \
                KITSUNE_DEBUGBREAK();                                       \
            }                                                               \
        }                                                                   \
        while (false)

    // Make sure that the expression `expr` is always true. If it is not
    // true, then the specified message is logged, and the application
    // will break if a debugger is connected.
    #define KITSUNE_ASSERT KITSUNE_ALWAYS_ASSERT_

    // Same thing as `KITSUNE_ASSERT`, but when building in Production
    // mode, this macro will still run the expression passed in. Use this
    // instead of `KITSUNE_ASSERT` in cases where the expression has an impact
    // on the program's control flow.
    #define KITSUNE_VERIFY KITSUNE_ALWAYS_ASSERT_
#else
    #define KITSUNE_ASSERT(expr, message) ((void)0)
    #define KITSUNE_VERIFY(expr, message) if (expr) { /* ... */ }
#endif
