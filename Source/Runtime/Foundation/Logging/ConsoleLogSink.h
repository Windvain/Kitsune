#pragma once

#include "Foundation/Logging/LogSink.h"

#include "Foundation/Streams/ConsoleWriter.h"
#include "Foundation/Streams/WriterIterator.h"

namespace Kitsune
{
    // A log sink which outputs log payloads to the console.
    class ConsoleLogSink : public LogSink
    {
    public:
        inline void Log(const LogPayload& payload) override
        {
            FormatTo(WriterIterator<ConsoleWriter>(m_Writer), "{0:c}", payload);
            m_Writer.WriteLine();
        }

        inline void Flush() override
        {
            m_Writer.Flush();
        }

    private:
        ConsoleWriter m_Writer{ ConsoleWriterType::StdOut };
    };
}
