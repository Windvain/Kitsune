#include "RenderingCore/D3D12/D3D12SwapChain.h"
#include "Foundation/Diagnostics/Assert.h"

namespace Kitsune
{
    D3D12SwapChain::D3D12SwapChain(const DirectX::ComPtr<IDXGIFactory5>& factory,
                                   const DirectX::ComPtr<ID3D12Device2>& device,
                                   const DirectX::ComPtr<ID3D12CommandQueue>& commandQueue,
                                   const SwapChainSpecs& specs)
        : m_Device(device), m_Window(dynamic_cast<WindowsWindow*>(specs.Window.Get())),
          m_VsyncEnabled(specs.Vsync), m_HasTearingSupport(HasTearingSupport(factory)),
          m_BufferCount(specs.BufferCount)
    {
        CreateDescriptorHeap(device, m_BufferCount);

        HWND windowHandle = m_Window->GetWindowsHandle();
        Vector2<Uint32> windowSize = m_Window->GetSize();

        DXGI_SWAP_CHAIN_DESC1 swapChainDesc = CreateSwapChainDesc1(factory, windowSize, m_BufferCount);
        DXGI_SWAP_CHAIN_FULLSCREEN_DESC fullscreenDesc = { .Windowed = !m_Window->IsFullscreen() };

        for (Uint32 i = 0; i < specs.BufferCount; ++i)
            m_Fences.PushBack(MakeShared<D3D12Fence>(device, 0));

        DirectX::ComPtr<IDXGISwapChain1> swapChain;
        DirectX::ThrowIfFailed(
            factory->CreateSwapChainForHwnd(commandQueue.Get(), windowHandle, &swapChainDesc,
                                            &fullscreenDesc, nullptr, &swapChain),
            "Failed to create a DirectX 12 swap chain.");

        DirectX::ThrowIfFailed(
            factory->MakeWindowAssociation(windowHandle, DXGI_MWA_NO_ALT_ENTER),
            "Failed to disable Alt + Enter fullscreen toggle.");

        DirectX::ThrowIfFailed(
            swapChain.As(&m_SwapChain),
            "Failed to convert an IDXGISwapChain1 to an IDXGISwapChain4.");

        m_CurrentBackBufferIndex = m_SwapChain->GetCurrentBackBufferIndex();
        UpdateRenderTargetViews();
    }

    D3D12SwapChain::~D3D12SwapChain()
    {
        WaitForPreviousFrame();
    }

    void D3D12SwapChain::Present()
    {
        UINT syncInterval = bool(m_VsyncEnabled);
        UINT presentFlags = (m_HasTearingSupport && !m_VsyncEnabled) ?
            DXGI_PRESENT_ALLOW_TEARING : 0;

        DirectX::ThrowIfFailed(m_SwapChain->Present(syncInterval, presentFlags),
                               "Failed to present the current frame.");
    }

    void D3D12SwapChain::Resize(const Vector2<Uint32>& size)
    {
        m_BackBuffers.Clear();

        DXGI_SWAP_CHAIN_DESC1 swapChainDesc;
        m_SwapChain->GetDesc1(&swapChainDesc);

        DirectX::ThrowIfFailed(
            m_SwapChain->ResizeBuffers(m_BufferCount, size.x, size.y, swapChainDesc.Format,
                                       swapChainDesc.Flags),
            "Failed to resize the DirectX 12 swap chain's buffers.");

        UpdateRenderTargetViews();
    }

    Uint32 D3D12SwapChain::GetCurrentBackBufferIndex() const
    {
        return static_cast<Uint32>(m_SwapChain->GetCurrentBackBufferIndex());
    }

    void D3D12SwapChain::WaitForPreviousFrame()
    {
        m_Fences[GetCurrentBackBufferIndex()]->Wait();
    }

    void D3D12SwapChain::CreateDescriptorHeap(const DirectX::ComPtr<ID3D12Device>& device, UINT bufferCount)
    {
        D3D12_DESCRIPTOR_HEAP_DESC heapDesc;
        heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        heapDesc.NodeMask = 0;
        heapDesc.NumDescriptors = bufferCount;
        heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;

        DirectX::ThrowIfFailed(
            device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_RtvDescriptorHeap)),
            "Failed to create a DirectX 12 RTV descriptor heap.");
    }

    DXGI_SWAP_CHAIN_DESC1 D3D12SwapChain::CreateSwapChainDesc1(const DirectX::ComPtr<IDXGIFactory5>& factory,
                                                               const Vector2<Uint32>& size, UINT bufferCount)
    {
        DXGI_SWAP_CHAIN_DESC1 swapChainDesc;
        swapChainDesc.Width = size.x;
        swapChainDesc.Height = size.y;
        swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

        swapChainDesc.Stereo = FALSE;
        swapChainDesc.SampleDesc = { 1, 0 };

        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.BufferCount = bufferCount;
        swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

        swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
        swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

        if (HasTearingSupport(factory))
            swapChainDesc.Flags |= DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

        return swapChainDesc;
    }

    bool D3D12SwapChain::HasTearingSupport(const DirectX::ComPtr<IDXGIFactory5>& factory)
    {
        BOOL allowTearing;
        KITSUNE_VERIFY(
            factory->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING,
                                         &allowTearing, sizeof(BOOL)) == S_OK,
            "Failed to check a DirectX 12 device's feature support. Could be an implementation bug.");

        return (allowTearing == TRUE);
    }

    Vector2<Uint32> D3D12SwapChain::GetBufferSize(HWND windowHandle)
    {
        RECT clientRect;
        ::GetClientRect(windowHandle, &clientRect);

        return { Uint32(clientRect.right - clientRect.left),
                 Uint32(clientRect.bottom - clientRect.top) };
    }

    void D3D12SwapChain::UpdateRenderTargetViews()
    {
        m_RtvDescriptorSize = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_RtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

        if (!m_BackBuffers.IsEmpty())
            m_BackBuffers.Clear();

        for (UINT i = 0; i < m_BufferCount; ++i)
        {
            DirectX::ComPtr<ID3D12Resource> backBuffer;
            DirectX::ThrowIfFailed(
                m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&backBuffer)),
                "Failed to retrieve the swap chain's buffer.");

            m_Device->CreateRenderTargetView(backBuffer.Get(), nullptr, rtvHandle);

            m_BackBuffers.PushBack(Move(backBuffer));
            rtvHandle.Offset(m_RtvDescriptorSize);
        }
    }
}
