#include "RenderingCore/D3D12/D3D12CommandBuffer.h"
#include "RenderingCore/D3D12/D3D12SwapChain.h"

namespace Kitsune
{
    namespace DirectX
    {
        CommandBufferType ConvertDirectXToEngine(D3D12_COMMAND_LIST_TYPE queueType)
        {
            switch (queueType)
            {
            case D3D12_COMMAND_LIST_TYPE_DIRECT:  return CommandBufferType::Graphics;
            case D3D12_COMMAND_LIST_TYPE_COPY:    return CommandBufferType::Transfer;
            case D3D12_COMMAND_LIST_TYPE_COMPUTE: return CommandBufferType::Compute;
            default:
                KITSUNE_UNREACHABLE();
            }
        }

        D3D12_COMMAND_LIST_TYPE ConvertEngineToDirectX(CommandBufferType queueType)
        {
            switch (queueType)
            {
            case CommandBufferType::Graphics: return D3D12_COMMAND_LIST_TYPE_DIRECT;
            case CommandBufferType::Transfer: return D3D12_COMMAND_LIST_TYPE_COPY;
            case CommandBufferType::Compute:  return D3D12_COMMAND_LIST_TYPE_COMPUTE;
            default:
                KITSUNE_UNREACHABLE();
            }
        }
    }

    D3D12CommandBuffer::D3D12CommandBuffer(const DirectX::ComPtr<ID3D12Device>& device,
                                           D3D12_COMMAND_LIST_TYPE type)
        : m_Type(type)
    {
        DirectX::ThrowIfFailed(
            device->CreateCommandAllocator(type, IID_PPV_ARGS(&m_CommandAllocator)),
            "Failed to create a DirectX 12 command allocator.");

        DirectX::ThrowIfFailed(
            device->CreateCommandList(0, type, m_CommandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_CommandList)),
            "Failed to create a DirectX 12 command list.");

        m_CommandList->Close();
    }

    D3D12CommandBuffer::~D3D12CommandBuffer()
    {
    }

    void D3D12CommandBuffer::BeginRecording(const SharedPtr<ISwapChain>& swapChain)
    {
        DirectX::ThrowIfFailed(m_CommandAllocator->Reset(),
            "Failed to reset a DirectX 12 command allocator.");

        DirectX::ThrowIfFailed(
            m_CommandList->Reset(m_CommandAllocator.Get(), nullptr),
            "Failed to reset a DirectX 12 command list.");

        auto* directXSwapChain = dynamic_cast<D3D12SwapChain*>(swapChain.Get());
        m_CurrentBackBuffer = directXSwapChain->GetCurrentDirectXBackBuffer();

        CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
            m_CurrentBackBuffer.Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

        m_CommandList->ResourceBarrier(1, &barrier);

        auto* rtvDescHeap = directXSwapChain->GetDirectXRtvDescriptorHeap().Get();
        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(
            rtvDescHeap->GetCPUDescriptorHandleForHeapStart(),
            directXSwapChain->GetCurrentBackBufferIndex(),
            directXSwapChain->GetDirectXRtvDescriptorSize());

        m_CpuRtvHandle = rtvHandle;
        m_CommandList->OMSetRenderTargets(1, &rtvHandle, false, nullptr);
    }

    void D3D12CommandBuffer::EndRecording()
    {
        CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
            m_CurrentBackBuffer.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

        m_CommandList->ResourceBarrier(1, &barrier);
        DirectX::ThrowIfFailed(m_CommandList->Close(),
            "Failed to close a DirectX 12 command list.");
    }

    void D3D12CommandBuffer::ClearColor(float red, float green, float blue, float alpha)
    {
        float clearColor[] = { red, green, blue, alpha };
        m_CommandList->ClearRenderTargetView(m_CpuRtvHandle, clearColor, 0, nullptr);
    }
}
