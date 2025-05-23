#pragma once

#include "Foundation/Memory/SharedPtr.h"
#include "Foundation/Containers/Array.h"

#include "RenderingCore/ILogicalDevice.h"
#include "RenderingCore/D3D12/DirectX12Core.h"
#include "RenderingCore/D3D12/D3D12CommandQueue.h"

namespace Kitsune
{
    class D3D12Instance;

    class D3D12LogicalDevice : public ILogicalDevice
    {
    public:
        KITSUNE_API_ explicit D3D12LogicalDevice(const DirectX::ComPtr<IDXGIFactory6>& factory,
                                                 const DirectX::ComPtr<IDXGIAdapter>& adapter, bool useDebug);

        KITSUNE_API_ ~D3D12LogicalDevice();

    public:
        inline SharedPtr<ICommandQueue> GetCommandQueue(CommandBufferType type) override
        {
            return m_CommandQueues[int(type)];
        }

    public:
        KITSUNE_API_ SharedPtr<ISwapChain> CreateSwapChain(const SwapChainSpecs& specs) override;
        KITSUNE_API_ SharedPtr<ICommandBuffer> CreateCommandBuffer(CommandBufferType type) override;

        KITSUNE_API_ SharedPtr<IFence> CreateFence(Uint64 initialValue) override;

    public:
        inline DirectX::ComPtr<ID3D12Device> GetDirectXDevice() const { return m_Device; }

    private:
        KITSUNE_API_ static void D3D12DebugMessageCallback(
            D3D12_MESSAGE_CATEGORY category, D3D12_MESSAGE_SEVERITY severity,
            D3D12_MESSAGE_ID id, LPCSTR desc, void* context);

    private:
        // Graphics, Transfer, Compute.
        static constexpr Uint32 s_CommandQueueCount = 3;

    private:
        DirectX::ComPtr<IDXGIFactory6> m_Factory;

        DirectX::ComPtr<ID3D12Device5> m_Device;
        DirectX::ComPtr<ID3D12InfoQueue1> m_InfoQueue;

        SharedPtr<D3D12CommandQueue> m_CommandQueues[s_CommandQueueCount];
        DWORD m_CallbackCookie;
    };
}
