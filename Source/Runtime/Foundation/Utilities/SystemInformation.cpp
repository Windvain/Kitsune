#include "Foundation/Utilities/SystemInformation.h"
#include "Foundation/Common/Predefined.h"

#if defined(KITSUNE_ARCH_X86)
    #include "Foundation/Utilities/CpuId.h"
#endif

namespace Kitsune
{
    CpuFeatures SystemInformation::GetCpuFeatures()
    {
        CpuFeatures features = CpuFeatures::None;

#if defined(KITSUNE_ARCH_X86)
        // MMX - AVX(128) instructions support is all in EAX=1.
        // AVX2 - AVX512meow instructions support is continued in EAX=0x7, ECX=0-2
        CpuIdResult cpuInfo = CallCpuId(/* Processor Info & Feature Bits */ 1, 0);
        if ((cpuInfo.Edx & (1 << 23)) != 0) features |= CpuFeatures::MMX;
        if ((cpuInfo.Edx & (1 << 25)) != 0) features |= CpuFeatures::SSE;
        if ((cpuInfo.Edx & (1 << 26)) != 0) features |= CpuFeatures::SSE2;

        if ((cpuInfo.Ecx & (1 << 0)) != 0)  features |= CpuFeatures::SSE3;
        if ((cpuInfo.Ecx & (1 << 9)) != 0)  features |= CpuFeatures::SSSE3;

        if ((cpuInfo.Ecx & (1 << 19)) != 0) features |= CpuFeatures::SSE4_1;
        if ((cpuInfo.Ecx & (1 << 20)) != 0) features |= CpuFeatures::SSE4_2;
        if ((cpuInfo.Ecx & (1 << 23)) != 0) features |= CpuFeatures::POPCNT;
        if ((cpuInfo.Ecx & (1 << 28)) != 0) features |= CpuFeatures::AVX;

        cpuInfo = CallCpuId(/* Extended Features */ 0x7, /* List 0 */ 0);
        if ((cpuInfo.Ebx & (1 << 5)) != 0)  features |= CpuFeatures::AVX2;
        if ((cpuInfo.Ebx & (1 << 16)) != 0) features |= CpuFeatures::AVX512_F;
#endif

        return features;
    }

    StringView SystemInformation::TranslateX86VendorString(const StringView manufacturer)
    {
        if (manufacturer == "GenuineIntel")      return "Intel";
        else if (manufacturer == "AuthenticAMD") return "AMD";
        else                                     return "<unknown>";
    }
}
