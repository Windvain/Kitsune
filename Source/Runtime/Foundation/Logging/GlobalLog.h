#pragma once

#include "Foundation/Logging/ILogger.h"

/*
namespace Kitsune
{
    KITSUNE_API_ Logger* SetGlobalLogger(Logger* logger);
    KITSUNE_API_ Logger* GetGlobalLogger();
}

#if !defined(KITSUNE_BUILD_PRODUCTION)
    #define KITSUNE_LOG_LEVEL_(severity, message, source) \
        ::Kitsune::GetGlobalLogger()->Log(severity, source, message)

    #define KITSUNE_LOG_FORMAT_LEVEL_(severity, message, source, ...) \
        ::Kitsune::GetGlobalLogger()->LogFormat(severity, source, message, __VA_ARGS__)

    #define KITSUNE_LOG(message) ::Kitsune::GetGlobalLogger()->Log(SourceLocation(), message)
    #define KITSUNE_LOG_FORMAT(message, ...) \
        ::Kitsune::GetGlobalLogger()->LogFormat(SourceLocation(), message, __VA_ARGS__)

    #define KITSUNE_LOG_SOURCED(message) ::Kitsune::GetGlobalLogger()->Log(SourceLocation::Current(), message)
    #define KITSUNE_LOG_FORMAT_SOURCED(message, ...) \
        ::Kitsune::GetGlobalLogger()->LogFormat(SourceLocation::Current(), message, __VA_ARGS__)
#else*/
    #define KITSUNE_LOG_LEVEL_(severity, message, source)
    #define KITSUNE_LOG_FORMAT_LEVEL_(severity, message, source, ...)

    #define KITSUNE_LOG(message)
    #define KITSUNE_LOG_FORMAT(message, ...)

    #define KITSUNE_LOG_SOURCED(message)
    #define KITSUNE_LOG_FORMAT_SOURCED(message, ...)
// #endif

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
