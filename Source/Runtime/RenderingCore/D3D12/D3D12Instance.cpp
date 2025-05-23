#include "RenderingCore/D3D12/D3D12Instance.h"

#include <dxgidebug.h>
#include "RenderingCore/RenderException.h"

#include "RenderingCore/D3D12/D3D12PhysicalDevice.h"
#include "RenderingCore/D3D12/D3D12LogicalDevice.h"

namespace Kitsune
{
    namespace DirectX
    {
        // Using DXGI_DEBUG_ALL causes a linking error because we aren't linking to DXGIDebug.dll.
        const GUID DxgiDebugAll = { 0xe48ae283, 0xda80, 0x490b, 0x87, 0xe6, 0x43, 0xe9, 0xa9, 0xcf, 0xda, 0x8 };

        DXGI_GPU_PREFERENCE ConvertEngineToDirectX(PhysicalDevicePreference pref)
        {
            return (pref == PhysicalDevicePreference::PowerSaving)     ? DXGI_GPU_PREFERENCE_MINIMUM_POWER :
                   (pref == PhysicalDevicePreference::HighPerformance) ? DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE :
                                                                         DXGI_GPU_PREFERENCE_UNSPECIFIED;
        }
    }

    [[noreturn]]
    KITSUNE_FORCEINLINE void ThrowCouldNotFindAdapter()
    {
        throw RenderException("Could not find a Direct3D 12 compatible adapter.");
    }

    D3D12Instance::D3D12Instance(bool requestDebug)
    {
        UINT factoryFlags = 0;

        if (!requestDebug)
            m_CanUseDebug = false;
        else
        {
            DirectX::ComPtr<ID3D12Debug1> debugInterface;
            DirectX::ComPtr<IDXGIDebug> dxgiDebugInterface;

            m_CanUseDebug = SUCCEEDED(::D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface))) &&
                            SUCCEEDED(::DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiDebugInterface)));

            if (CanUseDebug())
            {
                debugInterface->EnableDebugLayer();
                debugInterface->SetEnableGPUBasedValidation(true);
                debugInterface->SetEnableSynchronizedCommandQueueValidation(true);

                dxgiDebugInterface->ReportLiveObjects(DirectX::DxgiDebugAll, DXGI_DEBUG_RLO_ALL);
                factoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
            }
        }

        DirectX::ThrowIfFailed(::CreateDXGIFactory2(factoryFlags, IID_PPV_ARGS(&m_Factory)),
            "Failed to create a DXGI Factory.");
    }

    D3D12Instance::~D3D12Instance()
    {
    }

    SharedPtr<ILogicalDevice> D3D12Instance::CreateGraphicsDevice(
        const SharedPtr<IPhysicalDevice>& physicalDevice)
    {
        auto* directXPhysicalDevice = dynamic_cast<D3D12PhysicalDevice*>(physicalDevice.Get());
        auto adapter = directXPhysicalDevice->GetDirectXAdapter();

        return MakeShared<D3D12LogicalDevice>(m_Factory, adapter, CanUseDebug());
    }

    Array<SharedPtr<IPhysicalDevice>> D3D12Instance::EnumeratePhysicalDevices(DXGI_GPU_PREFERENCE gpuPref)
    {
        DirectX::ComPtr<IDXGIAdapter4> adapter;
        Array<SharedPtr<IPhysicalDevice>> physDevices;

        for (UINT index = 0;
             m_Factory->EnumAdapterByGpuPreference(index, gpuPref, IID_PPV_ARGS(&adapter)) == S_OK;
             ++index)
        {
            if (SUCCEEDED(::D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0,
                                              __uuidof(ID3D12Device), nullptr)))
            {
                physDevices.PushBack(MakeShared<D3D12PhysicalDevice>(Move(adapter)));
            }
        }

        if (physDevices.Size() == 0)
            ThrowCouldNotFindAdapter();

        return physDevices;
    }
}
