#include "Foundation/Utilities/SystemInformation.h"

#include <wrl.h>
#include <comdef.h>
#include <Wbemidl.h>

#include "Foundation/String/TranscodePresets.h"
#include "Foundation/Diagnostics/SystemException.h"

template<typename T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

namespace Kitsune
{
    struct ComInitializer
    {
        inline ComInitializer()
        {
            HRESULT result = ::CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
            if (FAILED(result))
                throw SystemException("Failed to initialize the COM library.");

            result = ::CoInitializeSecurity(
                nullptr, -1, nullptr, nullptr,
                RPC_C_AUTHN_LEVEL_DEFAULT, RPC_C_IMP_LEVEL_IMPERSONATE,
                nullptr, EOAC_NONE, nullptr);

            if (FAILED(result))
                throw SystemException("Failed to initialize the COM library.");
        }

        inline ~ComInitializer()
        {
            // CoInitializeEx() should always be proceeded with CoUninitialize(),
            // even if it returns S_FALSE.
            ::CoUninitialize();
        }
    };

    static ComPtr<IEnumWbemClassObject> ExecuteWmiQuery(WideStringView wqlQuery)
    {
        HRESULT result;

        // Get a pointer to the ROOT\CIMV2 namespace.
        ComPtr<IWbemLocator> wbemLocator;
        result = ::CoCreateInstance(
            CLSID_WbemLocator, nullptr, CLSCTX_INPROC_SERVER, IID_IWbemLocator,
            &wbemLocator);

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

    template<Invocable<IWbemClassObject*> Func>
    static void EnumerateWmiObject(const ComPtr<IEnumWbemClassObject>& enumerator,
                                   Func func)
    {
        ULONG objectCount = 0;
        ComPtr<IWbemClassObject> classObject;

        while (enumerator)
        {
            HRESULT result = enumerator->Next(
                WBEM_INFINITE, 1, &classObject, &objectCount);

            if (FAILED(result))
            {
                throw SystemException(
                    "Failed to obtain the next object with the "
                    "IEnumWbemClassObject class.");
            }

            if (objectCount == 0)
                break;

            func(classObject.Get());
        }
    }

    inline static HRESULT GetClassObjectValue(
        IWbemClassObject* classObject, WideStringView name,
        VARIANT* variant)
    {
        return classObject->Get(name.Data(), 0, variant, nullptr, nullptr);
    }

    inline static String BstrToUtf8(BSTR string)
    {
        WideString wideString(string, ::SysStringLen(string));
        return Utf16ToUtf8<wchar_t, char>(wideString);
    }

    Array<CpuInformation> SystemInformation::GetCpuInformation()
    {
        ComInitializer initializer_{};
        ComPtr<IEnumWbemClassObject> enumerator = ExecuteWmiQuery(
            L"SELECT Architecture, Manufacturer, Name, NumberOfCores, "
            L"NumberOfLogicalProcessors "
            L"FROM Win32_Processor");

        VARIANT variant;
        ::VariantInit(&variant);

        Array<CpuInformation> cpuInfoArray;
        EnumerateWmiObject(enumerator,
            [&](IWbemClassObject* classObject)
            {
                CpuInformation cpuInformation;
                if (SUCCEEDED(GetClassObjectValue(classObject, L"Architecture",
                                                  &variant)))
                {
                    switch (variant.uiVal)
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
                }

                if (SUCCEEDED(GetClassObjectValue(classObject, L"Manufacturer",
                                                  &variant)))
                {
                    cpuInformation.m_Vendor = BstrToUtf8(variant.bstrVal);
                }

                if (SUCCEEDED(GetClassObjectValue(classObject, L"Name", &variant)))
                    cpuInformation.m_Description = BstrToUtf8(variant.bstrVal);

                if (SUCCEEDED(GetClassObjectValue(classObject, L"NumberOfCores",
                                                  &variant)))
                {
                    cpuInformation.m_PhysicalCores = variant.ulVal;
                }

                if (SUCCEEDED(GetClassObjectValue(
                    classObject, L"NumberOfLogicalProcessors", &variant)))
                {
                    cpuInformation.m_LogicalCores = variant.ulVal;
                }

                cpuInfoArray.PushBack(Move(cpuInformation));
            });

        ::VariantClear(&variant);
        return cpuInfoArray;
    }

    OperatingSystemInformation SystemInformation::GetOperatingSystemInformation()
    {
        ComInitializer initializer_{};
        ComPtr<IEnumWbemClassObject> enumerator = ExecuteWmiQuery(
            L"SELECT Caption, OSType FROM Win32_OperatingSystem");

        VARIANT variant;
        ::VariantInit(&variant);

        enum OsTypeConstants : USHORT
        {
            Win3x = 15,
            Win95 = 16,
            Win98 = 17,
            WinNt = 18,
            WinCe = 19
        };

        OperatingSystemInformation osInformation;
        EnumerateWmiObject(enumerator, [&](IWbemClassObject* classObject)
        {
            if (SUCCEEDED(GetClassObjectValue(classObject, L"Caption", &variant)))
                osInformation.m_Name = BstrToUtf8(variant.bstrVal);

            if (SUCCEEDED(GetClassObjectValue(classObject, L"OsType", &variant)))
            {
                switch (variant.uiVal)
                {
                case OsTypeConstants::Win3x:
                    osInformation.m_ShortName = "Windows 3.x";
                    break;
                case OsTypeConstants::Win95:
                    osInformation.m_ShortName = "Windows 95";
                    break;
                case OsTypeConstants::Win98:
                    osInformation.m_ShortName = "Windows 98";
                    break;
                case OsTypeConstants::WinNt:
                    osInformation.m_ShortName = "Windows NT";
                    break;
                case OsTypeConstants::WinCe:
                    osInformation.m_ShortName = "Windows Embedded";
                    break;
                default:
                    osInformation.m_ShortName = "Windows";
                }
            }
        });

        ::VariantClear(&variant);
        return osInformation;
    }

    BatteryInformation SystemInformation::GetBatteryInformation()
    {
        ComInitializer initializer_{};
        ComPtr<IEnumWbemClassObject> enumerator = ExecuteWmiQuery(
            L"SELECT EstimatedChargeRemaining, BatteryStatus FROM Win32_Battery");

        VARIANT variant;
        ::VariantInit(&variant);

        enum BatteryStatusConstants : USHORT
        {
            Other = 1,          // Discharging.
            Unknown = 2,        // Has access to AC power.

            /* Charge status (Full, High, Low, Critical) */
        };

        BatteryInformation batteryInformation;
        EnumerateWmiObject(enumerator, [&](IWbemClassObject* classObject)
        {
            batteryInformation.m_UsesBattery = true;
            if (SUCCEEDED(GetClassObjectValue(
                classObject, L"EstimatedChargeRemaining", &variant)))
            {
                batteryInformation.m_ChargePercentage = variant.uiVal;
            }

            if (SUCCEEDED(classObject->Get(
                L"BatteryStatus", 0, &variant, nullptr, nullptr)))
            {
                batteryInformation.m_OnBattery =
                    (variant.uiVal != BatteryStatusConstants::Unknown);
            }
        });

        ::VariantClear(&variant);
        return batteryInformation;
    }
}
