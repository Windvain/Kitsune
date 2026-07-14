#include "Foundation/Utilities/SystemInformation.h"

#if defined(KITSUNE_ARCH_X86)
    #include "Foundation/Utilities/CPUID.h"
#endif

namespace Kitsune
{
    CPUFeatures SystemInformation::GetCPUFeatures()
    {
        CPUFeatures features = CPUFeatures::None;

#if defined(KITSUNE_ARCH_X86)
        // MMX - AVX(128) instructions support is all in EAX=1.
        // AVX2 - AVX512meow instructions support is continued in EAX=0x7, ECX=0-2
        CPUIDResult cpuInfo = CallCPUID(/* Processor Info & Feature Bits */ 1, 0);
        if ((cpuInfo.EDX & (1 << 23)) != 0) features |= CPUFeatures::MMX;
        if ((cpuInfo.EDX & (1 << 25)) != 0) features |= CPUFeatures::SSE;
        if ((cpuInfo.EDX & (1 << 26)) != 0) features |= CPUFeatures::SSE2;

        if ((cpuInfo.ECX & (1 << 0)) != 0)  features |= CPUFeatures::SSE3;
        if ((cpuInfo.ECX & (1 << 9)) != 0)  features |= CPUFeatures::SSSE3;

        if ((cpuInfo.ECX & (1 << 19)) != 0) features |= CPUFeatures::SSE4_1;
        if ((cpuInfo.ECX & (1 << 20)) != 0) features |= CPUFeatures::SSE4_2;
        if ((cpuInfo.ECX & (1 << 23)) != 0) features |= CPUFeatures::POPCNT;
        if ((cpuInfo.ECX & (1 << 28)) != 0) features |= CPUFeatures::AVX;

        cpuInfo = CallCPUID(/* Extended Features */ 0x7, /* List 0 */ 0);
        if ((cpuInfo.EBX & (1 << 5)) != 0)  features |= CPUFeatures::AVX2;
        if ((cpuInfo.EBX & (1 << 16)) != 0) features |= CPUFeatures::AVX512_F;
#endif

        return features;
    }
}
