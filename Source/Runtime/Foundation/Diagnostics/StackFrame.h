#pragma once

#include "Foundation/String/String.h"

namespace Kitsune
{
    class StackFrame
    {
    public:
        StackFrame() = default;
        virtual ~StackFrame() = default;

    public:
        [[nodiscard]]
        inline String GetFileName() const { return m_FileName; }

        [[nodiscard]]
        inline String GetSymbolName() const { return m_SymbolName; }

        [[nodiscard]]
        inline void* GetAddress() const { return m_Address; }

        [[nodiscard]]
        inline Uint64 GetLineNumber() const { return m_LineNumber; }

    public:
        inline bool operator==(const StackFrame& frame) const
        {
            return (GetFileName() == frame.GetFileName()) &&
                   (GetSymbolName() == frame.GetSymbolName()) &&
                   (GetAddress() == frame.GetAddress()) &&
                   (GetLineNumber() == frame.GetLineNumber());
        }

    protected:
        void* m_Address;
        Uint64 m_LineNumber;

        String m_FileName;
        String m_SymbolName;
    };
}
