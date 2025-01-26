#pragma once

#include "Foundation/Memory/SharedPtr.h"
#include "Foundation/Containers/Array.h"

#include "Foundation/String/String.h"
#include "Foundation/String/Format.h"

#include "Foundation/Logging/ILoggerSink.h"
#include "Foundation/Logging/LogSeverity.h"

namespace Kitsune
{
    class Logger
    {
    public:
        Logger() = default;

        inline explicit Logger(const StringView& name) : m_Name(name) { /* ... */ }
        inline Logger(const StringView& name, const SharedPtr<ILoggerSink>& sink)
            : m_Name(name), m_Sinks{sink}
        {
        }

        template<ForwardIterator It>
        inline Logger(const StringView& name, It begin, It end)
            : m_Name(name), m_Sinks(begin, end)
        {
        }

        Logger(const Logger& logger) = default;
        Logger(Logger&& logger) = default;

        ~Logger() = default;

    public:
        Logger& operator=(const Logger& logger) = default;
        Logger& operator=(Logger&& logger) = default;

    public:
        KITSUNE_API_ void Log(LogSeverity severity, const StringView& message);
        KITSUNE_API_ void Flush();

        inline void Log(const StringView& message) { Log(m_MinimumSeverity, message); }

        template<typename... Args>
        inline void LogFormat(LogSeverity severity, const StringView& format, Args&&... args)
        {
            String formatted = Format(format, Forward<Args>(args)...);
            Log(severity, formatted);
        }

        template<typename... Args>
        inline void LogFormat(const StringView& format, Args&&... args)
        {
            LogFormat(m_MinimumSeverity, format, Forward<Args>(args)...);
        }

    public:
        inline StringView  GetName()            const { return m_Name;            }
        inline LogSeverity GetMinimumSeverity() const { return m_MinimumSeverity; }
        inline LogSeverity GetFlushSeverity()   const { return m_FlushSeverity;   }

        inline void SetMinimumSeverity(LogSeverity severity) { m_MinimumSeverity = severity; }
        inline void SetFlushSeverity(LogSeverity severity)   { m_FlushSeverity = severity;   }

    public:
        inline bool IsLogged(LogSeverity severity)  { return (severity >= m_MinimumSeverity); }
        inline bool IsFlushed(LogSeverity severity) { return (severity >= m_FlushSeverity);   }

    public:
        inline       Array<SharedPtr<ILoggerSink>>& GetSinks()       { return m_Sinks; }
        inline const Array<SharedPtr<ILoggerSink>>& GetSinks() const { return m_Sinks; }

    private:
        LogSeverity m_MinimumSeverity = LogSeverity::Trace;
        LogSeverity m_FlushSeverity = LogSeverity::Error;

        String m_Name;
        Array<SharedPtr<ILoggerSink>> m_Sinks;
    };
}
