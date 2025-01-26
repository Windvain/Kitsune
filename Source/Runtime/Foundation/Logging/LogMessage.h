#pragma once

#include "Foundation/String/StringView.h"
#include "Foundation/Logging/LogSeverity.h"

namespace Kitsune
{
    struct LogMessage
    {
        StringView Message;
        LogSeverity Severity;

        StringView LoggerName;
    };
}
