#pragma once

#include "Foundation/Containers/Array.h"

#include "RenderingCore/ISwapChain.h"
#include "ApplicationCore/Windows/WindowsWindow.h"

#include "RenderingCore/D3D12/D3D12Fence.h"
#include "RenderingCore/D3D12/DirectX12Core.h"

namespace Kitsune
{
    class D3D12SwapChain : public ISwapChain
    {
    public:
        KITSUNE_API_ D3D12SwapChain(const DirectX::ComPtr<IDXGIFactory5>& factory,
                                    const DirectX::ComPtr<ID3D12Device2>& device,
                                    const DirectX::ComPtr<ID3D12CommandQueue>& commandQueue,
                                    const SwapChainSpecs& specs);

        KITSUNE_API_ ~D3D12SwapChain();

    public:
        KITSUNE_API_ void Present() override;
        KITSUNE_API_ void Resize(const Vector2<Uint32>& size) override;

        KITSUNE_API_ Uint32 GetCurrentBackBufferIndex() const override;

        inline void SetVsyncEnabled(bool vsync) override { m_VsyncEnabled = vsync; }
        inline bool IsVsyncEnabled() const override { return m_VsyncEnabled; }

        KITSUNE_API_ void WaitForPreviousFrame() override;

        inline SharedPtr<IFence> GetFence() override
        {
            return m_Fences[GetCurrentBackBufferIndex()];
        }

    public:
        inline DirectX::ComPtr<IDXGISwapChain4> GetDirectXSwapChain() const
        {
            return m_SwapChain;
        }

        inline DirectX::ComPtr<ID3D12DescriptorHeap> GetDirectXRtvDescriptorHeap() const
        {
            return m_RtvDescriptorHeap;
        }

        inline UINT GetDirectXRtvDescriptorSize() const
        {
            return m_RtvDescriptorSize;
        }

        inline DirectX::ComPtr<ID3D12Resource> GetCurrentDirectXBackBuffer() const
        {
            return m_BackBuffers[GetCurrentBackBufferIndex()];
        }

    private:
        KITSUNE_API_ void CreateDescriptorHeap(const DirectX::ComPtr<ID3D12Device>& device, UINT bufferCount);
        KITSUNE_API_ DXGI_SWAP_CHAIN_DESC1 CreateSwapChainDesc1(const DirectX::ComPtr<IDXGIFactory5>& factory,
                                                                const Vector2<Uint32>& size, UINT bufferCount);

        KITSUNE_API_ static bool HasTearingSupport(const DirectX::ComPtr<IDXGIFactory5>& factory);

        KITSUNE_API_ static Vector2<Uint32> GetBufferSize(HWND windowHandle);
        KITSUNE_API_ void UpdateRenderTargetViews();

    private:
        DirectX::ComPtr<ID3D12Device2> m_Device;
        DirectX::ComPtr<IDXGISwapChain4> m_SwapChain;

        DirectX::ComPtr<ID3D12DescriptorHeap> m_RtvDescriptorHeap;
        UINT m_RtvDescriptorSize;

        Array<DirectX::ComPtr<ID3D12Resource>> m_BackBuffers;
        WindowsWindow* m_Window;

        Array<SharedPtr<D3D12Fence>> m_Fences;

        bool m_VsyncEnabled;
        bool m_HasTearingSupport;

        UINT m_BufferCount;
        UINT m_CurrentBackBufferIndex;
    };
}
