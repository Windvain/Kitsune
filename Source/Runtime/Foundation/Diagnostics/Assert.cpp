#include "Foundation/Diagnostics/Assert.h"

#include <cstdio>
#include <cinttypes>

namespace Kitsune::Internal
{
    bool HandleAssertionFailure(const char* expression, const char* message,
                                SourceLocation loc)
    {
        std::printf(
            "Assertion `%s` has failed.\n"
            "%s [In function %s, file %s:%" PRIu32 "]\n",
            expression, message, loc.FunctionName(), loc.FileName(),
            loc.Line()
        );

        // TODO: This will be used when the message box is implemented.
        return true;
    }
}
