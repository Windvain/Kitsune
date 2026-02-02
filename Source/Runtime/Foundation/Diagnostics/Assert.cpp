#include "Foundation/Diagnostics/Assert.h"

#include <cstdio>
#include <cinttypes>

#include "Foundation/String/Format.h"
#include "Foundation/Logging/GlobalLog.h"

#include "Foundation/Diagnostics/MessageBox.h"
#include "Launch/EngineLoop.h"

namespace Kitsune::Details
{
    static const int MessageBoxAbortId = 1;
    static const int MessageBoxIgnoreId = 2;

    inline void FallbackLogAssertionMessage(const char* expression,
                                            const char* message,
                                            const SourceLocation& location)
    {
        // The global logger might not have been set just yet, so just print the
        // assertion message into stdout.
        std::printf("Assertion `%s` has failed.\n"
                    "`%s` [In function %s, file %s:%" PRIu32 "]\n",
                    expression, message,
                    location.FunctionName().Data(),
                    location.FileName().Data(),
                    location.Line());
    }

    MessageBoxButtonId ShowAssertMessageBox(const char* expression,
                                            const char* message,
                                            const SourceLocation& location)
    {
        String description = Format(
            "Assertion failed!\n\nFile: {0}\nLine: {1}\nFunction: {2}\n\n"
            "Expression: {3}\nMessage: {4}",
            location.FileName(), location.Line(), location.FunctionName(),
            expression, message);

        MessageBoxSpecifications msgBoxSpecs;
        msgBoxSpecs.Title = "Kitsune Engine";
        msgBoxSpecs.Description = description;
        msgBoxSpecs.Icon = MessageBoxIcon::Error;

        msgBoxSpecs.Buttons = {
            { .Id = MessageBoxAbortId,  .Text = "Abort" },
            { .Id = MessageBoxIgnoreId, .Text = "Ignore" }
        };

        MessageBoxButtonId buttonPressed;
        bool success = ShowMessageBox(msgBoxSpecs, &buttonPressed);

        return success ? buttonPressed : MessageBoxAbortId;
    }

    bool HandleAssertionFailure(const char* expression, const char* message,
                                SourceLocation location)
    {
        if (expression == nullptr) expression = "";
        if (message == nullptr)    message = "";

        auto* engineLoop = EngineLoop::GetInstance();
        if (!engineLoop || (engineLoop->GetLoggers().IsEmpty()))
            FallbackLogAssertionMessage(expression, message, location);
        else
        {
            KITSUNE_ENGINE_FATAL_FORMAT_(
                "Assertion `{0}` has failed.\n`{1}`",
                expression, message);
        }

        return (ShowAssertMessageBox(expression, message, location) == MessageBoxAbortId);
    }
}
