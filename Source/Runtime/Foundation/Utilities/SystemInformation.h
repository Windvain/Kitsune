#pragma once

#include "Foundation/String/String.h"

#include "Foundation/Utilities/EnumFlags.h"
#include "Foundation/Utilities/NonCopyable.h"

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

    // CPU additional features. Only x86 architecture features are listed
    // for now, as there are no plans to support ARM CPUs.
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

    struct CpuInformation
    {
        CpuFeatures Features;
        CpuArchitecture Architecture;

        String Vendor;
        String Description;

        Uint32 LogicalCoreCount;
        Uint32 PhysicalCoreCount;
    };

    struct OsInformation
    {
        String Name;
        String ShortName;
    };

    struct BatteryInformation
    {
        bool OnBattery;
        bool UsesBattery;

        Uint8 ChargePercentage;
    };

    struct MemoryStatusInformation
    {
        Uint64 TotalPhysicalMemory;
        Uint64 AvailablePhysicalMemory;
        Uint64 TotalVirtualMemory;
        Uint64 AvailableVirtualMemory;
    };

    // Obtains the running system's information. Currently, this class only
    // has full functionality under x86. ARM will still compile (?) but will
    // return `<unknown>` for many functions.
    class SystemInformation : public NonCopyable
    {
    public:
        static CpuInformation GetCpuInformation();
        static OsInformation GetOperatingSystemInformation();

        static BatteryInformation GetBatteryInformation();
        static MemoryStatusInformation GetCurrentMemoryStatus();

    private:
        static CpuFeatures GetCpuFeatures_();
        static StringView TranslateX86VendorString_(const StringView manufacturer);
    };
}
