#pragma once

#include "Foundation/Diagnostics/StackFrame.h"

namespace Kitsune
{
    class GeneralStackFrame : public StackFrame
    {
    public:
        inline GeneralStackFrame(const char* filename, const char* symbol,
                                 void* address, Uint64 line)
        {
            m_Address = address;
            m_LineNumber = line;
            m_FileName = filename;
            m_SymbolName = symbol;
        }
    };
}
