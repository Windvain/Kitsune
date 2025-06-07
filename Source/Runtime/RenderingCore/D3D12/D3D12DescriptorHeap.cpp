#include "RenderingCore/D3D12/D3D12DescriptorHeap.h"
#include "RenderingCore/D3D12/D3D12GraphicsDevice.h"

namespace Kitsune
{
    D3D12DescriptorHeap::D3D12DescriptorHeap(const D3D12GraphicsDevice& device,
                                             D3D12_DESCRIPTOR_HEAP_TYPE type, UINT descCount)
        : m_Type(type), m_DescriptorCapacity(descCount)
    {
        D3D12_DESCRIPTOR_HEAP_DESC desc;
        desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        desc.NodeMask = 0;
        desc.NumDescriptors = descCount;
        desc.Type = type;

        auto nativeDevice = device.GetDirectXDevice();
        DirectX::ThrowIfFailed(
            nativeDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_DescriptorHeap)),
            "Failed to create a DirectX 12 descriptor heap.");

        m_DescriptorSize = nativeDevice->GetDescriptorHandleIncrementSize(type);
    }

    UINT D3D12DescriptorHeap::AllocateDescriptor()
    {
        return m_CurrentDescriptorIndex++;
    }

    D3D12_CPU_DESCRIPTOR_HANDLE D3D12DescriptorHeap::GetHeapStartCpuHandle() const
    {
        return m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart();
    }

    D3D12_CPU_DESCRIPTOR_HANDLE D3D12DescriptorHeap::GetHeapCpuHandle(UINT index) const
    {
        D3D12_CPU_DESCRIPTOR_HANDLE handle = GetHeapStartCpuHandle();
        handle.ptr += index * m_DescriptorSize;

        return handle;
    }
}
