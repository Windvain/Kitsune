#pragma once

#include "RenderingCore/ITexture.h"
#include "RenderingCore/D3D12/DirectX12Core.h"

namespace Kitsune
{
    class D3D12Texture : public ITexture
    {
    public:
        KITSUNE_API_ D3D12Texture(const DirectX::ComPtr<ID3D12Resource>& resource);

    public:
        KITSUNE_API_ DirectX::ComPtr<ID3D12Resource> GetDirectXResource() const;

    private:
        DirectX::ComPtr<ID3D12Resource> m_Resource;
    };
}
