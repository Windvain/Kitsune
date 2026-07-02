#pragma once

#include "Foundation/String/Format.h"
#include "Foundation/Containers/Array.h"

#include "Foundation/Logging/LogSink.h"
#include "Foundation/Memory/ScopedPtr.h"

#include "Foundation/Utilities/NonCopyable.h"

namespace Kitsune
{
    // A singleton class which provides an interface to interact with
    // "global" sinks.
    class KITSUNE_API Logger : public NonCopyable
    {
    public:
        Logger();
        ~Logger();

    public:
        inline void Log(StringView message, StringView topic,
                        const SourceLocation& location, LogSeverity severity)
        {
            LogPayload payload(message, topic, location, severity);
            for (ScopedPtr<LogSink>& logSink : m_Sinks)
                logSink->Log(payload);

            // Flushing the sinks makes development easier.
#if !defined(KITSUNE_BUILD_PRODUCTION)
            Flush();
#endif
        }

        template<typename... Args>
        inline void LogFormat(
            StringView message, StringView topic,
            const SourceLocation& location, LogSeverity severity,
            Args&&... args)
        {
            String formatted = Format(message, Forward<Args>(args)...);
            Log(formatted, topic, location, severity);
        }

    public:
        inline void Log(StringView message, StringView topic, LogSeverity severity)
        {
            Log(message, topic, SourceLocation(), severity);
        }

        template<typename... Args>
        inline void LogFormat(
            StringView message, StringView topic, LogSeverity severity,
            Args&&... args)
        {
            LogFormat(message, topic, SourceLocation(),
                      severity, Forward<Args>(args)...);
        }

        inline void Log(StringView message, LogSeverity severity)
        {
            Log(message, "", SourceLocation(), severity);
        }

        template<typename... Args>
        inline void LogFormat(
            StringView message, LogSeverity severity,
            Args&&... args)
        {
            LogFormat(message, "", SourceLocation(),
                      severity, Forward<Args>(args)...);
        }

        inline void Log(StringView message, const SourceLocation& location, LogSeverity severity)
        {
            Log(message, "", location, severity);
        }

        template<typename... Args>
        inline void LogFormat(
            StringView message, const SourceLocation& location, LogSeverity severity,
            Args&&... args)
        {
            LogFormat(message, "", location,
                      severity, Forward<Args>(args)...);
        }

    public:
        inline void Flush()
        {
            for (ScopedPtr<LogSink>& sink : m_Sinks)
                sink->Flush();
        }

    public:
        [[nodiscard]]
        inline Array<ScopedPtr<LogSink>>& GetSinks()
        {
            return m_Sinks;
        }

        [[nodiscard]]
        inline const Array<ScopedPtr<LogSink>>& GetSinks() const
        {
            return m_Sinks;
        }

        inline void RegisterSink(ScopedPtr<LogSink>&& sink)
        {
            m_Sinks.PushBack(Move(sink));
        }

    public:
        static Logger* GetInstance()
        {
            return s_Instance;
        }

    private:
        static Logger* s_Instance;

    private:
        Array<ScopedPtr<LogSink>> m_Sinks;
    };
}

#if !defined(KITSUNE_BUILD_PRODUCTION)
    #define KITSUNE_LOG(topic, severity, location, message) \
        ::Kitsune::Logger::GetInstance()->Log(message, topic, location, severity)

    #define KITSUNE_LOG_FORMAT(topic, severity, location, message, ...) \
        ::Kitsune::Logger::GetInstance()->LogFormat(                    \
            message, topic, location, severity, __VA_ARGS__)
#else
    #define KITSUNE_LOG(topic, severity, message, source) ((void)0)
    #define KITSUNE_LOG_FORMAT(topic, severity, message, source, ...) ((void)0)
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
