#pragma once

#include "RenderingCore/D3D12/DirectX12Core.h"

namespace Kitsune
{
    class D3D12GraphicsDevice;

    class D3D12DescriptorHeap
    {
    public:
        KITSUNE_API_ D3D12DescriptorHeap(const D3D12GraphicsDevice& device,
                                         D3D12_DESCRIPTOR_HEAP_TYPE type, UINT descCount);

    public:
        inline UINT GetDescriptorSize() const
        {
            return m_DescriptorSize;
        }

        inline UINT GetDescriptorCapacity() const
        {
            return m_DescriptorCapacity;
        }

    public:
        KITSUNE_API_ UINT AllocateDescriptor();

    public:
        KITSUNE_API_ D3D12_CPU_DESCRIPTOR_HANDLE GetHeapStartCpuHandle() const;
        KITSUNE_API_ D3D12_CPU_DESCRIPTOR_HANDLE GetHeapCpuHandle(UINT index) const;

    private:
        DirectX::ComPtr<ID3D12DescriptorHeap> m_DescriptorHeap;
        UINT m_CurrentDescriptorIndex = 0;

        D3D12_DESCRIPTOR_HEAP_TYPE m_Type;
        UINT m_DescriptorCapacity;
        UINT m_DescriptorSize;
    };
}
