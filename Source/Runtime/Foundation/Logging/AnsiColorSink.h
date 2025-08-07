#pragma once

#include "Foundation/Memory/SharedPtr.h"

#include "Foundation/Logging/IStream.h"
#include "Foundation/Logging/ILogSink.h"

#include "Foundation/Threading/Mutex.h"
#include "Foundation/Threading/LockGuard.h"

namespace Kitsune
{
    class AnsiColorSink : public ILogSink
    {
    public:
        inline AnsiColorSink(const SharedPtr<IWriteStream<char>>& outStream)
            : m_Stream(outStream)
        {
        }

    public:
        KITSUNE_API_ void Log(const LogMessage& message) override;
        KITSUNE_API_ void Flush() override;

    private:
        KITSUNE_API_ static const char* ConvertToAnsiColor(LogSeverity severity);

    public:
        static constexpr const char* TraceColor = "\x1B[0m";
        static constexpr const char* InfoColor = "\x1B[36m";
        static constexpr const char* WarningColor = "\x1B[33m";

        // The bold colours appear *lighter* than their non-bolded counterparts (?)
        static constexpr const char* ErrorColor = "\x1B[31;1m";
        static constexpr const char* FatalColor = "\x1B[31m";

    private:
        SharedPtr<IWriteStream<char>> m_Stream;
        Mutex m_SinkLock;
    };
}
