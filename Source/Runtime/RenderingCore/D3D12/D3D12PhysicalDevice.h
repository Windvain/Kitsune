#pragma once

#include "RenderingCore/IPhysicalDevice.h"
#include "RenderingCore/D3D12/DirectX12Core.h"

namespace Kitsune
{
    class D3D12Instance;

    class D3D12PhysicalDevice : public IPhysicalDevice
    {
    public:
        KITSUNE_API_ explicit D3D12PhysicalDevice(DirectX::ComPtr<IDXGIAdapter4>&& adapter);
        KITSUNE_API_ ~D3D12PhysicalDevice();

    public:
        inline String GetName() const override { return m_Description; }
        inline PhysicalDeviceType GetType() const override { return m_Type; }

        inline Uint32 GetVendorId() const override { return m_VendorId; }
        inline String GetVendorName() const override { return m_VendorName; }

    public:
        inline Usize GetDedicatedVideoMemory() const override
        {
            return m_DedicatedVideoMemory;
        }

        inline Usize GetSharedSystemMemory() const override
        {
            return m_SharedSystemMemory;
        }

        inline Usize GetDedicatedSystemMemory() const override
        {
            return m_DedicatedSystemMemory;
        }

    public:
        inline DirectX::ComPtr<IDXGIAdapter4> GetDirectXAdapter() const { return m_Adapter; }

    private:
        static StringView VendorIdToString(Uint32 vendorId);
        static void D3D12DebugMessageCallback(D3D12_MESSAGE_CATEGORY, D3D12_MESSAGE_SEVERITY,
                                              D3D12_MESSAGE_ID, LPCSTR, void*);

    private:
        DirectX::ComPtr<IDXGIAdapter4> m_Adapter;
        PhysicalDeviceType m_Type;

        Usize m_DedicatedVideoMemory;
        Usize m_SharedSystemMemory;
        Usize m_DedicatedSystemMemory;

        String m_Description;
        Uint32 m_VendorId;
        String m_VendorName;
    };
}
