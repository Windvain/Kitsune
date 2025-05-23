#include "RenderingCore/D3D12/D3D12CommandQueue.h"

#include "RenderingCore/D3D12/D3D12Fence.h"
#include "RenderingCore/D3D12/D3D12CommandBuffer.h"

namespace Kitsune
{
    D3D12CommandQueue::D3D12CommandQueue(const DirectX::ComPtr<ID3D12Device>& device,
                                         D3D12_COMMAND_LIST_TYPE type)
        : m_Type(DirectX::ConvertDirectXToEngine(type))
    {
        D3D12_COMMAND_QUEUE_DESC desc;
        desc.Type = type;
        desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
        desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        desc.NodeMask = 0;

        DirectX::ThrowIfFailed(
            device->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_CommandQueue)),
            "Failed to create a DirectX 12 command queue.");
    }

    D3D12CommandQueue::~D3D12CommandQueue()
    {
    }

    void D3D12CommandQueue::Submit(const Array<SharedPtr<ICommandBuffer>>& buffers)
    {
        Array<ID3D12CommandList*> directXBuffers;
        for (SharedPtr<ICommandBuffer> buffer : buffers)
        {
            auto* directXBuffer = dynamic_cast<D3D12CommandBuffer*>(buffer.Get());
            directXBuffers.PushBack(directXBuffer->GetDirectXCommandList().Get());
        }

        m_CommandQueue->ExecuteCommandLists(static_cast<UINT>(directXBuffers.Size()), directXBuffers.Data());
    }

    void D3D12CommandQueue::Signal(const SharedPtr<IFence>& fence)
    {
        auto* directXFence = dynamic_cast<D3D12Fence*>(fence.Get());
        m_CommandQueue->Signal(directXFence->GetDirectXFence().Get(), fence->GetSignaledValue());
    }
}
