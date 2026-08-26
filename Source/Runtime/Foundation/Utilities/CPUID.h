#pragma once

#include "Foundation/Common/Types.h"
#include "Foundation/Common/Predefined.h"

#if defined(KITSUNE_ARCH_X86)
    #if defined(KITSUNE_COMPILER_MSVC)
        #include <intrin.h>
    #else /* compiler == GCC or compiler == Clang */
        #include <cpuid.h>
    #endif
#else
    #error CPUID.h is only supported when compiling programs targetting x86.
#endif

namespace Kitsune
{
    struct CPUIDResult
    {
        unsigned int EAX, EBX, ECX, EDX;
    };

    // Returns the result of calling the CPUID instruction. Returns the values
    // of the registers EAX, EBX, ECX, and EDX.
    inline CPUIDResult CallCPUID(Int32 eax, Int32 ecx)
    {
        unsigned int cpuInfo[4];
#if defined(KITSUNE_COMPILER_MSVC)
        __cpuidex(reinterpret_cast<int*>(cpuInfo), eax, ecx);
        return CPUIDResult(cpuInfo[0], cpuInfo[1], cpuInfo[2], cpuInfo[3]);
#else
        __get_cpuid_count(
            eax, ecx,
            cpuInfo,
            cpuInfo + 1,
            cpuInfo + 2,
            cpuInfo + 3);

        return CPUIDResult(cpuInfo[0], cpuInfo[1], cpuInfo[2], cpuInfo[3]);
#endif
    }
}
