#pragma once

#include "RenderingCore/ISwapChain.h"
#include "RenderingCore/D3D12/D3D12RenderTarget.h"
#include "RenderingCore/D3D12/D3D12DescriptorHeap.h"

#include "Foundation/Memory/SharedPtr.h"
#include "Foundation/Containers/Array.h"

#include "ApplicationCore/Windows/WindowsWindow.h"

namespace Kitsune
{
    class D3D12GraphicsDevice;

    class D3D12SwapChain : public ISwapChain
    {
    public:
        KITSUNE_API_ D3D12SwapChain(D3D12GraphicsDevice& device,
                                    const SharedPtr<WindowsWindow>& window,
                                    const SwapChainSpecs& specs);
    public:
        KITSUNE_API_ void Present() override;
        KITSUNE_API_ SharedPtr<IRenderTarget> GetCurrentBackBuffer() const override;

    private:
        KITSUNE_API_ DXGI_SWAP_CHAIN_FULLSCREEN_DESC CreateDxgiSwapChainFullscreenDesc(
            const SharedPtr<WindowsWindow>& window);

        KITSUNE_API_ DXGI_SWAP_CHAIN_DESC1 CreateDxgiSwapChainDesc(
            const SwapChainSpecs& specs);

        KITSUNE_API_ void UpdateRenderTargetViews();

    private:
        DirectX::ComPtr<IDXGISwapChain4> m_SwapChain;
        Array<SharedPtr<D3D12RenderTarget>> m_FrameBuffers;

        D3D12GraphicsDevice& m_GraphicsDevice;

        UINT m_BufferCount;
        bool m_VsyncEnabled;
        bool m_HasTearingSupport;
    };
}
