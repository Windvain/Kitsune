#pragma once

#include "Foundation/String/String.h"
#include "Foundation/Containers/Array.h"

#include "Foundation/Utilities/EnumFlags.h"
#include "Foundation/Utilities/NonCopyable.h"

namespace Kitsune
{
    // The architecture of the CPU.
    enum class CpuArchitecture
    {
        Other,      //< The architecture is unknown or is not included in this enum.
        x86_32,     //< CPU architecture is the 32-bit version of x86.
        x86_64,     //< CPU architecture is x86_64, or better known by x64 or AMD64.
        AArch32,    //< Architecture matches ARMv7 and below.
        AArch64     //< Architecture matches all ARMv8 CPUs and above.
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

    // Contains information regarding the system's CPU.
    class CpuInformation
    {
    public:
        [[nodiscard]] inline String Vendor() const { return m_Vendor; }
        [[nodiscard]] inline String Description() const { return m_Description; }

        [[nodiscard]]
        inline Uint32 LogicalCoreCount() const { return m_LogicalCores; }

        [[nodiscard]]
        inline Uint32 PhysicalCoreCount() const { return m_PhysicalCores; }

        [[nodiscard]]
        inline CpuArchitecture Architecture() const
        {
            return m_Architecture;
        }

        [[nodiscard]]
        inline Uint32 ArchitectureBitWidth() const
        {
            switch (m_Architecture)
            {
            case CpuArchitecture::x86_32:   [[fallthrough]];
            case CpuArchitecture::AArch32:
                return 32;

            case CpuArchitecture::x86_64:   [[fallthrough]];
            case CpuArchitecture::AArch64:
                return 64;

            case CpuArchitecture::Other:
                return 0;
            }

            KITSUNE_UNREACHABLE();
        }

    private:
        friend class SystemInformation;
        CpuInformation() = default;

    private:
        String m_Vendor;
        String m_Description;

        Uint32 m_LogicalCores = 0;
        Uint32 m_PhysicalCores = 0;

        CpuArchitecture m_Architecture = CpuArchitecture::Other;
    };

    // Contains information regarding the operating system.
    class OperatingSystemInformation
    {
    public:
        [[nodiscard]] inline String Name() const { return m_Name; }
        [[nodiscard]] inline String ShortName() const { return m_ShortName; }

        [[nodiscard]]
        inline Uint32 ArchitectureBitWidth() const { return m_ArchitectureBits; }

    private:
        friend class SystemInformation;
        OperatingSystemInformation() = default;

    private:
        String m_Name = "<unknown>";
        String m_ShortName = "<unknown>";

        Uint32 m_ArchitectureBits = 0;
    };

    // Contains information about the battery state.
    class BatteryInformation
    {
    public:
        [[nodiscard]] inline bool OnBattery() const { return m_OnBattery; }
        [[nodiscard]] inline bool UsesBattery() const { return m_UsesBattery; }

        [[nodiscard]]
        inline Uint8 ChargePercentage() const
        {
            return m_ChargePercentage;
        }

    private:
        friend class SystemInformation;
        BatteryInformation() = default;

    private:
        bool m_OnBattery = false;
        bool m_UsesBattery = false;

        Uint8 m_ChargePercentage = 100;
    };

    // Obtains the running system's information. Currently, this class only
    // has full functionality under the x86 architecture.
    class KITSUNE_API SystemInformation : public NonCopyable
    {
    public:
        static Array<CpuInformation> GetCpuInformation();
        static OperatingSystemInformation GetOperatingSystemInformation();

        static BatteryInformation GetBatteryInformation();

    public:
        static CpuFeatures GetCpuFeatures();
    };
}
