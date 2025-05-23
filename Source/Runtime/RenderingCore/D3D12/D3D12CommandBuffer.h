#pragma once

#include "Foundation/Containers/Array.h"

#include "RenderingCore/ICommandBuffer.h"
#include "RenderingCore/D3D12/DirectX12Core.h"

namespace Kitsune
{
    namespace DirectX
    {
        KITSUNE_API_ CommandBufferType ConvertDirectXToEngine(D3D12_COMMAND_LIST_TYPE queueType);
        KITSUNE_API_ D3D12_COMMAND_LIST_TYPE ConvertEngineToDirectX(CommandBufferType queueType);
    }

    class D3D12CommandBuffer : public ICommandBuffer
    {
    public:
        KITSUNE_API_ D3D12CommandBuffer(const DirectX::ComPtr<ID3D12Device>& device,
                                        D3D12_COMMAND_LIST_TYPE type);

        KITSUNE_API_ ~D3D12CommandBuffer();

    public:
        inline CommandBufferType GetType() const override
        {
            return DirectX::ConvertDirectXToEngine(m_Type);
        }

    public:
        KITSUNE_API_ void BeginRecording(const SharedPtr<ISwapChain>& swapChain) override;
        KITSUNE_API_ void EndRecording() override;

    public:
        KITSUNE_API_ void ClearColor(float red, float green, float blue, float alpha) override;

    public:
        KITSUNE_API_ auto GetDirectXCommandList() const { return m_CommandList; }

    private:
        DirectX::ComPtr<ID3D12GraphicsCommandList4> m_CommandList;
        DirectX::ComPtr<ID3D12CommandAllocator> m_CommandAllocator;

        DirectX::ComPtr<ID3D12Resource> m_CurrentBackBuffer;
        CD3DX12_CPU_DESCRIPTOR_HANDLE m_CpuRtvHandle;

        D3D12_COMMAND_LIST_TYPE m_Type;
    };
}
