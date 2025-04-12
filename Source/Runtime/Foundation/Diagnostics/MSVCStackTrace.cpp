#include "Foundation/Diagnostics/StackTrace.h"
#include <Windows.h>

#include <Psapi.h>
#include <DbgHelp.h>

#include "Foundation/String/Format.h"
#include "Foundation/Memory/ScopedPtr.h"

#include "Foundation/Diagnostics/StackTraceException.h"

namespace Kitsune
{
    struct WindowsSymbol
    {
        static constexpr ULONG MaxNameLength = 1024;

        SYMBOL_INFO SymbolInfo;
        TCHAR Buffer[MaxNameLength];
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
        ::EnumProcessModules(process, moduleHandles.Data(), static_cast<DWORD>(moduleHandles.Size() * sizeof(HMODULE)),
                             &bytesNeeded);

        Array<WindowsModuleInfo> moduleInfos(moduleHandles.Size());
        for (HMODULE module : moduleHandles)
        {
            WindowsModuleInfo modInfo;
            MODULEINFO winModInfo;

            ::GetModuleInformation(process, module, &winModInfo, sizeof(winModInfo));
            modInfo.Address = winModInfo.lpBaseOfDll;
            modInfo.LoadSize = winModInfo.SizeOfImage;

            wchar_t temp[1024] = { 0 };

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
            if (currDepth >= skipCount)
            {
                if (stackFrame.AddrPC.Offset != 0)
                {
                    constexpr char UnknownString[] = "<unknown>";

                    auto symbol = MakeScoped<WindowsSymbol>();
                    SYMBOL_INFO* winSymbol = &symbol->SymbolInfo;

                    const char* filename = UnknownString;
                    DWORD lineNumber = 0;
                    void* address = (void*)0;

                    ZeroMemory(symbol.Get(), sizeof(WindowsSymbol));
                    winSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
                    winSymbol->MaxNameLen = WindowsSymbol::MaxNameLength;

                    ::SymFromAddr(process, stackFrame.AddrPC.Offset, nullptr, winSymbol);

                    IMAGEHLP_LINE64 line;
                    line.SizeOfStruct = sizeof(line);

                    DWORD offset;
                    if (::SymGetLineFromAddr64(process, stackFrame.AddrPC.Offset, &offset, &line))
                    {
                        filename = line.FileName;
                        lineNumber = line.LineNumber;
                        address = reinterpret_cast<void*>(line.Address);
                    }

                    // Should fit in name buffer?
                    if (std::strlen(winSymbol->Name) == 0)
                        std::memcpy(winSymbol->Name, UnknownString, sizeof(UnknownString) / sizeof(char));

                    stackframes.PushBack(StackFrame(winSymbol->Name, filename, address, lineNumber));
                }
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
