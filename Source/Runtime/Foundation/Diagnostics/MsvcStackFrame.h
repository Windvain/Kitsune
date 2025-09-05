#include "Foundation/Diagnostics/StackFrame.h"
#include <cstdlib>

#include <Windows.h>
#include <DbgHelp.h>

namespace Kitsune
{
    class MsvcStackFrame : public StackFrame
    {
    public:
        inline MsvcStackFrame(HANDLE process, DWORD64 pcOffset)
        {
            SYMBOL_INFO* symbolInfo = static_cast<SYMBOL_INFO*>(std::malloc(s_SymbolInfoSize));
            ZeroMemory(symbolInfo, s_SymbolInfoSize);

            symbolInfo->SizeOfStruct = sizeof(SYMBOL_INFO);
            symbolInfo->MaxNameLen = s_MaxNameLength + /* Name[1] + NUL */ 2;

            ::SymFromAddr(process, pcOffset, nullptr, symbolInfo);

            m_SymbolName = symbolInfo->Name;
            if (m_SymbolName.IsEmpty())
                m_SymbolName = s_UnknownString;

            IMAGEHLP_LINE64 lineStruct;
            lineStruct.SizeOfStruct = sizeof(lineStruct);

            DWORD offset;
            if (::SymGetLineFromAddr64(process, pcOffset, &offset, &lineStruct))
            {
                m_FileName = lineStruct.FileName;
                m_LineNumber = lineStruct.LineNumber;
                m_Address = reinterpret_cast<void*>(lineStruct.Address);
            }
            else
            {
                m_FileName = s_UnknownString;
                m_LineNumber = 0;
                m_Address = nullptr;
            }
        }

    private:
        static constexpr Usize s_MaxNameLength = 1024;
        static constexpr Usize s_SymbolInfoSize = sizeof(SYMBOL_INFO) + s_MaxNameLength + 1;

        static constexpr const char* s_UnknownString = "<unknown>";
    };
}
