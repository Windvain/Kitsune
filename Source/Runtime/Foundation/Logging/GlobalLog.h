#pragma once

#include "Foundation/String/Format.h"
#include "Foundation/Logging/LogPayload.h"

namespace Kitsune::Details
{
    void LogGlobal(const StringView loggerName, LogSeverity severity, SourceLocation loc, const StringView str);

    template<typename... Args>
    inline void LogGlobalFormat(const StringView loggerName, LogSeverity severity, SourceLocation loc,
                                const StringView fmt, Args&&... args)
    {
        String formatted =  Format(fmt, Forward<Args>(args)...);
        LogGlobal(loggerName, severity, loc, formatted);
    }
}

#if !defined(KITSUNE_BUILD_PRODUCTION)
    #define KITSUNE_LOG_LEVEL_(loggerName, severity, message, source) \
        ::Kitsune::Details::LogGlobal(loggerName, severity, source, message)

    #define KITSUNE_LOG_FORMAT_LEVEL_(loggerName, severity, message, source, ...) \
        ::Kitsune::Details::LogGlobalFormat(loggerName, severity, source, message __VA_OPT__(,) __VA_ARGS__)
#else
    #define KITSUNE_LOG_LEVEL_(loggerName, severity, message, source)
    #define KITSUNE_LOG_FORMAT_LEVEL_(loggerName, severity, message, source, ...)
#endif

// For use in client code.
#define KITSUNE_TRACE(message) KITSUNE_TRACE_NAMED("GLOBAL", message)
#define KITSUNE_INFO(message)  KITSUNE_INFO_NAMED( "GLOBAL", message)
#define KITSUNE_WARN(message)  KITSUNE_WARN_NAMED( "GLOBAL", message)
#define KITSUNE_ERROR(message) KITSUNE_ERROR_NAMED("GLOBAL", message)
#define KITSUNE_FATAL(message) KITSUNE_FATAL_NAMED("GLOBAL", message)

#define KITSUNE_TRACE_NAMED(loggerName, message) KITSUNE_LOG_LEVEL_(loggerName, ::Kitsune::LogSeverity::Trace,   message, SourceLocation())
#define KITSUNE_INFO_NAMED(loggerName, message)  KITSUNE_LOG_LEVEL_(loggerName, ::Kitsune::LogSeverity::Info,    message, SourceLocation())
#define KITSUNE_WARN_NAMED(loggerName, message)  KITSUNE_LOG_LEVEL_(loggerName, ::Kitsune::LogSeverity::Warning, message, SourceLocation())
#define KITSUNE_ERROR_NAMED(loggerName, message) KITSUNE_LOG_LEVEL_(loggerName, ::Kitsune::LogSeverity::Error,   message, SourceLocation())
#define KITSUNE_FATAL_NAMED(loggerName, message) KITSUNE_LOG_LEVEL_(loggerName, ::Kitsune::LogSeverity::Fatal,   message, SourceLocation())

#define KITSUNE_TRACE_FORMAT(message, ...) KITSUNE_TRACE_FORMAT_NAMED("GLOBAL", message __VA_OPT__(,) __VA_ARGS__)
#define KITSUNE_INFO_FORMAT(message, ...)  KITSUNE_INFO_FORMAT_NAMED( "GLOBAL", message __VA_OPT__(,) __VA_ARGS__)
#define KITSUNE_WARN_FORMAT(message, ...)  KITSUNE_WARN_FORMAT_NAMED( "GLOBAL", message __VA_OPT__(,) __VA_ARGS__)
#define KITSUNE_ERROR_FORMAT(message, ...) KITSUNE_ERROR_FORMAT_NAMED("GLOBAL", message __VA_OPT__(,) __VA_ARGS__)
#define KITSUNE_FATAL_FORMAT(message, ...) KITSUNE_FATAL_FORMAT_NAMED("GLOBAL", message __VA_OPT__(,) __VA_ARGS__)

#define KITSUNE_TRACE_FORMAT_NAMED(loggerName, message, ...) KITSUNE_LOG_FORMAT_LEVEL_(loggerName, ::Kitsune::LogSeverity::Trace,   message, SourceLocation() __VA_OPT__(,) __VA_ARGS__)
#define KITSUNE_INFO_FORMAT_NAMED(loggerName, message, ...)  KITSUNE_LOG_FORMAT_LEVEL_(loggerName, ::Kitsune::LogSeverity::Info,    message, SourceLocation() __VA_OPT__(,) __VA_ARGS__)
#define KITSUNE_WARN_FORMAT_NAMED(loggerName, message, ...)  KITSUNE_LOG_FORMAT_LEVEL_(loggerName, ::Kitsune::LogSeverity::Warning, message, SourceLocation() __VA_OPT__(,) __VA_ARGS__)
#define KITSUNE_ERROR_FORMAT_NAMED(loggerName, message, ...) KITSUNE_LOG_FORMAT_LEVEL_(loggerName, ::Kitsune::LogSeverity::Error,   message, SourceLocation() __VA_OPT__(,) __VA_ARGS__)
#define KITSUNE_FATAL_FORMAT_NAMED(loggerName, message, ...) KITSUNE_LOG_FORMAT_LEVEL_(loggerName, ::Kitsune::LogSeverity::Fatal,   message, SourceLocation() __VA_OPT__(,) __VA_ARGS__)

// For use in engine code.
#define KITSUNE_ENGINE_TRACE_(message) KITSUNE_TRACE_NAMED("ENGINE", message)
#define KITSUNE_ENGINE_INFO_(message)  KITSUNE_INFO_NAMED( "ENGINE", message)
#define KITSUNE_ENGINE_WARN_(message)  KITSUNE_WARN_NAMED( "ENGINE", message)
#define KITSUNE_ENGINE_ERROR_(message) KITSUNE_ERROR_NAMED("ENGINE", message)
#define KITSUNE_ENGINE_FATAL_(message) KITSUNE_FATAL_NAMED("ENGINE", message)

#define KITSUNE_ENGINE_TRACE_FORMAT_(message, ...) KITSUNE_TRACE_FORMAT_NAMED("ENGINE", message __VA_OPT__(,) __VA_ARGS__)
#define KITSUNE_ENGINE_INFO_FORMAT_(message, ...)  KITSUNE_INFO_FORMAT_NAMED( "ENGINE", message __VA_OPT__(,) __VA_ARGS__)
#define KITSUNE_ENGINE_WARN_FORMAT_(message, ...)  KITSUNE_WARN_FORMAT_NAMED( "ENGINE", message __VA_OPT__(,) __VA_ARGS__)
#define KITSUNE_ENGINE_ERROR_FORMAT_(message, ...) KITSUNE_ERROR_FORMAT_NAMED("ENGINE", message __VA_OPT__(,) __VA_ARGS__)
#define KITSUNE_ENGINE_FATAL_FORMAT_(message, ...) KITSUNE_FATAL_FORMAT_NAMED("ENGINE", message __VA_OPT__(,) __VA_ARGS__)
