#pragma once

#include "RenderingCore/ICommandQueue.h"
#include "RenderingCore/D3D12/DirectX12Core.h"

namespace Kitsune
{
    class D3D12CommandQueue : public ICommandQueue
    {
    public:
        KITSUNE_API_ D3D12CommandQueue(const DirectX::ComPtr<ID3D12Device>& device,
                                       D3D12_COMMAND_LIST_TYPE type);

        KITSUNE_API_ ~D3D12CommandQueue();

    public:
        inline CommandBufferType GetType() const { return m_Type; }

    public:
        KITSUNE_API_ void Submit(const Array<SharedPtr<ICommandBuffer>>& buffers) override;
        KITSUNE_API_ void Signal(const SharedPtr<IFence>& fence) override;

    public:
        inline DirectX::ComPtr<ID3D12CommandQueue> GetDirectXCommandQueue() const
        {
            return m_CommandQueue;
        }

    private:
        DirectX::ComPtr<ID3D12CommandQueue> m_CommandQueue;
        CommandBufferType m_Type;
    };
}
