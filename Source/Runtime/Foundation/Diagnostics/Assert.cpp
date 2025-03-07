#include "Foundation/Diagnostics/Assert.h"

#include <cstdio>
#include <cinttypes>

#include "Foundation/Logging/Logging.h"
#include "Foundation/Diagnostics/MessageBox.h"

namespace Kitsune::Internal
{
    constexpr int MessageBoxAbortId = 1;
    constexpr int MessageBoxIgnoreId = 2;

    inline void FallbackLogAssertionMessage(const char* expression, const char* message,
                                            const SourceLocation& loc)
    {
        // The global logger might not have been set just yet, so just print the
        // assertion message into stdout.
        std::printf("Assertion `%s` has failed.\n"
                    "`%s` [In function %s, file %s:%" PRIu32 "]\n",
                    expression, message, loc.FunctionName(), loc.FileName(), loc.Line());
    }

    MessageBoxButtonId ShowAssertMessageBox(const char* expression, const char* message,
                                            const SourceLocation& loc)
    {
        String description = Format(
            "Assertion failed!\n\nFile: {0}\nLine: {1}\nFunction: {2}\n\n"
            "Expression: {3}\nMessage: {4}",
            loc.FileName(), loc.Line(), loc.FunctionName(), expression, message);

        MessageBoxSpecs msgBoxSpecs;
        msgBoxSpecs.Title = "Kitsune Engine";
        msgBoxSpecs.Description = description;
        msgBoxSpecs.Icon = MessageBoxIcon::Error;

        msgBoxSpecs.Buttons = {
            { .Id = MessageBoxAbortId,  .Text = "Abort" },
            { .Id = MessageBoxIgnoreId, .Text = "Ignore" }
        };

        MessageBoxButtonId buttonPressed;
        ShowMessageBox(msgBoxSpecs, &buttonPressed);

        return buttonPressed;
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

        return (ShowAssertMessageBox(expression, message, loc) == MessageBoxAbortId);
    }
}
