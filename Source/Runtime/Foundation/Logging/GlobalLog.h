#pragma once

#include "Foundation/String/Format.h"
#include "Foundation/Logging/LogPayload.h"

namespace Kitsune
{
    // Logs the specified message along with additional information with the globally
    // registered loggers.
    void Log(const StringView loggerName, const LogSeverity severity,
             const SourceLocation location, const StringView message);

    // Same thing as `Log`, but uses format strings instead.
    template<typename... Args>
    inline void LogFormat(const StringView loggerName, const LogSeverity severity,
                          const SourceLocation location, const StringView message,
                          Args&&... args)
    {
        String formatted = Format(message, Forward<Args>(args)...);
        Log(loggerName, severity, location, formatted);
    }
}

#if !defined(KITSUNE_BUILD_PRODUCTION)
    #define KITSUNE_LOG_LEVEL_(loggerName, severity, message, source) \
        ::Kitsune::Log(loggerName, severity, source, message)

    #define KITSUNE_LOG_FORMAT_LEVEL_(loggerName, severity, message, source, ...) \
        ::Kitsune::LogFormat(loggerName, severity, source, message, __VA_ARGS__)
#else
    #define KITSUNE_LOG_LEVEL_(loggerName, severity, message, source)             ((void)0)
    #define KITSUNE_LOG_FORMAT_LEVEL_(loggerName, severity, message, source, ...) ((void)0)
#endif

#define KITSUNE_TRACE(message) KITSUNE_TRACE_NAMED("User", message)
#define KITSUNE_INFO(message)  KITSUNE_INFO_NAMED( "User", message)
#define KITSUNE_WARN(message)  KITSUNE_WARN_NAMED( "User", message)
#define KITSUNE_ERROR(message) KITSUNE_ERROR_NAMED("User", message)
#define KITSUNE_FATAL(message) KITSUNE_FATAL_NAMED("User", message)

#define KITSUNE_TRACE_NAMED(loggerName, message) KITSUNE_LOG_LEVEL_(loggerName, ::Kitsune::LogSeverity::Trace,   message, SourceLocation())
#define KITSUNE_INFO_NAMED(loggerName, message)  KITSUNE_LOG_LEVEL_(loggerName, ::Kitsune::LogSeverity::Info,    message, SourceLocation())
#define KITSUNE_WARN_NAMED(loggerName, message)  KITSUNE_LOG_LEVEL_(loggerName, ::Kitsune::LogSeverity::Warning, message, SourceLocation())
#define KITSUNE_ERROR_NAMED(loggerName, message) KITSUNE_LOG_LEVEL_(loggerName, ::Kitsune::LogSeverity::Error,   message, SourceLocation())
#define KITSUNE_FATAL_NAMED(loggerName, message) KITSUNE_LOG_LEVEL_(loggerName, ::Kitsune::LogSeverity::Fatal,   message, SourceLocation())

#define KITSUNE_TRACE_FORMAT(message, ...) KITSUNE_TRACE_FORMAT_NAMED("User", message, __VA_ARGS__)
#define KITSUNE_INFO_FORMAT(message, ...)  KITSUNE_INFO_FORMAT_NAMED( "User", message, __VA_ARGS__)
#define KITSUNE_WARN_FORMAT(message, ...)  KITSUNE_WARN_FORMAT_NAMED( "User", message, __VA_ARGS__)
#define KITSUNE_ERROR_FORMAT(message, ...) KITSUNE_ERROR_FORMAT_NAMED("User", message, __VA_ARGS__)
#define KITSUNE_FATAL_FORMAT(message, ...) KITSUNE_FATAL_FORMAT_NAMED("User", message, __VA_ARGS__)

#define KITSUNE_TRACE_FORMAT_NAMED(loggerName, message, ...) KITSUNE_LOG_FORMAT_LEVEL_(loggerName, ::Kitsune::LogSeverity::Trace,   message, SourceLocation(), __VA_ARGS__)
#define KITSUNE_INFO_FORMAT_NAMED(loggerName, message, ...)  KITSUNE_LOG_FORMAT_LEVEL_(loggerName, ::Kitsune::LogSeverity::Info,    message, SourceLocation(), __VA_ARGS__)
#define KITSUNE_WARN_FORMAT_NAMED(loggerName, message, ...)  KITSUNE_LOG_FORMAT_LEVEL_(loggerName, ::Kitsune::LogSeverity::Warning, message, SourceLocation(), __VA_ARGS__)
#define KITSUNE_ERROR_FORMAT_NAMED(loggerName, message, ...) KITSUNE_LOG_FORMAT_LEVEL_(loggerName, ::Kitsune::LogSeverity::Error,   message, SourceLocation(), __VA_ARGS__)
#define KITSUNE_FATAL_FORMAT_NAMED(loggerName, message, ...) KITSUNE_LOG_FORMAT_LEVEL_(loggerName, ::Kitsune::LogSeverity::Fatal,   message, SourceLocation(), __VA_ARGS__)

#define KITSUNE_ENGINE_TRACE_(message) KITSUNE_TRACE_NAMED("Kitsune", message)
#define KITSUNE_ENGINE_INFO_(message)  KITSUNE_INFO_NAMED( "Kitsune", message)
#define KITSUNE_ENGINE_WARN_(message)  KITSUNE_WARN_NAMED( "Kitsune", message)
#define KITSUNE_ENGINE_ERROR_(message) KITSUNE_ERROR_NAMED("Kitsune", message)
#define KITSUNE_ENGINE_FATAL_(message) KITSUNE_FATAL_NAMED("Kitsune", message)

#define KITSUNE_ENGINE_TRACE_FORMAT_(message, ...) KITSUNE_TRACE_FORMAT_NAMED("Kitsune", message, __VA_ARGS__)
#define KITSUNE_ENGINE_INFO_FORMAT_(message, ...)  KITSUNE_INFO_FORMAT_NAMED( "Kitsune", message, __VA_ARGS__)
#define KITSUNE_ENGINE_WARN_FORMAT_(message, ...)  KITSUNE_WARN_FORMAT_NAMED( "Kitsune", message, __VA_ARGS__)
#define KITSUNE_ENGINE_ERROR_FORMAT_(message, ...) KITSUNE_ERROR_FORMAT_NAMED("Kitsune", message, __VA_ARGS__)
#define KITSUNE_ENGINE_FATAL_FORMAT_(message, ...) KITSUNE_FATAL_FORMAT_NAMED("Kitsune", message, __VA_ARGS__)
