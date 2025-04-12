#pragma once

#include "Foundation/String/String.h"

namespace Kitsune
{
    class StackFrame
    {
    public:
        StackFrame() = default;
        inline StackFrame(const StringView fileName, const StringView functionName,
                   void* functionAddr, Uint32 lineNum)
            : m_FileName(fileName), m_FunctionAddress(functionAddr),
              m_FunctionName(functionName), m_LineNumber(lineNum)
        {
        }

    public:
        [[nodiscard]] String GetFileName() const { return m_FileName; }
        [[nodiscard]] String GetFunctionName() const { return m_FunctionName; }

        [[nodiscard]] void* GetFunctionAddress() const { return m_FunctionAddress; }
        [[nodiscard]] Uint32 GetLineNumber() const { return m_LineNumber; }

    public:
        inline bool operator==(const StackFrame& stackFrame) const
        {
            return (m_FileName == stackFrame.m_FileName) &&
                   (m_FunctionAddress == stackFrame.m_FunctionAddress) &&
                   (m_FunctionName == stackFrame.m_FunctionName) &&
                   (m_LineNumber == stackFrame.m_LineNumber);
        }

    private:
        String m_FileName = "<unknown>";

        void* m_FunctionAddress = nullptr;
        String m_FunctionName = "<unknown>";

        Uint32 m_LineNumber = 0;
    };
}
