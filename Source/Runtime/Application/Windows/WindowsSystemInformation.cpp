#include "Application/SystemInformation.h"

#include <cstring>
#include <intrin.h>
#include <Windows.h>

#include "Foundation/Common/Features.h"
#include "Foundation/Containers/Array.h"
#include "Foundation/String/UnicodeConversion.h"

#if defined(KITSUNE_COMPILER_SUPPORTS_WMI)
    #include <wrl.h>
    #include <comdef.h>
    #include <Wbemidl.h>
#endif

namespace Kitsune
{
#if defined(KITSUNE_COMPILER_SUPPORTS_WMI)
    // CoInitializeEx() should always be proceeded with CoUnintialize(), even if it returns S_FALSE.
    struct ComInitializer
    {
        inline ComInitializer(HRESULT& result)
        {
            result = ::CoInitializeEx(nullptr, COINIT_MULTITHREADED);
        }

        inline ~ComInitializer()
        {
            ::CoUninitialize();
        }
    };

    template<typename T>
    Array<T> QueryWmi(const WideStringView className, const WideStringView property)
    {
        using namespace Microsoft::WRL;

        HRESULT result;
        ComInitializer comInitializer_(result);

        if (FAILED(result))
            return { /* ... */ };

        result = ::CoInitializeSecurity(nullptr, -1, nullptr, nullptr,
                                        RPC_C_AUTHN_LEVEL_DEFAULT, RPC_C_IMP_LEVEL_IMPERSONATE,
                                        nullptr, EOAC_NONE, nullptr);

        if (FAILED(result))
            return { /* ... */ };

        ComPtr<IWbemLocator> wbemLocator;
        result = ::CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER, IID_IWbemLocator, &wbemLocator);

        if (FAILED(result))
            return { /* ... */ };

        ComPtr<IWbemServices> wbemServices;
        result = wbemLocator->ConnectServer(bstr_t("ROOT\\CIMV2"), nullptr, nullptr,
                                            nullptr, 0, nullptr, nullptr, &wbemServices);

        if (FAILED(result))
            return { /* ... */ };

        result = ::CoSetProxyBlanket(wbemServices.Get(), RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE,
                                     nullptr, RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE,
                                     nullptr, EOAC_NONE);

        if (FAILED(result))
            return { /* ... */ };

        ComPtr<IEnumWbemClassObject> enumerator;
        WideString query = WideString(L"SELECT ") + property + L" FROM " + className;

        result = wbemServices->ExecQuery(bstr_t("WQL"), bstr_t(query.Data()),
                                         WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
                                         nullptr, &enumerator);

        if (FAILED(result))
            return { /* ... */ };

        Array<T> wmiQuery;
        while (enumerator)
        {
            ULONG ret = 0;
            ComPtr<IWbemClassObject> classObject;

            result = enumerator->Next(WBEM_INFINITE, 1, &classObject, &ret);
            if (ret == 0)
                break;

            VARIANT variant;
            ::VariantInit(&variant);

            result = classObject->Get(property.Data(), 0, &variant, nullptr, nullptr);

            if constexpr (std::is_same_v<T, String>)
            {
                WideString str(variant.bstrVal, ::SysStringLen(variant.bstrVal));
                wmiQuery.PushBack(Unicode::ConvertString<wchar_t, char>(str));
            }
            else if constexpr (std::is_same_v<T, Uint32>)
                wmiQuery.PushBack(variant.ulVal);
            else if constexpr (std::is_same_v<T, Uint16>)
                wmiQuery.PushBack(variant.uiVal);
            else
            {
                static_assert(false, "Type is not supported.");
            }

            ::VariantClear(&variant);
        }

        return wmiQuery;
    }
#endif

#if defined(KITSUNE_ARCH_X86)
#pragma pack(push, 1)
    struct CpuInfo
    {
        Int32 Eax, Ebx, Ecx, Edx;
    };
