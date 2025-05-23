#pragma once

#include "RenderingCore/ILogicalDevice.h"
#include "RenderingCore/IPhysicalDevice.h"
#include "RenderingCore/IGraphicsInstance.h"

#include "RenderingCore/D3D12/DirectX12Core.h"

namespace Kitsune
{
    namespace DirectX
    {
        KITSUNE_API_ DXGI_GPU_PREFERENCE ConvertEngineToDirectX(PhysicalDevicePreference pref);
    }

    class D3D12Instance : public IGraphicsInstance
    {
    public:
        KITSUNE_API_ explicit D3D12Instance(bool requestDebug);
        KITSUNE_API_ ~D3D12Instance();

    public:
        inline GraphicsBackend GetGraphicsBackend() const override
        {
            return GraphicsBackend::DirectX12;
        }

    public:
        inline Array<SharedPtr<IPhysicalDevice>> EnumeratePhysicalDevices() override
        {
            return EnumeratePhysicalDevices(DXGI_GPU_PREFERENCE_UNSPECIFIED);
        }

        inline Array<SharedPtr<IPhysicalDevice>> EnumeratePhysicalDevices(PhysicalDevicePreference gpuPref)
        {
            return EnumeratePhysicalDevices(DirectX::ConvertEngineToDirectX(gpuPref));
        }

        KITSUNE_API_ SharedPtr<ILogicalDevice> CreateGraphicsDevice(
            const SharedPtr<IPhysicalDevice>& physicalDevice) override;

    public:
        KITSUNE_API_ Array<SharedPtr<IPhysicalDevice>> EnumeratePhysicalDevices(DXGI_GPU_PREFERENCE pref);

    public:
        inline DirectX::ComPtr<IDXGIFactory6> GetDirectXFactory() const { return m_Factory; }

    public:
        inline bool CanUseDebug() const { return m_CanUseDebug; }

    private:
        // Chose IDXGIFactory6 because it has EnumAdapterByGpuPreference().
        DirectX::ComPtr<IDXGIFactory6> m_Factory;

        bool m_CanUseDebug;
    };
}
