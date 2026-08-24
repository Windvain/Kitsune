#pragma once

#include "Foundation/Logging/LogSink.h"

#include "Foundation/Streams/FileWriter.h"
#include "Foundation/Streams/WriterIterator.h"

namespace Kitsune
{
    class FileLogSink : public LogSink
    {
    public:
        inline FileLogSink(Filesystem::PathView path)
            : m_Writer(path, FileOpenMode::Append)
        {
        }

    public:
        inline void Log(const LogPayload& payload) override
        {
            FormatTo(WriterIterator<FileWriter>(m_Writer), "{0}", payload);
            m_Writer.WriteLine();
        }

        inline void Flush() override
        {
            m_Writer.Flush();
        }

    private:
        FileWriter m_Writer;
    };
}
