#pragma once

#include "Foundation/String/Format.h"
#include "Foundation/Logging/LogPayload.h"

namespace Kitsune
{
    // Logs the specified message along with additional information with the globally
    // registered loggers.
    void Log(StringView loggerName, LogSeverity severity,
             const SourceLocation& location, StringView message);

    // Same thing as `Log`, but uses format strings instead.
    template<typename... Args>
    inline void LogFormat(
        StringView loggerName, LogSeverity severity, const SourceLocation& location,
        StringView message, Args&&... args)
    {
        String formatted = Format(message, Forward<Args>(args)...);
        Log(loggerName, severity, location, formatted);
    }
}

#if !defined(KITSUNE_BUILD_PRODUCTION)
    #define KITSUNE_LOG(loggerName, severity, location, message) \
        ::Kitsune::Log(loggerName, severity, location, message)

    #define KITSUNE_LOG_FORMAT(loggerName, severity, location, message, ...) \
        ::Kitsune::LogFormat(loggerName, severity, location, message, __VA_ARGS__)
#else
    #define KITSUNE_LOG(loggerName, severity, message, source) ((void)0)
    #define KITSUNE_LOG_FORMAT(loggerName, severity, message, source, ...) ((void)0)
#endif

// Logging macros used in engine code.
#define KITSUNE_ENGINE_LOG(subsystem, severity, location, message) \
    KITSUNE_LOG("Kitsune/" KITSUNE_STRINGIFY(subsystem), severity, location, message)

#define KITSUNE_ENGINE_LOG_FORMAT(subsystem, severity, location, message, ...) \
    KITSUNE_LOG_FORMAT(                                                        \
        "Kitsune/" KITSUNE_STRINGIFY(subsystem),                               \
        severity,                                                              \
        location,                                                              \
        message,                                                               \
        __VA_ARGS__)

#define KITSUNE_ENGINE_TRACE(subsystem, message)  \
    KITSUNE_ENGINE_LOG(                           \
        subsystem, ::Kitsune::LogSeverity::Trace, \
        ::Kitsune::SourceLocation(), message)

#define KITSUNE_ENGINE_INFO(subsystem, message)  \
    KITSUNE_ENGINE_LOG(                          \
        subsystem, ::Kitsune::LogSeverity::Info, \
        ::Kitsune::SourceLocation(), message)

#define KITSUNE_ENGINE_WARN(subsystem, message)     \
    KITSUNE_ENGINE_LOG(                             \
        subsystem, ::Kitsune::LogSeverity::Warning, \
        ::Kitsune::SourceLocation(), message)

#define KITSUNE_ENGINE_ERROR(subsystem, message)  \
    KITSUNE_ENGINE_LOG(                           \
        subsystem, ::Kitsune::LogSeverity::Error, \
        ::Kitsune::SourceLocation(), message)

#define KITSUNE_ENGINE_FATAL(subsystem, message)  \
    KITSUNE_ENGINE_LOG(                           \
        subsystem, ::Kitsune::LogSeverity::Fatal, \
        ::Kitsune::SourceLocation(), message)

#define KITSUNE_ENGINE_TRACE_FORMAT(subsystem, message, ...) \
    KITSUNE_ENGINE_LOG_FORMAT(                               \
        subsystem, ::Kitsune::LogSeverity::Trace,            \
        ::Kitsune::SourceLocation(), message,                \
        __VA_ARGS__)                                         \

#define KITSUNE_ENGINE_INFO_FORMAT(subsystem, message, ...) \
    KITSUNE_ENGINE_LOG_FORMAT(                              \
        subsystem, ::Kitsune::LogSeverity::Info,            \
        ::Kitsune::SourceLocation(), message,               \
        __VA_ARGS__)                                        \

#define KITSUNE_ENGINE_WARN_FORMAT(subsystem, message, ...) \
    KITSUNE_ENGINE_LOG_FORMAT(                              \
        subsystem, ::Kitsune::LogSeverity::Warn,            \
        ::Kitsune::SourceLocation(), message,               \
        __VA_ARGS__)                                        \

#define KITSUNE_ENGINE_ERROR_FORMAT(subsystem, message, ...) \
    KITSUNE_ENGINE_LOG_FORMAT(                               \
        subsystem, ::Kitsune::LogSeverity::Error,            \
        ::Kitsune::SourceLocation(), message,                \
        __VA_ARGS__)                                         \

#define KITSUNE_ENGINE_FATAL_FORMAT(subsystem, message, ...) \
    KITSUNE_ENGINE_LOG_FORMAT(                               \
        subsystem, ::Kitsune::LogSeverity::Fatal,            \
        ::Kitsune::SourceLocation(), message,                \
        __VA_ARGS__)                                         \

// Logging macros which should be used in user code.
#define KITSUNE_USER_LOG(severity, message) \
    KITSUNE_LOG("User", severity, ::Kitsune::SourceLocation(), message)

#define KITSUNE_USER_LOG_FORMAT(severity, message, ...) \
    KITSUNE_LOG_FORMAT(                                 \
        "User",                                         \
        severity,                                       \
        ::Kitsune::SourceLocation(),                    \
        message,                                        \
        __VA_ARGS__)

#define KITSUNE_TRACE(message) KITSUNE_USER_LOG(::Kitsune::LogSeverity::Trace, message)
#define KITSUNE_INFO(message)  KITSUNE_USER_LOG(::Kitsune::LogSeverity::Info, message)
#define KITSUNE_WARN(message)  KITSUNE_USER_LOG(::Kitsune::LogSeverity::Warning, message)
#define KITSUNE_ERROR(message) KITSUNE_USER_LOG(::Kitsune::LogSeverity::Error, message)
#define KITSUNE_FATAL(message) KITSUNE_USER_LOG(::Kitsune::LogSeverity::Fatal, message)

#define KITSUNE_TRACE_FORMAT(message, ...) \
    KITSUNE_USER_LOG_FORMAT(::Kitsune::LogSeverity::Trace, message, __VA_ARGS__)

#define KITSUNE_INFO_FORMAT(message, ...) \
    KITSUNE_USER_LOG_FORMAT(::Kitsune::LogSeverity::Info, message, __VA_ARGS__)

#define KITSUNE_WARN_FORMAT(message, ...) \
    KITSUNE_USER_LOG_FORMAT(::Kitsune::LogSeverity::Warning, message, __VA_ARGS__)

#define KITSUNE_ERROR_FORMAT(message, ...) \
    KITSUNE_USER_LOG_FORMAT(::Kitsune::LogSeverity::Error, message, __VA_ARGS__)

#define KITSUNE_FATAL_FORMAT(message, ...) \
    KITSUNE_USER_LOG_FORMAT(::Kitsune::LogSeverity::Fatal, message, __VA_ARGS__)