#pragma pack(pop)

    inline void CallCpuIdInstruction(CpuInfo* cpuInfo, int eax, int ecx)
    {
        __cpuidex(reinterpret_cast<int*>(cpuInfo), eax, ecx);
    }
#endif

#if !defined(KITSUNE_COMPILER_SUPPORTS_WMI)
    String ReadRegistryKey(const wchar_t* path, const wchar_t* name)
    {
        LRESULT result;
        HKEY keyHandle;

        result = ::RegOpenKeyEx(HKEY_LOCAL_MACHINE, path, 0, KEY_READ, &keyHandle);
        if (result != ERROR_SUCCESS)
            return "<unknown>";

        DWORD dataSize;
        result = ::RegGetValueW(keyHandle, nullptr, name, RRF_RT_REG_SZ, nullptr, nullptr, &dataSize);

        if (result != ERROR_SUCCESS)
        {
            ::RegCloseKey(keyHandle);
            return "<unknown>";
        }

        WideString wideData(dataSize / sizeof(wchar_t), L'\0');
        ::RegGetValueW(keyHandle, nullptr, name, RRF_RT_REG_SZ, nullptr, wideData.Data(), &dataSize);

        ::RegCloseKey(keyHandle);
        return Unicode::ConvertString<wchar_t, char>(wideData);
    }
#endif

    // Got too lazy to rewrite this over and over again.
    inline BOOL GetGlobalMemoryStatus(MEMORYSTATUSEX* memoryStatus)
    {
        memoryStatus->dwLength = sizeof(MEMORYSTATUSEX);
        return ::GlobalMemoryStatusEx(memoryStatus);
    }

    String SystemInformation::GetCpuVendor()
    {
#if defined(KITSUNE_COMPILER_SUPPORTS_WMI)
        Array<String> wmiQuery = QueryWmi<String>(L"Win32_Processor", L"Manufacturer");
        if (wmiQuery.IsEmpty())
            return "<unknown>";

        String manufacturerId = wmiQuery[0];

#elif /* !defined(KITSUNE_COMPILER_SUPPORTS_WMI) && */ defined(KITSUNE_ARCH_X86)
        CpuInfo cpuInfo;
        CallCpuIdInstruction(&cpuInfo, /* Manufacturer ID */ 0, 0);

        // Maximum 12 characters, 3 registers * 4 bytes.
        String manufacturerId(12, '\0');
        std::memcpy(manufacturerId.Data(),     &cpuInfo.Ebx, sizeof(Int32));
        std::memcpy(manufacturerId.Data() + 4, &cpuInfo.Edx, sizeof(Int32));
        std::memcpy(manufacturerId.Data() + 8, &cpuInfo.Ecx, sizeof(Int32));
#else
        return "<unknown>";
#endif

        if (manufacturerId == "AuthenticAMD")      return "AMD";
        else if (manufacturerId == "GenuineIntel") return "Intel";
        else                                       return "<unknown>";
    }

    String SystemInformation::GetCpuDescription()
    {
#if defined(KITSUNE_COMPILER_SUPPORTS_WMI)
        auto wmiQuery = QueryWmi<String>(L"Win32_Processor", L"Name");
        if (wmiQuery.IsEmpty())
            return "<unknown>";

        return wmiQuery[0];
#else
        return ReadRegistryKey(L"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", L"ProcessorNameString");
#endif
    }

    Uint32 SystemInformation::GetLogicalCoreCount()
    {
#if defined(KITSUNE_COMPILER_SUPPORTS_WMI)
        auto wmiQuery = QueryWmi<Uint32>(L"Win32_Processor", L"NumberOfLogicalProcessors");
        if (wmiQuery.IsEmpty())
            return 0;

        return wmiQuery[0];
#else
        SYSTEM_INFO systemInfo;
        ::GetSystemInfo(&systemInfo);

        return systemInfo.dwNumberOfProcessors;
#endif
    }

    Uint32 SystemInformation::GetPhysicalCoreCount()
    {
#if defined(KITSUNE_COMPILER_SUPPORTS_WMI)
        auto wmiQuery = QueryWmi<Uint32>(L"Win32_Processor", L"NumberOfCores");
        if (wmiQuery.IsEmpty())
            return 0;

        return wmiQuery[0];
#else
        DWORD bufferSize = 0;
        KITSUNE_VERIFY(::GetLogicalProcessorInformation(nullptr, &bufferSize) == FALSE,
                       "GetLogicalProcessorInformation() should have failed here.");

        KITSUNE_VERIFY(::GetLastError() == ERROR_INSUFFICIENT_BUFFER,
                       "This should have been ERROR_INSUFFICIENT_BUFFER.");

        Array<SYSTEM_LOGICAL_PROCESSOR_INFORMATION> buffer(bufferSize / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION), SYSTEM_LOGICAL_PROCESSOR_INFORMATION());
        if (::GetLogicalProcessorInformation(buffer.Data(), &bufferSize) == FALSE)
            return 0;

        Uint32 physicalCoreCount = 0;
        for (auto& processorInfo : buffer)
        {
            if (processorInfo.Relationship == RelationProcessorCore)
                ++physicalCoreCount;
        }

        return physicalCoreCount;
