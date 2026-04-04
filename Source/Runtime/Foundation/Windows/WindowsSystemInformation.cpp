#include "Foundation/Utilities/SystemInformation.h"
#include <Windows.h>

#include "Foundation/Common/Predefined.h"
#include "Foundation/Diagnostics/Assert.h"

#include "Foundation/String/TranscodePresets.h"
#include "Foundation/Diagnostics/SystemException.h"

#if defined(KITSUNE_ARCH_X86)
    #include "Foundation/Utilities/CpuId.h"
#endif

namespace Kitsune
{
    static String ReadStringRegistryKey(const wchar_t* path, const wchar_t* name)
    {
        HKEY keyHandle;
        LRESULT result = ::RegOpenKeyExW(
            HKEY_LOCAL_MACHINE, path, 0, KEY_READ, &keyHandle);

        if (result != ERROR_SUCCESS)
            throw SystemException("Failed to open a registry key.");

        DWORD dataSize;
        result = ::RegGetValueW(
            keyHandle, nullptr, name, RRF_RT_REG_SZ,
            nullptr, nullptr, &dataSize);

        if (result != ERROR_SUCCESS)
        {
            ::RegCloseKey(keyHandle);
            throw SystemException(
                "Failed to get the amount of bytes needed to store "
                "a registry key's contents.");
        }

        WideString wideData(dataSize / sizeof(wchar_t), L'\0');
        KITSUNE_VERIFY(
            ::RegGetValueW(
                keyHandle, nullptr, name, RRF_RT_REG_SZ, nullptr,
                wideData.Data(), &dataSize) == ERROR_SUCCESS,
            "Failed to retrieve a registry key's value. "
            "This function should have failed on the first call.");

        ::RegCloseKey(keyHandle);
        return Utf16ToUtf8<wchar_t, char>(wideData);
    }

    Array<CpuInformation> SystemInformation::GetCpuInformation()
    {
        CpuInformation cpuInformation;

        // Get architecture and logical core count. (GetSystemInfo)
        SYSTEM_INFO systemInfo;
        ::GetSystemInfo(&systemInfo);

        switch (systemInfo.wProcessorArchitecture)
        {
        case PROCESSOR_ARCHITECTURE_INTEL:
            cpuInformation.m_Architecture = CpuArchitecture::x86_32;
            break;
        case PROCESSOR_ARCHITECTURE_AMD64:
            cpuInformation.m_Architecture = CpuArchitecture::x86_64;
            break;
        case PROCESSOR_ARCHITECTURE_ARM:
            cpuInformation.m_Architecture = CpuArchitecture::AArch32;
            break;
        case PROCESSOR_ARCHITECTURE_ARM64:
            cpuInformation.m_Architecture = CpuArchitecture::AArch64;
            break;
        default:
            cpuInformation.m_Architecture = CpuArchitecture::Other;
        }

        cpuInformation.m_LogicalCores = systemInfo.dwNumberOfProcessors;

        // Get vendor and features. (CPUID instruction)
#if defined(KITSUNE_ARCH_X86)
        {
            CpuIdResult cpuIdResult = CallCpuId(/* Manufacturer ID */ 0, 0);
            String manufacturer(3 * sizeof(Int32), '\0');

            std::memcpy(manufacturer.Data(), &cpuIdResult.Ebx, sizeof(Int32));
            std::memcpy(manufacturer.Data() + 4, &cpuIdResult.Edx, sizeof(Int32));
            std::memcpy(manufacturer.Data() + 8, &cpuIdResult.Ecx, sizeof(Int32));

            cpuInformation.m_Vendor = manufacturer;
        }
#endif

        // Get description. (Registry)
        cpuInformation.m_Description = ReadStringRegistryKey(
            L"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0",
            L"ProcessorNameString");

        // Get the CPU's physical core count.
        DWORD bufferSize = 0;
        KITSUNE_VERIFY(
            !::GetLogicalProcessorInformation(nullptr, &bufferSize),
            "GetLogicalProcessorInformation() should have failed when "
            "we tried to obtain the required buffer size.");

        Array<SYSTEM_LOGICAL_PROCESSOR_INFORMATION> buffer(
            bufferSize / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION),
            SYSTEM_LOGICAL_PROCESSOR_INFORMATION());

        // If we fail to get the processor information, just use the logical
        // core count, it's a safe bet.
        if (!::GetLogicalProcessorInformation(buffer.Data(), &bufferSize))
            cpuInformation.m_PhysicalCores = cpuInformation.m_LogicalCores;

        Uint32 physicalCoreCount = 0;
        for (auto& processorInfo : buffer)
        {
            if (processorInfo.Relationship == RelationProcessorCore)
                ++physicalCoreCount;
        }

        cpuInformation.m_PhysicalCores = physicalCoreCount;

        // Without using WMI, we can only get information about one CPU.
        return { cpuInformation };
    }

    OperatingSystemInformation SystemInformation::GetOperatingSystemInformation()
    {
        using RtlGetVersionFunc = NTSTATUS (WINAPI*)(LPOSVERSIONINFOEXW);
        OperatingSystemInformation osInformation;

        HMODULE ntdll = ::GetModuleHandleW(L"ntdll.dll");
        String fixedName = ReadStringRegistryKey(
            L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion",
            L"ProductName");

        // HACK: The registry returns 10 for both Windows 10 and 11 for
        // backwards compatibility with old Win10 software. If the build
        // number indicates Win11, just replace the 10 --> 11.
        if (ntdll == nullptr)
        {
            throw SystemException(
                "Failed to obtain handle to ntdll.dll. Please make sure "
                "that the application has already been linked to ntdll.dll.");
        }

        auto rtlGetVersion = (RtlGetVersionFunc)(void*)(::GetProcAddress(
            ntdll, "RtlGetVersion"));

        if (rtlGetVersion == nullptr)
            throw SystemException("Failed to get the address of RtlGetVersion().");

        OSVERSIONINFOEXW osVersionInfo;
        if (SUCCEEDED(rtlGetVersion(&osVersionInfo)))
        {
            if ((osVersionInfo.dwMajorVersion == 10) &&
                (osVersionInfo.dwBuildNumber >= 21996))
            {
                auto iterator = fixedName.Find("10");
                if (iterator != fixedName.GetEnd())
                    *++iterator = '1';
            }
        }

        osInformation.m_Name = fixedName;
        osInformation.m_ShortName = "Windows";

#if defined(KITSUNE_OS_WINDOWS_64_BIT)
        osInformation.m_ArchitectureBits = 64;
#else /* defined(KITSUNE_OS_WINDOWS_32_BIT) */
        osInformation.m_ArchitectureBits = 32;
#endif

        return osInformation;
    }

    BatteryInformation SystemInformation::GetBatteryInformation()
    {
        BatteryInformation batteryInformation;
        SYSTEM_POWER_STATUS powerStatus;

        if (!::GetSystemPowerStatus(&powerStatus))
            throw SystemException("Failed to obtain system power status.");

        batteryInformation.m_OnBattery = (powerStatus.ACLineStatus == 0);
        batteryInformation.m_UsesBattery =
            (powerStatus.BatteryFlag != 128) &&
            (powerStatus.BatteryFlag != 255);

        // A value of 255 for the BatteryLifePercent member variable means that
        // the status of charge is unknown.
        batteryInformation.m_ChargePercentage =
            Maths::Minimum(powerStatus.BatteryLifePercent, BYTE(100));

        return batteryInformation;
    }
}
