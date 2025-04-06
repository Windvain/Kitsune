#pragma once

#include "Foundation/Logging/Logger.h"

namespace Kitsune
{
    KITSUNE_API_ Logger* SetGlobalLogger(Logger* logger);
    KITSUNE_API_ Logger* GetGlobalLogger();
}

#if !defined(KITSUNE_BUILD_RELEASE)
    #define KITSUNE_LOG_LEVEL(severity, message) \
        ::Kitsune::GetGlobalLogger()->Log(severity, SourceLocation::Current(), message)

    #define KITSUNE_LOGF_LEVEL(severity, message, ...) \
        ::Kitsune::GetGlobalLogger()->LogFormat(severity, SourceLocation::Current(), message, __VA_ARGS__)

    #define KITSUNE_LOG(message) ::Kitsune::GetGlobalLogger()->Log(SourceLocation::Current(), message)
    #define KITSUNE_LOGF(message, ...) \
        ::Kitsune::GetGlobalLogger()->LogFormat(SourceLocation::Current(), message, __VA_ARGS__)
#else
    #define KITSUNE_LOG_LEVEL(severity, message)
    #define KITSUNE_LOGF_LEVEL(severity, message, ...)

    #define KITSUNE_LOG(message)
    #define KITSUNE_LOGF(message, ...)
#endif

#define KITSUNE_TRACE(message)       KITSUNE_LOG_LEVEL(::Kitsune::LogSeverity::Trace, message);
#define KITSUNE_INFO(message)        KITSUNE_LOG_LEVEL(::Kitsune::LogSeverity::Info, message);
#define KITSUNE_WARN(message)        KITSUNE_LOG_LEVEL(::Kitsune::LogSeverity::Warn, message);
#define KITSUNE_ERROR(message)       KITSUNE_LOG_LEVEL(::Kitsune::LogSeverity::Error, message);
#define KITSUNE_FATAL(message)       KITSUNE_LOG_LEVEL(::Kitsune::LogSeverity::Fatal, message);

#define KITSUNE_TRACE_FORMAT(message, ...) KITSUNE_LOGF_LEVEL(::Kitsune::LogSeverity::Trace, message, __VA_ARGS__);
#define KITSUNE_INFO_FORMAT(message, ...)  KITSUNE_LOGF_LEVEL(::Kitsune::LogSeverity::Info, message, __VA_ARGS__);
#define KITSUNE_WARN_FORMAT(message, ...)  KITSUNE_LOGF_LEVEL(::Kitsune::LogSeverity::Warn, message, __VA_ARGS__);
#define KITSUNE_ERROR_FORMAT(message, ...) KITSUNE_LOGF_LEVEL(::Kitsune::LogSeverity::Error, message, __VA_ARGS__);
#define KITSUNE_FATAL_FORMAT(message, ...) KITSUNE_LOGF_LEVEL(::Kitsune::LogSeverity::Fatal, message, __VA_ARGS__);