#endif
    }

    CpuArchitecture SystemInformation::GetCpuArchitecture()
    {
#if defined(KITSUNE_COMPILER_SUPPORTS_WMI)
        auto wmiQuery = QueryWmi<Uint16>(L"Win32_Processor", L"Architecture");
        if (wmiQuery.IsEmpty())
            return CpuArchitecture::Unknown;

        Uint16 architecture = wmiQuery[0];
        switch (architecture)
        {
        case 0:  return CpuArchitecture::x86_32;
        case 9:  return CpuArchitecture::x86_64;
        case 5:  return CpuArchitecture::AArch32;
        case 12: return CpuArchitecture::AArch64;
        default:
            return CpuArchitecture::Unknown;
        }
#else
        SYSTEM_INFO systemInfo;
        ::GetSystemInfo(&systemInfo);

        switch (systemInfo.wProcessorArchitecture)
        {
        case PROCESSOR_ARCHITECTURE_AMD64: return CpuArchitecture::x86_64;
        case PROCESSOR_ARCHITECTURE_INTEL: return CpuArchitecture::x86_32;
        case PROCESSOR_ARCHITECTURE_ARM:   return CpuArchitecture::AArch32;
        case PROCESSOR_ARCHITECTURE_ARM64: return CpuArchitecture::AArch64;
        default:
            return CpuArchitecture::Unknown;
        }
#endif
    }

    CpuFeatures SystemInformation::GetCpuFeatures()
    {
#if defined(KITSUNE_ARCH_X86)
        // MMX - AVX(128) instructions support is all in EAX=1.
        // AVX2 - AVX512meow instructions support is continued in EAX=0x7, ECX=0-2
        CpuInfo cpuInfo;
        CallCpuIdInstruction(&cpuInfo, /* Processor Info & Feature Bits */ 1, 0);

        CpuFeatures features = CpuFeatures::None;
        if ((cpuInfo.Edx & (1 << 23)) != 0) features |= CpuFeatures::MMX;
        if ((cpuInfo.Edx & (1 << 25)) != 0) features |= CpuFeatures::SSE;
        if ((cpuInfo.Edx & (1 << 26)) != 0) features |= CpuFeatures::SSE2;

        if ((cpuInfo.Ecx & (1 << 0)) != 0)  features |= CpuFeatures::SSE3;
        if ((cpuInfo.Ecx & (1 << 9)) != 0)  features |= CpuFeatures::SSSE3;

        if ((cpuInfo.Ecx & (1 << 19)) != 0) features |= CpuFeatures::SSE4_1;
        if ((cpuInfo.Ecx & (1 << 20)) != 0) features |= CpuFeatures::SSE4_2;
        if ((cpuInfo.Ecx & (1 << 23)) != 0) features |= CpuFeatures::POPCNT;
        if ((cpuInfo.Ecx & (1 << 28)) != 0) features |= CpuFeatures::AVX;

        CallCpuIdInstruction(&cpuInfo, /* Extended Features */ 0x7, /* List 0 */ 0);
        if ((cpuInfo.Ebx & (1 << 5)) != 0)  features |= CpuFeatures::AVX2;
        if ((cpuInfo.Ebx & (1 << 16)) != 0) features |= CpuFeatures::AVX512_F;

        return features;
#else
        return CpuFeatures::None;
#endif
    }

    String SystemInformation::GetOperatingSystemName()
    {
#if defined(KITSUNE_COMPILER_SUPPORTS_WMI)
        auto wmiQuery = QueryWmi<String>(L"Win32_OperatingSystem", L"Name");
        if (wmiQuery.IsEmpty())
            return SystemInformation::GetOperatingSystemShortName();

        return wmiQuery[0];
#else
        using RtlGetVersionFunction = NTSTATUS (WINAPI*)(LPOSVERSIONINFOEXW);

        HMODULE ntdll = ::GetModuleHandleW(L"ntdll.dll");
        String currentVer = ReadRegistryKey(L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", L"ProductName");

        // The registry returns Windows 10 even if the OS installed is Windows 11.
        // This is done for backwards compatibility with old Win10 software.
        if (ntdll == nullptr)
            return currentVer;

        RtlGetVersionFunction rtlGetVersion = (RtlGetVersionFunction)(void*)(::GetProcAddress(ntdll, "RtlGetVersion"));
        if (rtlGetVersion == nullptr)
            return currentVer;

        OSVERSIONINFOEXW osVersionInfo;
        if (FAILED(rtlGetVersion(&osVersionInfo)))
            return currentVer;

        // Replace the Windows version number to 11.
        if ((osVersionInfo.dwMajorVersion == 10) && (osVersionInfo.dwBuildNumber >= 21996))
        {
            char* ptr = std::strstr(currentVer.Raw(), "10");        // TODO: Replace this in the future.
            if (ptr != nullptr)
                *++ptr = '1';
        }

        return currentVer;
#endif
    }

    String SystemInformation::GetOperatingSystemShortName()
    {
#if defined(KITSUNE_COMPILER_SUPPORTS_WMI)
        auto wmiQuery = QueryWmi<Uint16>(L"Win32_OperatingSystem", L"OSType");
        if (wmiQuery.IsEmpty())
            return "Windows";

        switch (wmiQuery[0])
        {
        case 15: return "Windows 3.x";
        case 16: return "Windows 95";
        case 17: return "Windows 98";
        case 18: return "Windows NT";
        case 19: return "Windows Embedded";
        default:
            return "Windows";
        }
#else
        return "Windows";
#endif
    }

    bool SystemInformation::OnBattery()
    {
        SYSTEM_POWER_STATUS powerStatus;
        ::GetSystemPowerStatus(&powerStatus);

        return (powerStatus.ACLineStatus == 0);
    }

    Uint64 SystemInformation::GetTotalPhysicalMemory()
    {
        MEMORYSTATUSEX memoryStatus;
        if (GetGlobalMemoryStatus(&memoryStatus))
            return memoryStatus.ullTotalPhys;

        return 0;
    }

    Uint64 SystemInformation::GetAvailablePhysicalMemory()
    {
        MEMORYSTATUSEX memoryStatus;
        if (GetGlobalMemoryStatus(&memoryStatus))
            return memoryStatus.ullAvailPhys;

        return 0;
    }

    Uint64 SystemInformation::GetTotalVirtualMemory()
    {
        MEMORYSTATUSEX memoryStatus;
        if (GetGlobalMemoryStatus(&memoryStatus))
            return memoryStatus.ullTotalVirtual;

        return 0;
    }

    Uint64 SystemInformation::GetAvailableVirtualMemory()
    {
        MEMORYSTATUSEX memoryStatus;
        if (GetGlobalMemoryStatus(&memoryStatus))
            return memoryStatus.ullAvailVirtual;

        return 0;
    }
}
