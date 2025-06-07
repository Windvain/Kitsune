#include "RenderingCore/D3D12/D3D12CommandQueue.h"
#include "RenderingCore/D3D12/D3D12GraphicsDevice.h"

namespace Kitsune
{
    namespace DirectX
    {
        CommandBufferType ConvertDirectXToEngine(D3D12_COMMAND_LIST_TYPE queueType)
        {
            switch (queueType)
            {
            case D3D12_COMMAND_LIST_TYPE_DIRECT:  return CommandBufferType::Graphics;
            case D3D12_COMMAND_LIST_TYPE_COPY:    return CommandBufferType::Copy;
            case D3D12_COMMAND_LIST_TYPE_COMPUTE: return CommandBufferType::Compute;
            default:
                KITSUNE_UNREACHABLE();
            }
        }
    }

    namespace Details
    {
        [[noreturn]]
        KITSUNE_FORCEINLINE void ThrowEmptyCommandLists()
        {
            throw RenderException("Tried to push a command without a command list. Did you forget to call BeginCommandList()?");
        }
    }

    D3D12CommandQueue::D3D12CommandQueue(const D3D12GraphicsDevice& device, D3D12_COMMAND_LIST_TYPE type)
        : m_Device(device.GetDirectXDevice()), m_Type(type)
    {
        /* Create the command queue */
        D3D12_COMMAND_QUEUE_DESC desc;
        desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        desc.NodeMask = 0;
        desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
        desc.Type = type;

        DirectX::ThrowIfFailed(
            m_Device->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_CommandQueue)),
            "Failed to create a DirectX 12 command queue.");

        /* Then, create the command allocator */
        DirectX::ThrowIfFailed(
            m_Device->CreateCommandAllocator(type, IID_PPV_ARGS(&m_CommandAllocator)),
            "Failed to create a DirectX 12 command allocator.");

        m_FenceEvent = ::CreateEventW(nullptr, false, false, nullptr);
        if (m_FenceEvent == NULL)
            throw RenderException("Failed to create a Windows fence event.");
    }

    D3D12CommandQueue::~D3D12CommandQueue()
    {
        if (m_FenceEvent)
            ::CloseHandle(m_FenceEvent);

        WaitFinished();
    }

    CommandBufferType D3D12CommandQueue::GetType() const
    {
        return DirectX::ConvertDirectXToEngine(m_Type);
    }

    void D3D12CommandQueue::BeginCommandList()
    {
        DirectX::ComPtr<ID3D12GraphicsCommandList4> commandList;
        DirectX::ThrowIfFailed(
            m_Device->CreateCommandList(0, m_Type, m_CommandAllocator.Get(), nullptr, IID_PPV_ARGS(&commandList)),
            "Failed to create a DirectX 12 command list.");

        m_CommandLists.PushBack(Move(commandList));
    }

    void D3D12CommandQueue::EndCommandList()
    {
        if (m_CommandLists.Size() == 0)
            throw RenderException("BeginCommandList() was not called prior to EndCommandList().");

        // Mark the resource's state as PRESENT.
        for (auto& renderTarget : m_CurrentRenderTargets)
        {
            TransitionBarrier(renderTarget->GetDirectXResource(),
                D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
        }

        m_CurrentRenderTargets.Clear();
        DirectX::ThrowIfFailed(m_CommandLists.Back()->Close(), "Failed to close the command list.");
    }

    void D3D12CommandQueue::ExecuteCommandLists()
    {
        Array<ID3D12CommandList*> commandListPointers;
        for (const auto& commandList : m_CommandLists)
            commandListPointers.PushBack(commandList.Get());

        m_CommandQueue->ExecuteCommandLists(
            static_cast<UINT>(commandListPointers.Size()),
            commandListPointers.Data());

        m_CommandLists.Clear();
    }

    void D3D12CommandQueue::WaitFinished()
    {
        DirectX::ComPtr<ID3D12Fence> fence;
        DirectX::ThrowIfFailed(
            m_Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)),
            "Failed to create a DirectX 12 fence.");

        DirectX::ThrowIfFailed(m_CommandQueue->Signal(fence.Get(), 1),
                               "Failed to signal the command queue.");

        if (fence->GetCompletedValue() == 0)
        {
            DirectX::ThrowIfFailed(
                fence->SetEventOnCompletion(1, m_FenceEvent),
                "Failed to set fence completion event.");

            ::WaitForSingleObject(m_FenceEvent, INFINITE);
        }
    }

    void D3D12CommandQueue::SetRenderTargets(const Array<SharedPtr<IRenderTarget>>& targets)
    {
        if (m_CommandLists.Size() == 0)
            Details::ThrowEmptyCommandLists();

        Array<D3D12_CPU_DESCRIPTOR_HANDLE> descHandles;
        for (const auto& renderTarget : targets)
        {
            SharedPtr<D3D12RenderTarget> directXTarget = DynamicPointerCast<D3D12RenderTarget>(renderTarget);
            m_CurrentRenderTargets.PushBack(directXTarget);

            descHandles.PushBack(directXTarget->GetDirectXCpuDescriptorHandle());
            TransitionBarrier(directXTarget->GetDirectXResource(),
                D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
        }

        m_CommandLists.Back()->OMSetRenderTargets(static_cast<UINT>(targets.Size()), descHandles.Data(),
                                                  false, nullptr);
    }

    void D3D12CommandQueue::ClearRenderTargets(float red, float green, float blue, float alpha)
    {
        if (m_CommandLists.Size() == 0)
            Details::ThrowEmptyCommandLists();

        FLOAT color[4] = { red, green, blue, alpha };
        for (auto& renderTarget : m_CurrentRenderTargets)
            m_CommandLists.Back()->ClearRenderTargetView(renderTarget->GetDirectXCpuDescriptorHandle(), color, 0, nullptr);
    }

    DirectX::ComPtr<ID3D12CommandQueue> D3D12CommandQueue::GetDirectXCommandQueue() const
    {
        return m_CommandQueue;
    }

    void D3D12CommandQueue::TransitionBarrier(const DirectX::ComPtr<ID3D12Resource>& resource,
        D3D12_RESOURCE_STATES prevState, D3D12_RESOURCE_STATES newState)
    {
        if (m_CommandLists.Size() == 0)
            Details::ThrowEmptyCommandLists();

        CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(resource.Get(), prevState, newState);
        m_CommandLists.Back()->ResourceBarrier(1, &barrier);
    }
}
