#pragma once

#include "Foundation/Logging/ILogSink.h"
#include "Foundation/Logging/LogMessage.h"

#include "Foundation/String/String.h"
#include "Foundation/Memory/SharedPtr.h"

#include "Foundation/Containers/Array.h"
#include "Foundation/Algorithms/ForEach.h"

namespace Kitsune
{
    class Logger
    {
    public:
        Logger() = default;
        inline Logger(const StringView name)
            : m_Name(name)
        {
        }

        inline Logger(const StringView name, const SharedPtr<ILogSink>& sink)
            : m_Name(name), m_Sinks(1, sink)
        {
        }

        template<ForwardIterator It>
        inline Logger(const StringView name, It begin, It end)
            : m_Name(name), m_Sinks(begin, end)
        {
        }

        inline Logger(const StringView name, std::initializer_list<SharedPtr<ILogSink>> sinks)
            : m_Name(name), m_Sinks(sinks.begin(), sinks.end())
        {
        }

        ~Logger() = default;

    public:
        void Log(LogSeverity severity, SourceLocation loc, const StringView message)
        {
            if (!IsLogged(severity)) return;

            LogMessage logMessage(message, m_Name, Move(loc), severity);
            Algorithms::ForEach(m_Sinks.GetBegin(), m_Sinks.GetEnd(), [&](const auto& sink)
            {
                sink->Log(logMessage);
            });

            if (IsFlushed(severity))
                Flush();
        }

        KITSUNE_FORCEINLINE void Log(LogSeverity severity, const StringView message)
        {
            Log(severity, SourceLocation(), message);
        }

        KITSUNE_FORCEINLINE void Log(SourceLocation loc, const StringView message)
        {
            Log(m_MinSeverity, Move(loc), message);
        }

        KITSUNE_FORCEINLINE void Log(const StringView message)
        {
            Log(m_MinSeverity, SourceLocation(), message);
        }

        template<typename... Args>
        void LogFormat(LogSeverity severity, SourceLocation loc, const StringView fmt, Args&&... args)
        {
            String formatted = Format(fmt, Forward<Args>(args)...);
            Log(severity, Move(loc), formatted);
        }

        template<typename... Args>
        KITSUNE_FORCEINLINE void LogFormat(LogSeverity severity, const StringView fmt, Args&&... args)
        {
            LogFormat(severity, fmt, Forward<Args>(args)...);
        }

        template<typename... Args>
        KITSUNE_FORCEINLINE void LogFormat(SourceLocation loc, const StringView fmt, Args&&... args)
        {
            LogFormat(m_MinSeverity, Move(loc), fmt, Forward<Args>(args)...);
        }

        template<typename... Args>
        KITSUNE_FORCEINLINE void LogFormat(const StringView fmt, Args&&... args)
        {
            LogFormat(m_MinSeverity, SourceLocation(), fmt, Forward<Args>(args)...);
        }

    public:
        void Flush()
        {
            Algorithms::ForEach(m_Sinks.GetBegin(), m_Sinks.GetEnd(),
                                [&](const auto& sink) { sink->Flush(); });
        }

    public:
        inline bool IsLogged(LogSeverity severity) const { return (severity >= m_MinSeverity); }
        inline bool IsFlushed(LogSeverity severity) const { return (severity >= m_FlushSeverity); }

    public:
        inline String GetName() const { return m_Name; }

        inline Array<SharedPtr<ILogSink>>& GetSinks()             { return m_Sinks; }
        inline const Array<SharedPtr<ILogSink>>& GetSinks() const { return m_Sinks; }

        inline LogSeverity GetMinimumSeverity() const { return m_MinSeverity; }
        inline LogSeverity GetFlushSeverity() const { return m_FlushSeverity; }

        inline void SetMinimumSeverity(LogSeverity severity)
        {
            m_MinSeverity = severity;
        }

        inline void SetFlushSeverity(LogSeverity severity)
        {
            m_FlushSeverity = severity;
        }

    private:
        String m_Name;
        Array<SharedPtr<ILogSink>> m_Sinks;

        LogSeverity m_MinSeverity = LogSeverity::Trace;
        LogSeverity m_FlushSeverity = LogSeverity::Warning;
    };
}
