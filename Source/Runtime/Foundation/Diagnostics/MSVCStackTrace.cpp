#include "Foundation/Diagnostics/StackTrace.h"
#include <Windows.h>

#include <Psapi.h>
#include <DbgHelp.h>

#include "Foundation/String/Format.h"
#include "Foundation/Memory/ScopedPtr.h"

#include "Foundation/Diagnostics/StackTraceException.h"

namespace Kitsune
{
    class WindowsSymbol
    {
    public:
        WindowsSymbol(HANDLE process, const STACKFRAME64& stackFrame)
            : m_Process(process), m_PCOffset(stackFrame.AddrPC.Offset)
        {
            m_SymbolInfo = static_cast<SYMBOL_INFO*>(
                Memory::Allocate(sizeof(SYMBOL_INFO) + MaxNameLength - 1));

            ZeroMemory(m_SymbolInfo, sizeof(SYMBOL_INFO) + MaxNameLength - 1);
            m_SymbolInfo->SizeOfStruct = sizeof(SYMBOL_INFO);
            m_SymbolInfo->MaxNameLen = WindowsSymbol::MaxNameLength;

            ::SymFromAddr(m_Process, m_PCOffset, nullptr, m_SymbolInfo);

            if (std::strlen(m_SymbolInfo->Name) == 0)
                std::memcpy(m_SymbolInfo->Name, UnknownString, std::strlen(UnknownString));

            IMAGEHLP_LINE64 line;
            line.SizeOfStruct = sizeof(line);

            DWORD offset;
            if (::SymGetLineFromAddr64(m_Process, m_PCOffset, &offset, &line))
            {
                m_FileName = line.FileName;
                m_LineNum = line.LineNumber;
                m_Address = reinterpret_cast<void*>(line.Address);
            }
            else
            {
                m_FileName = UnknownString;
                m_LineNum = 0;
                m_Address = nullptr;
            }
        }

        ~WindowsSymbol()
        {
            Memory::Free(m_SymbolInfo);
        }

    public:
        String GetFileName() const { return m_FileName; }
        String GetName()     const { return m_SymbolInfo->Name; }

        DWORD GetLine()      const { return m_LineNum; }
        void* GetAddress()   const { return m_Address; }

    public:
        static constexpr ULONG MaxNameLength = 1024;
        static constexpr char UnknownString[] = "<unknown>";

    private:
        HANDLE m_Process;
        DWORD64 m_PCOffset;

        String m_FileName;
        DWORD m_LineNum;
        void* m_Address;

        SYMBOL_INFO* m_SymbolInfo;
    };

    class ScopedSymInit
    {
    public:
        ScopedSymInit(HANDLE process, DWORD options)
            : m_Process(process)
        {
            if (::SymInitializeW(process, nullptr, false) == FALSE)
                throw StackTraceException("Failed to initialize symbol handler.");

            ::SymSetOptions(::SymGetOptions() | options);
        }

        ~ScopedSymInit()
        {
            ::SymCleanup(m_Process);
        }

    private:
        HANDLE m_Process;
    };

    void* LoadModulesSymbols(HANDLE process)
    {
        struct WindowsModuleInfo
        {
            WideString ImageName;
            WideString ModuleName;

            void* Address;
            DWORD LoadSize;
        };

        DWORD bytesNeeded;
        ::EnumProcessModules(process, nullptr, 0, &bytesNeeded);

        Array<HMODULE> moduleHandles(bytesNeeded / sizeof(HMODULE), HMODULE());
        ::EnumProcessModules(process, moduleHandles.Data(), DWORD(moduleHandles.Size() * sizeof(HMODULE)),
                             &bytesNeeded);

        constexpr Usize BufferSize = 1024;
        Array<WindowsModuleInfo> moduleInfos(moduleHandles.Size());

        for (HMODULE module : moduleHandles)
        {
            WindowsModuleInfo modInfo;
            MODULEINFO winModInfo;

            ::GetModuleInformation(process, module, &winModInfo, sizeof(winModInfo));
            modInfo.Address = winModInfo.lpBaseOfDll;
            modInfo.LoadSize = winModInfo.SizeOfImage;

            wchar_t temp[BufferSize] = { 0 };

            ::GetModuleFileNameExW(process, module, temp, sizeof(temp));
            modInfo.ImageName = temp;

            ::GetModuleBaseNameW(process, module, temp, sizeof(temp));
            modInfo.ModuleName = temp;

            if (::SymLoadModuleExW(process, nullptr, modInfo.ImageName.Data(), modInfo.ModuleName.Data(),
                                   reinterpret_cast<DWORD64>(modInfo.Address), modInfo.LoadSize, nullptr, 0) == 0)
            {
                throw StackTraceException("Could not load modules for the specified process.");
            }

            moduleInfos.PushBack(Move(modInfo));
        }

        return moduleInfos[0].Address;
    }

    inline STACKFRAME64 InitializeStackFrame(PCONTEXT context)
    {
        STACKFRAME64 stackFrame;
        stackFrame.AddrFrame.Mode = AddrModeFlat;
        stackFrame.AddrPC.Mode = AddrModeFlat;
        stackFrame.AddrStack.Mode = AddrModeFlat;

#if defined(KITSUNE_ARCH_X86_64)
        stackFrame.AddrFrame.Offset = context->Rbp;
        stackFrame.AddrPC.Offset = context->Rip;
        stackFrame.AddrStack.Offset = context->Rsp;
#else
    #error StackTrace is only implemented for x64.
#endif

        return stackFrame;
    }

    StackTrace MakeStackTrace(Usize skipCount, Usize maxDepth)
    {
        ++skipCount;

        CONTEXT context;
        ::RtlCaptureContext(&context);

        HANDLE process = ::GetCurrentProcess();
        HANDLE thread = ::GetCurrentThread();
        ScopedSymInit handler(process, SYMOPT_LOAD_LINES | SYMOPT_UNDNAME | SYMOPT_EXACT_SYMBOLS);

        void* baseAddress = LoadModulesSymbols(process);
        STACKFRAME64 stackFrame = InitializeStackFrame(&context);

        IMAGE_NT_HEADERS* headers = ::ImageNtHeader(baseAddress);
        DWORD machineType = headers->FileHeader.Machine;

        Usize currDepth = 0;
        Array<StackFrame> stackframes;

        while (currDepth < maxDepth)
        {
            if ((currDepth >= skipCount) && (stackFrame.AddrPC.Offset != 0))
            {
                WindowsSymbol symbol(process, stackFrame);
                stackframes.EmplaceBack(symbol.GetFileName(), symbol.GetName(), symbol.GetAddress(),
                                        symbol.GetLine());
            }

            if (::StackWalk64(machineType, process, thread, &stackFrame, &context, nullptr,
                              SymFunctionTableAccess64, SymGetModuleBase64, nullptr) == FALSE)
            {
                break;
            }

            ++currDepth;
        }

        return StackTrace(Move(stackframes), Format("{0}", ::GetCurrentThreadId()));
    }
}
