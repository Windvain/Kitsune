#pragma once

#include "Foundation/Memory/SharedPtr.h"
#include "Foundation/Containers/Array.h"

#include "RenderingCore/ICommandQueue.h"
#include "RenderingCore/D3D12/D3D12RenderTarget.h"
#include "RenderingCore/D3D12/DirectX12Core.h"

namespace Kitsune
{
    namespace DirectX
    {
        KITSUNE_API_ CommandBufferType ConvertDirectXToEngine(D3D12_COMMAND_LIST_TYPE queueType);
    }

    class D3D12GraphicsDevice;

    class D3D12CommandQueue : public ICommandQueue
    {
    public:
        KITSUNE_API_ D3D12CommandQueue(const D3D12GraphicsDevice& device, D3D12_COMMAND_LIST_TYPE type);
        KITSUNE_API_ ~D3D12CommandQueue();

    public:
        KITSUNE_API_ CommandBufferType GetType() const override;

    public:
        KITSUNE_API_ void BeginCommandList() override;
        KITSUNE_API_ void EndCommandList() override;

        KITSUNE_API_ void ExecuteCommandLists() override;
        KITSUNE_API_ void WaitFinished() override;

    public:
        KITSUNE_API_ void SetRenderTargets(const Array<SharedPtr<IRenderTarget>>& targets) override;
        KITSUNE_API_ void ClearRenderTargets(float red, float green, float blue, float alpha) override;

    public:
        KITSUNE_API_ DirectX::ComPtr<ID3D12CommandQueue> GetDirectXCommandQueue() const;

    private:
        KITSUNE_API_ void TransitionBarrier(const DirectX::ComPtr<ID3D12Resource>& resource,
            D3D12_RESOURCE_STATES prevState, D3D12_RESOURCE_STATES newState);

    private:
        DirectX::ComPtr<ID3D12Device> m_Device;
        Array<SharedPtr<D3D12RenderTarget>> m_CurrentRenderTargets;

        DirectX::ComPtr<ID3D12CommandQueue> m_CommandQueue;
        DirectX::ComPtr<ID3D12CommandAllocator> m_CommandAllocator;

        Array<DirectX::ComPtr<ID3D12GraphicsCommandList4>> m_CommandLists;
        D3D12_COMMAND_LIST_TYPE m_Type;

        HANDLE m_FenceEvent;
    };
}
