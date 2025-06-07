#include "RenderingCore/D3D12/D3D12RenderTarget.h"
#include "RenderingCore/D3D12/D3D12GraphicsDevice.h"

namespace Kitsune
{
    D3D12RenderTarget::D3D12RenderTarget(const D3D12GraphicsDevice& device, const RenderTargetSpecs& specs)
        : m_Texture(DynamicPointerCast<D3D12Texture>(specs.Texture))
    {
        auto rtvHeap = device.GetDirectXRtvDescriptorHeap();
        auto nativeDevice = device.GetDirectXDevice();

        UINT descIndex = rtvHeap->AllocateDescriptor();
        m_CpuDescriptorHandle = rtvHeap->GetHeapCpuHandle(descIndex);

        nativeDevice->CreateRenderTargetView(m_Texture->GetDirectXResource().Get(), nullptr,
                                             m_CpuDescriptorHandle);
    }

    SharedPtr<ITexture> D3D12RenderTarget::GetTexture() const
    {
        return m_Texture;
    }

    DirectX::ComPtr<ID3D12Resource> D3D12RenderTarget::GetDirectXResource() const
    {
        return m_Texture->GetDirectXResource();
    }

    D3D12_CPU_DESCRIPTOR_HANDLE D3D12RenderTarget::GetDirectXCpuDescriptorHandle() const
    {
        return m_CpuDescriptorHandle;
    }
}
