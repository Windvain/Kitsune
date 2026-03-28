#pragma once

#include "Foundation/String/String.h"

namespace Kitsune
{
    // An entry in a backtrace that contains file, function, line, and address
    // info.
    class BacktraceFrame
    {
    public:
        inline BacktraceFrame() = default;
        inline BacktraceFrame(
            StringView fileName,
            StringView symbolName,
            Uint64 lineNumber,
            void* address)
            : m_FileName(fileName),
              m_SymbolName(symbolName),
              m_LineNumber(lineNumber),
              m_Address(address)
        {
        }

    public:
        [[nodiscard]]
        inline String GetFileName() const
        {
            return m_FileName;
        }

        [[nodiscard]]
        inline String GetSymbolName() const
        {
            return m_SymbolName;
        }

        [[nodiscard]]
        inline Uint64 GetLineNumber() const
        {
            return m_LineNumber;
        }

        [[nodiscard]]
        inline void* GetAddress() const
        {
            return m_Address;
        }

    public:
        inline bool operator==(const BacktraceFrame& backtraceFrame) const
        {
            return (m_FileName == backtraceFrame.m_FileName) &&
                   (m_SymbolName == backtraceFrame.m_SymbolName) &&
                   (m_LineNumber == backtraceFrame.m_LineNumber) &&
                   (m_Address == backtraceFrame.m_Address);
        }

    private:
        String m_FileName = "<unknown>";
        String m_SymbolName = "<unknown>";

        Uint64 m_LineNumber = 0;
        void* m_Address = nullptr;
    };
}
