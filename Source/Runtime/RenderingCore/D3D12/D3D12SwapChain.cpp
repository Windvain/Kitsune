#include "RenderingCore/D3D12/D3D12SwapChain.h"

#include "Foundation/Diagnostics/Assert.h"
#include "RenderingCore/D3D12/D3D12Texture.h"
#include "RenderingCore/D3D12/D3D12GraphicsDevice.h"

namespace Kitsune
{
    D3D12SwapChain::D3D12SwapChain(D3D12GraphicsDevice& device,
                                   const SharedPtr<WindowsWindow>& window,
                                   const SwapChainSpecs& specs)
        : m_GraphicsDevice(device), m_BufferCount(specs.BufferCount),
          m_VsyncEnabled(specs.VsyncEnabled), m_HasTearingSupport(device.HasTearingSupport())
    {
        DirectX::ComPtr<ID3D12Device5> nativeDevice = device.GetDirectXDevice();
        DirectX::ComPtr<IDXGIFactory6> nativeFactory = device.GetDxgiFactory();

        DirectX::ComPtr<IDXGISwapChain1> tempSwapChain;
        HWND windowHandle = window->GetWindowsHandle();

        DXGI_SWAP_CHAIN_DESC1 desc = CreateDxgiSwapChainDesc(specs);
        DXGI_SWAP_CHAIN_FULLSCREEN_DESC fullscreenDesc = CreateDxgiSwapChainFullscreenDesc(window);

        KITSUNE_ASSERT((desc.Flags & DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH) != 0,
                "The DirectX 12 swap chain descriptor should have "
                "DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH enabled.");

        auto graphicsQueue = DynamicPointerCast<D3D12CommandQueue>(device.GetGraphicsCommandQueue());
        auto nativeGraphicsQueue = graphicsQueue->GetDirectXCommandQueue();

        DirectX::ThrowIfFailed(
            nativeFactory->CreateSwapChainForHwnd(
                nativeGraphicsQueue.Get(), windowHandle, &desc,
                &fullscreenDesc, nullptr, &tempSwapChain),
            "Failed to create a DirectX 12 swap chain.");

        DirectX::ThrowIfFailed(
            nativeFactory->MakeWindowAssociation(windowHandle, DXGI_MWA_NO_ALT_ENTER),
            "Failed to disable DirectX Alt + Enter fullscreen toggle.");

        DirectX::ThrowIfFailed(
            tempSwapChain.As(&m_SwapChain),
            "Failed to convert an IDXGISwapChain1 to an IDXGISwapChain4.");

        UpdateRenderTargetViews();
    }

    void D3D12SwapChain::Present()
    {
        UINT syncInterval = bool(m_VsyncEnabled);
        UINT presentFlags = (m_HasTearingSupport && !m_VsyncEnabled) ?
            DXGI_PRESENT_ALLOW_TEARING : 0;

        DirectX::ThrowIfFailed(m_SwapChain->Present(syncInterval, presentFlags),
                               "Failed to present the current frame.");
    }

    SharedPtr<IRenderTarget> D3D12SwapChain::GetCurrentBackBuffer() const
    {
        return m_FrameBuffers[m_SwapChain->GetCurrentBackBufferIndex()];
    }

    void D3D12SwapChain::UpdateRenderTargetViews()
    {
        if (!m_FrameBuffers.IsEmpty())
            m_FrameBuffers.Clear();

        for (UINT i = 0; i < m_BufferCount; ++i)
        {
            DirectX::ComPtr<ID3D12Resource> resource;
            DirectX::ThrowIfFailed(
                m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&resource)),
                "Failed to retrieve the swap chain's buffer.");

            RenderTargetSpecs specs;
            specs.Texture = MakeShared<D3D12Texture>(Move(resource));

            m_FrameBuffers.PushBack(
                MakeShared<D3D12RenderTarget>(m_GraphicsDevice, specs));
        }
    }

    DXGI_SWAP_CHAIN_FULLSCREEN_DESC D3D12SwapChain::CreateDxgiSwapChainFullscreenDesc(
            const SharedPtr<WindowsWindow>& window)
    {
        DXGI_SWAP_CHAIN_FULLSCREEN_DESC fullscreenDesc;
        fullscreenDesc.RefreshRate = { 0, 0 };
        fullscreenDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
        fullscreenDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
        fullscreenDesc.Windowed = !window->IsFullscreen();

        return fullscreenDesc;
    }

    DXGI_SWAP_CHAIN_DESC1 D3D12SwapChain::CreateDxgiSwapChainDesc(
        const SwapChainSpecs& specs)
    {
        DXGI_SWAP_CHAIN_DESC1 swapChainDesc;
        Vector2<Uint32> windowSize = specs.Window->GetSize();

        swapChainDesc.Width = windowSize.x;
        swapChainDesc.Height = windowSize.y;
        swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

        swapChainDesc.Stereo = FALSE;
        swapChainDesc.SampleDesc = { 1, 0 };

        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.BufferCount = specs.BufferCount;
        swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

        swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
        swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

        if (m_HasTearingSupport)
            swapChainDesc.Flags |= DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

        return swapChainDesc;
    }
}
