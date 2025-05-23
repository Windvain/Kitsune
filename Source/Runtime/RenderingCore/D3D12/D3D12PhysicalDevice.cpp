#include "RenderingCore/D3D12/D3D12PhysicalDevice.h"

#include "RenderingCore/D3D12/D3D12Instance.h"

#include "Foundation/Logging/GlobalLog.h"
#include "Foundation/Windows/StringConversions.h"
#include "Foundation/Diagnostics/InvalidArgumentException.h"

namespace Kitsune
{
    D3D12PhysicalDevice::D3D12PhysicalDevice(DirectX::ComPtr<IDXGIAdapter4>&& adapter)
        : m_Adapter(Move(adapter))
    {
        DXGI_ADAPTER_DESC3 adapterDesc;
        m_Adapter->GetDesc3(&adapterDesc);

        m_Description = Details::WindowsConvertToUtf8(adapterDesc.Description);
        m_Type = (adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE) ? PhysicalDeviceType::Software :
                 (adapterDesc.DedicatedVideoMemory == 0)           ? PhysicalDeviceType::Integrated :
                                                                     PhysicalDeviceType::Discrete;

        m_VendorId = adapterDesc.VendorId;
        m_VendorName = VendorIdToString(m_VendorId);

        m_DedicatedSystemMemory = adapterDesc.DedicatedSystemMemory;
        m_DedicatedVideoMemory = adapterDesc.DedicatedVideoMemory;
        m_SharedSystemMemory = adapterDesc.SharedSystemMemory;
    }

    D3D12PhysicalDevice::~D3D12PhysicalDevice()
    {
    }

    StringView D3D12PhysicalDevice::VendorIdToString(Uint32 vendorId)
    {
        switch (vendorId)
        {
        case 0x10DE: return "Nvidia";
        case 0x1002: return "AMD";
        case 0x8086: return "Intel";
        case 0x1414: return "Microsoft";
        default:
            return "Unknown";
        }

        KITSUNE_UNREACHABLE();
    }

    void D3D12PhysicalDevice::D3D12DebugMessageCallback(D3D12_MESSAGE_CATEGORY /* category */, D3D12_MESSAGE_SEVERITY severity,
                                                        D3D12_MESSAGE_ID /* id */, LPCSTR desc, void* /* context */)
    {
        LogSeverity logSeverity = (severity == D3D12_MESSAGE_SEVERITY_CORRUPTION) ? LogSeverity::Fatal :
                                  (severity == D3D12_MESSAGE_SEVERITY_ERROR)      ? LogSeverity::Error :
                                  (severity == D3D12_MESSAGE_SEVERITY_WARNING)    ? LogSeverity::Warning :
                                                                                    LogSeverity::Info;

        GetGlobalLogger()->LogFormat(logSeverity, "A DirectX 12 error has occured: \"{0}\"", desc);
    }
}
