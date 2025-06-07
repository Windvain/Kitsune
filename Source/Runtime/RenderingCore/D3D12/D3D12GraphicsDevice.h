#pragma once

#include "Foundation/Logging/LogMessage.h"

#include "RenderingCore/IGraphicsDevice.h"
#include "RenderingCore/D3D12/DirectX12Core.h"

#include "RenderingCore/D3D12/D3D12CommandQueue.h"
#include "RenderingCore/D3D12/D3D12DescriptorHeap.h"

namespace Kitsune
{
    namespace DirectX
    {
        KITSUNE_API_ DXGI_GPU_PREFERENCE ConvertEngineToDirectX(GpuPreference pref);
        KITSUNE_API_ LogSeverity ConvertDirectXToEngine(D3D12_MESSAGE_SEVERITY severity);
    }

    class D3D12GraphicsDevice : public IGraphicsDevice
    {
    public:
        KITSUNE_API_ explicit D3D12GraphicsDevice(const GraphicsDeviceSpecs& specs);
        KITSUNE_API_ ~D3D12GraphicsDevice();

    public:
        KITSUNE_API_ SharedPtr<IPhysicalDevice> GetPhysicalDevice() const override;
        KITSUNE_API_ SharedPtr<ICommandQueue> GetGraphicsCommandQueue() const override;

        KITSUNE_API_ SharedPtr<ISwapChain> CreateSwapChain(const SwapChainSpecs& specs) override;
        KITSUNE_API_ SharedPtr<IRenderTarget> CreateRenderTarget(const RenderTargetSpecs& specs) override;

    public:
        inline GraphicsBackend GetGraphicsBackend() const override
        {
            return GraphicsBackend::DirectX12;
        }

    public:
        KITSUNE_API_ DirectX::ComPtr<IDXGIFactory6> GetDxgiFactory() const;
        KITSUNE_API_ DirectX::ComPtr<ID3D12Device5> GetDirectXDevice() const;

        KITSUNE_API_ SharedPtr<D3D12DescriptorHeap> GetDirectXRtvDescriptorHeap() const;

    public:
        KITSUNE_API_ bool HasTearingSupport() const;

    private:
        KITSUNE_API_ static void D3D12DebugMessageCallback(
            D3D12_MESSAGE_CATEGORY category, D3D12_MESSAGE_SEVERITY severity,
            D3D12_MESSAGE_ID id, LPCSTR desc, void* context);

    private:
        static constexpr UINT s_RtvDescriptorHeapSize = 32;     // We shouldn't need more than 32, right..?

    private:
        bool m_DebugEnabled;
        DirectX::ComPtr<IDXGIFactory6> m_Factory;

        DirectX::ComPtr<ID3D12InfoQueue1> m_InfoQueue;
        DWORD m_CallbackCookie;

        SharedPtr<IPhysicalDevice> m_PhysicalDevice;
        DirectX::ComPtr<ID3D12Device5> m_Device;

        SharedPtr<D3D12CommandQueue> m_GraphicsCommandQueue;

        SharedPtr<D3D12DescriptorHeap> m_RtvDescriptorHeap;

        HANDLE m_FenceEvent;
    };
}
