#pragma once

#include "Foundation/String/Format.h"
#include "Foundation/Logging/LogPayload.h"

namespace Kitsune::Details
{
    void LogGlobal(LogSeverity severity, SourceLocation loc, const StringView str);

    template<typename... Args>
    inline void LogGlobalFormat(LogSeverity severity, SourceLocation loc, const StringView fmt, Args&&... args)
    {
        String formatted =  Format(fmt, Forward<Args>(args)...);
        LogGlobal(severity, loc, formatted);
    }
}

#if !defined(KITSUNE_BUILD_PRODUCTION)
    #define KITSUNE_LOG_LEVEL_(severity, message, source) \
        ::Kitsune::Details::LogGlobal(severity, source, message)

    #define KITSUNE_LOG_FORMAT_LEVEL_(severity, message, source, ...) \
        ::Kitsune::Details::LogGlobalFormat(severity, source, message, __VA_ARGS__)
#else
    #define KITSUNE_LOG_LEVEL_(severity, message, source)
    #define KITSUNE_LOG_FORMAT_LEVEL_(severity, message, source, ...)
#endif

#define KITSUNE_TRACE(message)       KITSUNE_LOG_LEVEL_(::Kitsune::LogSeverity::Trace, message, SourceLocation());
#define KITSUNE_INFO(message)        KITSUNE_LOG_LEVEL_(::Kitsune::LogSeverity::Info, message, SourceLocation());
#define KITSUNE_WARN(message)        KITSUNE_LOG_LEVEL_(::Kitsune::LogSeverity::Warn, message, SourceLocation());
#define KITSUNE_ERROR(message)       KITSUNE_LOG_LEVEL_(::Kitsune::LogSeverity::Error, message, SourceLocation());
#define KITSUNE_FATAL(message)       KITSUNE_LOG_LEVEL_(::Kitsune::LogSeverity::Fatal, message, SourceLocation());

#define KITSUNE_TRACE_FORMAT(message, ...) KITSUNE_LOG_FORMAT_LEVEL_(::Kitsune::LogSeverity::Trace, message, SourceLocation(), __VA_ARGS__);
#define KITSUNE_INFO_FORMAT(message, ...)  KITSUNE_LOG_FORMAT_LEVEL_(::Kitsune::LogSeverity::Info, message, SourceLocation(), __VA_ARGS__);
#define KITSUNE_WARN_FORMAT(message, ...)  KITSUNE_LOG_FORMAT_LEVEL_(::Kitsune::LogSeverity::Warn, message, SourceLocation(), __VA_ARGS__);
#define KITSUNE_ERROR_FORMAT(message, ...) KITSUNE_LOG_FORMAT_LEVEL_(::Kitsune::LogSeverity::Error, message, SourceLocation(), __VA_ARGS__);
#define KITSUNE_FATAL_FORMAT(message, ...) KITSUNE_LOG_FORMAT_LEVEL_(::Kitsune::LogSeverity::Fatal, message, SourceLocation(), __VA_ARGS__);
