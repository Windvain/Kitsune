#include "Foundation/Utilities/SystemInformation.h"

#include <wrl.h>
#include <comdef.h>
#include <Wbemidl.h>

#include "Foundation/Diagnostics/Assert.h"
#include "Foundation/Diagnostics/SystemException.h"

#include "Foundation/String/TranscodePresets.h"

template<typename T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

namespace Kitsune
{
    struct ComInitializer
    {
        inline ComInitializer()
        {
            if (FAILED(::CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED)))
                throw SystemException("Failed to initialize the COM library.");

            // Initialize the COM library.
            HRESULT result;
            result = ::CoInitializeSecurity(nullptr, -1, nullptr, nullptr,
                RPC_C_AUTHN_LEVEL_DEFAULT, RPC_C_IMP_LEVEL_IMPERSONATE,
                nullptr, EOAC_NONE, nullptr);

            if (FAILED(result))
                throw SystemException("Failed to initialize the COM library.");
        }

        inline ~ComInitializer()
        {
            // CoInitializeEx() should always be proceeded with CoUninitialize(), even
            // if it returns S_FALSE.
            ::CoUninitialize();
        }
    };

    static ComPtr<IEnumWbemClassObject> ExecuteWmiQuery(const WideStringView wqlQuery)
    {
        HRESULT result;

        // Get a pointer to the ROOT\CIMV2 namespace.
        ComPtr<IWbemLocator> wbemLocator;
        result = ::CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER,
            IID_IWbemLocator, &wbemLocator);

        if (FAILED(result))
            throw SystemException("Failed to create an instance of IWbemLocator.");

        ComPtr<IWbemServices> wbemServices;
        result = wbemLocator->ConnectServer(
            bstr_t("ROOT\\CIMV2"), nullptr, nullptr,
            nullptr, 0, nullptr, nullptr, &wbemServices);

        if (FAILED(result))
            throw SystemException("Failed to create a connection to ROOT\\CIMV2.");

        result = ::CoSetProxyBlanket(
            wbemServices.Get(), RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE,
            nullptr, RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE,
            nullptr, EOAC_NONE);

        if (FAILED(result))
            throw SystemException("Failed to set authentication information.");

        // Execute our query.
        ComPtr<IEnumWbemClassObject> enumerator;
        result = wbemServices->ExecQuery(
            bstr_t("WQL"), bstr_t(wqlQuery.Data()),
            WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
            nullptr, &enumerator);

        if (FAILED(result))
            throw SystemException("Failed to execute the WQL query.");

        return enumerator;
    }

    template<Invocable<IWbemClassObject*, VARIANT&> Func>
    static void EnumerateWmiObject(
        const ComPtr<IEnumWbemClassObject>& enumerator,
        Func func)
    {
        ULONG objectCount = 0;
        ComPtr<IWbemClassObject> classObject;

        while (enumerator)
        {
            HRESULT result = enumerator->Next(WBEM_INFINITE, 1, &classObject,
                                              &objectCount);

            if (FAILED(result))
            {
                throw SystemException("Failed to obtain the next object with the "
                                      "IEnumWbemClassObject class.");
            }

            if (objectCount == 0)
                break;

            // Main body of the enumeration.
            VARIANT variant;
            ::VariantInit(&variant);

            func(classObject.Get(), variant);

            ::VariantClear(&variant);
        }
    }

    static CpuArchitecture ToEngineArchitecture(Uint16 architecture)
    {
        switch (architecture)
        {
        case PROCESSOR_ARCHITECTURE_INTEL: return CpuArchitecture::x86_32;
        case PROCESSOR_ARCHITECTURE_AMD64: return CpuArchitecture::x86_64;
        case PROCESSOR_ARCHITECTURE_ARM  : return CpuArchitecture::AArch32;
        case PROCESSOR_ARCHITECTURE_ARM64: return CpuArchitecture::AArch64;
        default:
            return CpuArchitecture::Unknown;
        }
    }

    static Uint64 TranslateToUint64(const BSTR string)
    {
        WideString wideString(string, ::SysStringLen(string));
        unsigned long long value = std::wcstoull(wideString.Raw(), nullptr, 10);

        KITSUNE_ASSERT(
            value != 0,
            "Failed to convert a BSTR into an unsigned long long.");

        return value;
    }

    CpuInformation SystemInformation::GetCpuInformation()
    {
        ComInitializer initializer_{};
        ComPtr<IEnumWbemClassObject> enumerator = ExecuteWmiQuery(
            L"SELECT Architecture, Manufacturer, Name, NumberOfCores, "
            L"NumberOfLogicalProcessors "
            L"FROM Win32_Processor");

        CpuInformation cpuInfo;
        bool alreadyRun = false;

        EnumerateWmiObject(enumerator, [&cpuInfo, &alreadyRun](
            IWbemClassObject* classObject, VARIANT& variant)
        {
            // Make sure the enumeration only runs once, because this function
            // doesn't support multi-CPU setups, like ones using Intel's Xeon CPUs
            // or AMD's Epyc CPUs.
            if (alreadyRun)
                return;

            if (SUCCEEDED(classObject->Get(L"Architecture", 0, &variant, nullptr, nullptr)))
                cpuInfo.Architecture = ToEngineArchitecture(variant.uiVal);

            if (SUCCEEDED(classObject->Get(L"Manufacturer", 0, &variant, nullptr, nullptr)))
            {
                WideString manufacturer(variant.bstrVal, ::SysStringLen(variant.bstrVal));
                cpuInfo.Vendor = Utf16ToUtf8<wchar_t, char>(manufacturer);

#if defined(KITSUNE_ARCH_X86)
                cpuInfo.Vendor = TranslateX86VendorString(cpuInfo.Vendor);
#endif
            }

            if (SUCCEEDED(classObject->Get(L"Name", 0, &variant, nullptr, nullptr)))
            {
                WideString wideName(variant.bstrVal, ::SysStringLen(variant.bstrVal));
                cpuInfo.Description = Utf16ToUtf8<wchar_t, char>(wideName);
            }

            if (SUCCEEDED(classObject->Get(L"NumberOfCores", 0, &variant, nullptr, nullptr)))
                cpuInfo.PhysicalCoreCount = variant.ulVal;

            if (SUCCEEDED(classObject->Get(L"NumberOfLogicalProcessors", 0, &variant,
                                           nullptr, nullptr)))
            {
                cpuInfo.LogicalCoreCount = variant.ulVal;
            }

            alreadyRun = true;
        });

        cpuInfo.Features = GetCpuFeatures();
        return cpuInfo;
    }

    OsInformation SystemInformation::GetOperatingSystemInformation()
    {
        ComInitializer initializer_{};
        ComPtr<IEnumWbemClassObject> enumerator = ExecuteWmiQuery(
            L"SELECT Caption, OSType FROM Win32_OperatingSystem");

        OsInformation osInfo;
        EnumerateWmiObject(enumerator, [&osInfo](IWbemClassObject* classObject,
                                                 VARIANT& variant)
        {
            if (SUCCEEDED(classObject->Get(L"Caption", 0, &variant, nullptr, nullptr)))
            {
                WideString wideName(variant.bstrVal, ::SysStringLen(variant.bstrVal));
                osInfo.Name = Utf16ToUtf8<wchar_t, char>(wideName);
            }

            if (SUCCEEDED(classObject->Get(L"OsType", 0, &variant, nullptr, nullptr)))
            {
                switch (variant.uiVal)
                {
                case 15: osInfo.ShortName = "Windows 3.x";      break;
                case 16: osInfo.ShortName = "Windows 95";       break;
                case 17: osInfo.ShortName = "Windows 98";       break;
                case 18: osInfo.ShortName = "Windows NT";       break;
                case 19: osInfo.ShortName = "Windows Embedded"; break;
                default:
                    osInfo.ShortName = "Windows";
                }
            }
        });

        return osInfo;
    }

    BatteryInformation SystemInformation::GetBatteryInformation()
    {
        ComInitializer initializer_{};
        ComPtr<IEnumWbemClassObject> enumerator = ExecuteWmiQuery(
            L"SELECT EstimatedChargeRemaining, BatteryStatus FROM Win32_Battery");

        BatteryInformation batteryInfo = {
            .OnBattery = false,
            .UsesBattery = false,

            .ChargePercentage = 100
        };

        // A system without a battery installed won't even loop once.
        EnumerateWmiObject(enumerator, [&batteryInfo](IWbemClassObject* classObject,
                                                      VARIANT& variant)
        {
            batteryInfo.UsesBattery = true;
            if (SUCCEEDED(classObject->Get(L"EstimatedChargeRemaining", 0, &variant,
                                           nullptr, nullptr)))
            {
                batteryInfo.ChargePercentage = variant.uiVal;
            }

            if (SUCCEEDED(classObject->Get(L"BatteryStatus", 0, &variant, nullptr, nullptr)))
                batteryInfo.OnBattery = (variant.uiVal != 2);
        });

        return batteryInfo;
    }

    MemoryStatusInformation SystemInformation::GetCurrentMemoryStatus()
    {
        ComInitializer initializer_{};
        ComPtr<IEnumWbemClassObject> enumerator = ExecuteWmiQuery(
            L"SELECT FreePhysicalMemory, FreeVirtualMemory, TotalVirtualMemorySize "
            L"FROM Win32_OperatingSystem");

        MemoryStatusInformation memoryStatusInfo = { 0 };
        EnumerateWmiObject(enumerator, [&memoryStatusInfo](IWbemClassObject* classObject,
                                                           VARIANT& variant)
        {
            // CIM passes in Uint64 values via STRINGS (bstrVal), not unsigned long long (ullVal).
            // Why..
            if (SUCCEEDED(classObject->Get(L"FreePhysicalMemory", 0, &variant,
                                           nullptr, nullptr)))
            {
                memoryStatusInfo.AvailablePhysicalMemory = TranslateToUint64(variant.bstrVal);
            }

            if (SUCCEEDED(classObject->Get(L"FreeVirtualMemory", 0, &variant,
                                           nullptr, nullptr)))
            {
                memoryStatusInfo.AvailableVirtualMemory = TranslateToUint64(variant.bstrVal);
            }

            if (SUCCEEDED(classObject->Get(L"TotalVirtualMemorySize", 0, &variant,
                                           nullptr, nullptr)))
            {
                memoryStatusInfo.TotalVirtualMemory = TranslateToUint64(variant.bstrVal);
            }
        });

        enumerator = ExecuteWmiQuery(L"SELECT Capacity FROM Win32_PhysicalMemory");
        EnumerateWmiObject(enumerator, [&memoryStatusInfo](IWbemClassObject* classObject,
                                                           VARIANT& variant)
        {
            if (SUCCEEDED(classObject->Get(L"Capacity", 0, &variant, nullptr, nullptr)))
                memoryStatusInfo.TotalPhysicalMemory += TranslateToUint64(variant.bstrVal) / 1000;
        });

        return memoryStatusInfo;
    }
}
