#include "Foundation/Diagnostics/StackTrace.h"

#include <Windows.h>

#include <Psapi.h>
#include <DbgHelp.h>

#include "Foundation/Diagnostics/MsvcStackFrame.h"
#include "Foundation/Diagnostics/StackTraceException.h"

namespace Kitsune::Details
{
    class ScopedSymInit
    {
    public:
        ScopedSymInit(DWORD options)
        {
            if (::SymInitializeW(::GetCurrentProcess(), nullptr, false) == FALSE)
                throw StackTraceException("Failed to initialize symbol handler.");

            ::SymSetOptions(::SymGetOptions() | options);
        }

        ~ScopedSymInit()
        {
            ::SymCleanup(::GetCurrentProcess());
        }
    };

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

    inline DWORD GetMachineType()
    {
#if defined(KITSUNE_ARCH_X86_64)
        return IMAGE_FILE_MACHINE_AMD64;
#else
        #error StackTrace is only implemented for x64.
#endif
    }

    void LoadModulesSymbols()
    {
        HANDLE process = ::GetCurrentProcess();

        DWORD bytesNeeded;
        ::EnumProcessModules(process, nullptr, 0, &bytesNeeded);

        Array<HMODULE> moduleHandles(bytesNeeded / sizeof(HMODULE), HMODULE());
        ::EnumProcessModules(process, moduleHandles.Data(), DWORD(moduleHandles.Size() * sizeof(HMODULE)),
            &bytesNeeded);

        constexpr Usize BufferSize = 1024;
        wchar_t tempBuffer[BufferSize];

        for (HMODULE module : moduleHandles)
        {
            MODULEINFO moduleInfo;
            ::GetModuleInformation(process, module, &moduleInfo, sizeof(moduleInfo));

            WideString imageName;
            WideString moduleName;

            ::GetModuleFileNameW(module, tempBuffer, BufferSize);
            imageName = tempBuffer;

            ::GetModuleBaseNameW(process, module, tempBuffer, BufferSize);
            moduleName = tempBuffer;

            if (::SymLoadModuleExW(process, nullptr, imageName.Data(), moduleName.Data(),
                reinterpret_cast<DWORD64>(moduleInfo.lpBaseOfDll), moduleInfo.SizeOfImage, nullptr, 0) == 0)
            {
                throw StackTraceException("Could not load modules for the specified process.");
            }
        }
    }

    void DoBackTrace(Usize skipCount, Usize maxDepth,
                     StackTraceFillCallback callback, void* data)
    {
        ++skipCount;        // Skip the function that we are currently in.
        ScopedSymInit handler_(SYMOPT_LOAD_LINES | SYMOPT_EXACT_SYMBOLS);

        CONTEXT context;
        ::RtlCaptureContext(&context);

        STACKFRAME64 stackFrame = InitializeStackFrame(&context);
        DWORD machineType = GetMachineType();

        HANDLE process = ::GetCurrentProcess();
        HANDLE thread = ::GetCurrentThread();

        LoadModulesSymbols();

        while (maxDepth != 0)
        {
            --maxDepth;
            if (skipCount != 0)
                --skipCount;
            else if (stackFrame.AddrPC.Offset != 0)
            {
                auto engineStackFrame = MakeShared<MsvcStackFrame>(
                    process, stackFrame.AddrPC.Offset);

                callback(Move(engineStackFrame), data);
            }

            if (::StackWalk64(machineType, process, thread, &stackFrame, &context, nullptr,
                              SymFunctionTableAccess64, SymGetModuleBase64, nullptr) == FALSE)
            {
                break;
            }
        }
    }
}
