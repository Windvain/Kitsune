#pragma once

#include "Foundation/Common/Macros.h"
#include "Foundation/Diagnostics/SourceLocation.h"

namespace Kitsune::Internal
{
    KITSUNE_API_ bool HandleAssertionFailure(const char* expression, const char* message,
                                             SourceLocation loc = SourceLocation::Current());
}

#define KITSUNE_ALWAYS_ASSERT(expr, message)                                        \
    do                                                                              \
    {                                                                               \
        if (!(expr) && ::Kitsune::Internal::HandleAssertionFailure(#expr, message)) \
        {                                                                           \
            KITSUNE_DEBUGBREAK();                                                   \
        }                                                                           \
    } while (false)

#if !defined(KITSUNE_BUILD_PRODUCTION)
    #define KITSUNE_ASSERT KITSUNE_ALWAYS_ASSERT
    #define KITSUNE_VERIFY KITSUNE_ALWAYS_ASSERT
#else
    #define KITSUNE_ASSERT(expr, message) ((void)0)
    #define KITSUNE_VERIFY(expr, message) if (expr) {}
#endif

