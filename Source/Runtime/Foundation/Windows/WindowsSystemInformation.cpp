#include "Foundation/Utilities/SystemInformation.h"

#include <Windows.h>
#include "Foundation/Containers/Array.h"

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
        LRESULT result;
        HKEY keyHandle;

        result = ::RegOpenKeyExW(HKEY_LOCAL_MACHINE, path, 0, KEY_READ, &keyHandle);
        if (result != ERROR_SUCCESS)
            throw SystemException("Failed to open a registry key.");

        DWORD dataSize;
        result = ::RegGetValueW(keyHandle, nullptr, name, RRF_RT_REG_SZ,
                                nullptr, nullptr, &dataSize);

        if (result != ERROR_SUCCESS)
        {
            ::RegCloseKey(keyHandle);
            throw SystemException("Failed to get the amount of bytes needed to store "
                                  "a registry key's contents.");
        }

        WideString wideData(dataSize / sizeof(wchar_t), L'\0');
        KITSUNE_VERIFY(::RegGetValueW(keyHandle, nullptr, name, RRF_RT_REG_SZ, nullptr,
                                      wideData.Data(), &dataSize) == ERROR_SUCCESS,
                       "Failed to retrieve a registry key's value. "
                       "This function should have failed on the first call.");

        ::RegCloseKey(keyHandle);
        return Utf16ToUtf8<wchar_t, char>(wideData);
    }

    CpuInformation SystemInformation::GetCpuInformation()
    {
        CpuInformation cpuInfo;

        // Get architecture and logical core count. (GetSystemInfo)
        SYSTEM_INFO systemInfo;
        ::GetSystemInfo(&systemInfo);

        switch (systemInfo.wProcessorArchitecture)
        {
        case PROCESSOR_ARCHITECTURE_INTEL:
            cpuInfo.Architecture = CpuArchitecture::x86_32;
            break;
        case PROCESSOR_ARCHITECTURE_AMD64:
            cpuInfo.Architecture = CpuArchitecture::x86_64;
            break;
        case PROCESSOR_ARCHITECTURE_ARM:
            cpuInfo.Architecture = CpuArchitecture::AArch32;
            break;
        case PROCESSOR_ARCHITECTURE_ARM64:
            cpuInfo.Architecture = CpuArchitecture::AArch64;
            break;
        default:
            cpuInfo.Architecture = CpuArchitecture::Unknown;
        }

        cpuInfo.LogicalCoreCount = systemInfo.dwNumberOfProcessors;

        // Get vendor and features. (CPUID instruction)
#if defined(KITSUNE_ARCH_X86)
        {
            CpuIdResult cpuIdResult = CallCpuId(/* Manufacturer ID */ 0, 0);

            // Maximum 12 characters, 3 registers * 4 bytes.
            String manufacturerId(12, '\0');
            std::memcpy(manufacturerId.Data(),     &cpuIdResult.Ebx, sizeof(Int32));
            std::memcpy(manufacturerId.Data() + 4, &cpuIdResult.Edx, sizeof(Int32));
            std::memcpy(manufacturerId.Data() + 8, &cpuIdResult.Ecx, sizeof(Int32));

            cpuInfo.Vendor = TranslateX86VendorString(manufacturerId);
            cpuInfo.Features = GetCpuFeatures();
        }
#else
        cpuInfo.Vendor = "<unknown>";
        cpuInfo.Features = CpuFeatures::None;
#endif

        // Get description. (Registry)
        cpuInfo.Description = ReadStringRegistryKey(
            L"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0",
            L"ProcessorNameString");

        // Get the CPU's physical core count.
        DWORD bufferSize = 0;
        KITSUNE_VERIFY(!::GetLogicalProcessorInformation(nullptr, &bufferSize),
                       "GetLogicalProcessorInformation() should have failed when "
                       "we tried to obtain the required buffer size.");

        Array<SYSTEM_LOGICAL_PROCESSOR_INFORMATION> buffer(
            bufferSize / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION),
            SYSTEM_LOGICAL_PROCESSOR_INFORMATION());

        // If we fail to get the processor information, just use the logical core count.
        // It's a safe bet.
        if (!::GetLogicalProcessorInformation(buffer.Data(), &bufferSize))
            cpuInfo.PhysicalCoreCount = cpuInfo.LogicalCoreCount;

        Uint32 physicalCoreCount = 0;
        for (auto& processorInfo : buffer)
        {
            if (processorInfo.Relationship == RelationProcessorCore)
                ++physicalCoreCount;
        }

        cpuInfo.PhysicalCoreCount = physicalCoreCount;
        return cpuInfo;
    }

    OsInformation SystemInformation::GetOperatingSystemInformation()
    {
        using RtlGetVersionFunction = NTSTATUS (WINAPI*)(LPOSVERSIONINFOEXW);
        OsInformation osInfo;

        HMODULE ntdll = ::GetModuleHandleW(L"ntdll.dll");
        String currentVersion = ReadStringRegistryKey(
            L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion",
            L"ProductName");

        // HACK: The registry returns 10 for both Windows 10 and 11 for backwards
        // compatibility with old Win10 software. If the build number indicates Win11,
        // just replace the 10 --> 11.
        if (ntdll == nullptr)
        {
            throw SystemException("Failed to obtain handle to ntdll.dll. Please make "
                                  "sure that the application has already been linked "
                                  "to ntdll.dll.");
        }

        auto rtlGetVersion = (RtlGetVersionFunction)(void*)(
            ::GetProcAddress(ntdll, "RtlGetVersion"));

        if (rtlGetVersion == nullptr)
            throw SystemException("Failed to get the address of RtlGetVersion().");

        OSVERSIONINFOEXW osVersionInfo;
        if (SUCCEEDED(rtlGetVersion(&osVersionInfo)))
        {
            if ((osVersionInfo.dwMajorVersion == 10) &&
                (osVersionInfo.dwBuildNumber >= 21996))
            {
                auto iterator = currentVersion.Find("10");
                if (iterator != currentVersion.GetEnd())
                    *++iterator = '1';
            }
        }

        return OsInformation{ .Name = currentVersion, .ShortName = "Windows" };
    }

    BatteryInformation SystemInformation::GetBatteryInformation()
    {
        BatteryInformation batteryInfo;
        SYSTEM_POWER_STATUS systemPowerStatus;

        if (!::GetSystemPowerStatus(&systemPowerStatus))
            throw SystemException("Failed to obtain system power status.");

        batteryInfo.OnBattery = (systemPowerStatus.ACLineStatus == 0);
        batteryInfo.UsesBattery = (systemPowerStatus.BatteryFlag != 128) &&
                                  (systemPowerStatus.BatteryFlag != 255);

        // A value of 255 for the BatteryLifePercent member variable means that
        // the status of charge is unknown.
        batteryInfo.ChargePercentage = KITSUNE_MIN(systemPowerStatus.BatteryLifePercent, 100);
        return batteryInfo;
    }

    MemoryStatusInformation SystemInformation::GetCurrentMemoryStatus()
    {
        MEMORYSTATUSEX memoryStatus;
        memoryStatus.dwLength = sizeof(memoryStatus);

        if (!::GlobalMemoryStatusEx(&memoryStatus))
        {
            throw SystemException("Failed to obtain system memory status via "
                                  "GlobalMemoryStatusEx().");
        }

        // MEMORYSTATUSEX returns memory statuses in bytes. Our engine's memory status
        // needs it in kiloBYTES.
        return MemoryStatusInformation{
            .TotalPhysicalMemory = memoryStatus.ullTotalPhys / 1000,
            .AvailablePhysicalMemory = memoryStatus.ullAvailPhys / 1000,
            .TotalVirtualMemory = memoryStatus.ullTotalVirtual / 1000,
            .AvailableVirtualMemory = memoryStatus.ullAvailVirtual / 1000
        };
    }
}
