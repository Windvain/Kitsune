#include "RenderingCore/D3D12/D3D12Texture.h"

namespace Kitsune
{
    D3D12Texture::D3D12Texture(const DirectX::ComPtr<ID3D12Resource>& resource)
        : m_Resource(resource)
    {
    }

    DirectX::ComPtr<ID3D12Resource> D3D12Texture::GetDirectXResource() const
    {
        return m_Resource;
    }
}
