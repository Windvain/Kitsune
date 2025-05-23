#pragma once

#include "RenderingCore/IFence.h"
#include "RenderingCore/D3D12/DirectX12Core.h"

namespace Kitsune
{
    class D3D12Fence : public IFence
    {
    public:
        KITSUNE_API_ D3D12Fence(const DirectX::ComPtr<ID3D12Device2>& device,
                                Uint64 value);

        KITSUNE_API_ ~D3D12Fence();

    public:
        KITSUNE_API_ void Signal() override;
        KITSUNE_API_ void Wait() override;

    public:
        inline Uint64 GetSignaledValue() const override
        {
            return m_Value;
        }

    public:
        inline DirectX::ComPtr<ID3D12Fence> GetDirectXFence() const
        {
            return m_Fence;
        }

    private:
        DirectX::ComPtr<ID3D12Fence> m_Fence;
        HANDLE m_Event;

        UINT64 m_Value;
    };
}
