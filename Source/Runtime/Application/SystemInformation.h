#pragma once

#include "Foundation/String/String.h"
#include "Foundation/Utilities/EnumFlags.h"

namespace Kitsune
{
    enum class CpuArchitecture
    {
        Unknown,

        x86_32,
        x86_64,
        AArch32,
        AArch64
    };

    enum class CpuFeatures : Uint64
    {
        None     = 0,
        MMX      = 1 << 0,
        SSE      = 1 << 1,
        SSE2     = 1 << 2,
        SSE3     = 1 << 3,
        SSSE3    = 1 << 4,
        SSE4_1   = 1 << 5,
        SSE4_2   = 1 << 6,
        POPCNT   = 1 << 7,
        AVX      = 1 << 8,
        AVX2     = 1 << 9,
        AVX512_F = 1 << 10
    };

    KITSUNE_OVERLOAD_FLAGS_OPERATORS(CpuFeatures);

    class SystemInformation
    {
    public:
        static String GetCpuVendor();
        static String GetCpuDescription();

        static Uint32 GetLogicalCoreCount();
        static Uint32 GetPhysicalCoreCount();

        static CpuArchitecture GetCpuArchitecture();
        static CpuFeatures GetCpuFeatures();

    public:
        static String GetOperatingSystemName();
        static String GetOperatingSystemShortName();

    public:
        static bool OnBattery();

    public:
        static Uint64 GetTotalPhysicalMemory();
        static Uint64 GetAvailablePhysicalMemory();

        static Uint64 GetTotalVirtualMemory();
        static Uint64 GetAvailableVirtualMemory();
    };
}
