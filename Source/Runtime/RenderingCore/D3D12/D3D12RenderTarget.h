#pragma once

#include "RenderingCore/IRenderTarget.h"

#include "RenderingCore/D3D12/D3D12Texture.h"
#include "RenderingCore/D3D12/DirectX12Core.h"

namespace Kitsune
{
    class D3D12GraphicsDevice;

    class D3D12RenderTarget : public IRenderTarget
    {
    public:
        KITSUNE_API_ D3D12RenderTarget(const D3D12GraphicsDevice& device, const RenderTargetSpecs& specs);

    public:
        KITSUNE_API_ SharedPtr<ITexture> GetTexture() const override;

    public:
        KITSUNE_API_ DirectX::ComPtr<ID3D12Resource> GetDirectXResource() const;
        D3D12_CPU_DESCRIPTOR_HANDLE GetDirectXCpuDescriptorHandle() const;

    private:
        SharedPtr<D3D12Texture> m_Texture;
        D3D12_CPU_DESCRIPTOR_HANDLE m_CpuDescriptorHandle;
    };
}
