#include "Foundation/Diagnostics/Assert.h"

#include <cstdio>
#include <cinttypes>

#include "Foundation/Logging/Logging.h"

namespace Kitsune::Internal
{
    inline void FallbackLogAssertionMessage(const char* expression, const char* message,
                                            const SourceLocation& loc)
    {
        // The global logger might not have been set just yet, so just print the
        // assertion message into stdout.
        std::printf("Assertion `%s` has failed.\n"
                    "`%s` [In function %s, file %s:%" PRIu32 "]\n",
                    expression, message, loc.FunctionName(), loc.FileName(), loc.Line());
    }

    bool HandleAssertionFailure(const char* expression, const char* message,
                                SourceLocation loc)
    {
        if (Logging::GetGlobalLogger() == nullptr)
            FallbackLogAssertionMessage(expression, message, loc);
        else
        {
            Logging::Fatal("Assertion `{0}` has failed.\n"
                           "`{1}` [In function {2}, file {3}:{4}]",
                           expression, message, loc.FunctionName(), loc.FileName(), loc.Line());
        }

        // TODO: This will be used when the message box is implemented.
        return true;
    }
}
