#pragma once

#include "Foundation/Logging/Logger.h"
#include "Foundation/Diagnostics/Assert.h"

namespace Kitsune
{
    class Logging
    {
    public:
        static inline void SetGlobalLogger(Logger* logger) { s_Logger = logger; }
        static inline Logger* GetGlobalLogger()            { return s_Logger;   }

    public:
        static inline void Log(LogSeverity severity, const StringView& message)
        {
            KITSUNE_ASSERT(s_Logger != nullptr, "A global logger has not been set.");
            s_Logger->Log(severity, message);
        }

        static inline void Log(const StringView& message)
        {
            KITSUNE_ASSERT(s_Logger != nullptr, "A global logger has not been set.");
            s_Logger->Log(message);
        }

        template<typename... Args>
        static inline void LogFormat(LogSeverity severity, const StringView& message,
                                     Args&&... args)
        {
            KITSUNE_ASSERT(s_Logger != nullptr, "A global logger has not been set.");
            s_Logger->LogFormat(severity, message, Forward<Args>(args)...);
        }

        template<typename... Args>
        static inline void LogFormat(const StringView& message, Args&&... args)
        {
            KITSUNE_ASSERT(s_Logger != nullptr, "A global logger has not been set.");
            s_Logger->LogFormat(message, Forward<Args>(args)...);
        }

        static inline void Flush()
        {
            KITSUNE_ASSERT(s_Logger != nullptr, "A global logger has not been set.");
            s_Logger->Flush();
        }

    public:
        template<typename... Args>
        static inline void Trace(const StringView& message, Args&&... args)
        {
            LogFormat(LogSeverity::Trace, message, Forward<Args>(args)...);
        }

        template<typename... Args>
        static inline void Info(const StringView& message, Args&&... args)
        {
            LogFormat(LogSeverity::Info, message, Forward<Args>(args)...);
        }

        template<typename... Args>
        static inline void Warn(const StringView& message, Args&&... args)
        {
            LogFormat(LogSeverity::Warning, message, Forward<Args>(args)...);
        }
        template<typename... Args>
        static inline void Error(const StringView& message, Args&&... args)
        {
            LogFormat(LogSeverity::Error, message, Forward<Args>(args)...);
        }
        template<typename... Args>
        static inline void Fatal(const StringView& message, Args&&... args)
        {
            LogFormat(LogSeverity::Fatal, message, Forward<Args>(args)...);
        }

    private:
        KITSUNE_API_ static Logger* s_Logger;
    };
}
