#include "RenderingCore/D3D12/D3D12GraphicsDevice.h"

#include <dxgidebug.h>
#include "Foundation/Logging/GlobalLog.h"

#include "RenderingCore/D3D12/D3D12SwapChain.h"
#include "RenderingCore/D3D12/D3D12PhysicalDevice.h"

namespace Kitsune
{
    namespace DirectX
    {
        // Using DXGI_DEBUG_ALL causes a linking error because we aren't linking to DXGIDebug.dll.
        const GUID DxgiDebugAll = { 0xe48ae283, 0xda80, 0x490b, { 0x87, 0xe6, 0x43, 0xe9, 0xa9, 0xcf, 0xda, 0x8 } };

        DXGI_GPU_PREFERENCE ConvertEngineToDirectX(GpuPreference pref)
        {
            return (pref == GpuPreference::PowerSaving)     ? DXGI_GPU_PREFERENCE_MINIMUM_POWER :
                   (pref == GpuPreference::HighPerformance) ? DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE :
                                                              DXGI_GPU_PREFERENCE_UNSPECIFIED;
        }

        LogSeverity ConvertDirectXToEngine(D3D12_MESSAGE_SEVERITY severity)
        {
            // Didn't expect DirectX's message severity and the engine's to match so perfectly..
            switch (severity)
            {
            case D3D12_MESSAGE_SEVERITY_CORRUPTION: return LogSeverity::Fatal;
            case D3D12_MESSAGE_SEVERITY_ERROR:      return LogSeverity::Error;
            case D3D12_MESSAGE_SEVERITY_WARNING:    return LogSeverity::Warning;
            case D3D12_MESSAGE_SEVERITY_INFO:       return LogSeverity::Info;
            case D3D12_MESSAGE_SEVERITY_MESSAGE:    return LogSeverity::Trace;
            default:
                KITSUNE_UNREACHABLE();
            }
        }
    }

    D3D12GraphicsDevice::D3D12GraphicsDevice(const GraphicsDeviceSpecs& specs)
    {
        /* Instantiate a factory & debug components as needed */
        UINT factoryFlags = 0;

#if defined(KITSUNE_BUILD_RELEASE)
        m_DebugEnabled = false;
#else
        {
            DirectX::ComPtr<ID3D12Debug1> debugInterface;
            DirectX::ComPtr<IDXGIDebug> dxgiDebugInterface;

            // The usage of DXGIGetDebugInterface1() doesn't need a link to DXGIDebug.dll.
            m_DebugEnabled = SUCCEEDED(::D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface))) &&
                            SUCCEEDED(::DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiDebugInterface)));

            if (m_DebugEnabled)
            {
                debugInterface->EnableDebugLayer();
                debugInterface->SetEnableGPUBasedValidation(true);
                debugInterface->SetEnableSynchronizedCommandQueueValidation(true);

                dxgiDebugInterface->ReportLiveObjects(DirectX::DxgiDebugAll, DXGI_DEBUG_RLO_ALL);
                factoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
            }
        }
#endif

        DirectX::ThrowIfFailed(::CreateDXGIFactory2(factoryFlags, IID_PPV_ARGS(&m_Factory)),
            "Failed to create a DXGI Factory.");

        /* Retrieve a handle to the physical device which best suits the GPU hint given */
        DirectX::ComPtr<IDXGIAdapter4> adapter;
        DXGI_GPU_PREFERENCE gpuPref = DirectX::ConvertEngineToDirectX(specs.GpuHint);

        for (UINT index = 0;
            m_Factory->EnumAdapterByGpuPreference(index, gpuPref, IID_PPV_ARGS(&adapter)) == S_OK;
            ++index)
        {
            if (SUCCEEDED(::D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0,
                                              __uuidof(ID3D12Device), nullptr)))
            {
                m_PhysicalDevice = MakeShared<D3D12PhysicalDevice>(Move(adapter));
                break;
            }
        }

        /* Create the device */
        DirectX::ThrowIfFailed(
            ::D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_Device)),
            "Failed to create a DirectX 12 device.");

        if (SUCCEEDED(m_Device.As(&m_InfoQueue)))
        {
            m_InfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
            m_InfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
            m_InfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);

            m_InfoQueue->RegisterMessageCallback(D3D12DebugMessageCallback, D3D12_MESSAGE_CALLBACK_FLAG_NONE,
                                                nullptr, &m_CallbackCookie);
        }

        /* Instantiate the command queues */
        m_GraphicsCommandQueue = MakeShared<D3D12CommandQueue>(*this, D3D12_COMMAND_LIST_TYPE_DIRECT);

        /* Instantiate descriptor heap */
        m_RtvDescriptorHeap = MakeShared<D3D12DescriptorHeap>(*this, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, s_RtvDescriptorHeapSize);
    }

    D3D12GraphicsDevice::~D3D12GraphicsDevice()
    {
        if (m_InfoQueue)
            m_InfoQueue->UnregisterMessageCallback(m_CallbackCookie);
    }

    SharedPtr<IPhysicalDevice> D3D12GraphicsDevice::GetPhysicalDevice() const
    {
        return m_PhysicalDevice;
    }

    SharedPtr<ISwapChain> D3D12GraphicsDevice::CreateSwapChain(const SwapChainSpecs& specs)
    {
        return MakeShared<D3D12SwapChain>(
            *this, DynamicPointerCast<WindowsWindow>(specs.Window), specs);
    }

    SharedPtr<IRenderTarget> D3D12GraphicsDevice::CreateRenderTarget(const RenderTargetSpecs& specs)
    {
        return MakeShared<D3D12RenderTarget>(*this, specs);
    }

    DirectX::ComPtr<IDXGIFactory6> D3D12GraphicsDevice::GetDxgiFactory() const
    {
        return m_Factory;
    }

    DirectX::ComPtr<ID3D12Device5> D3D12GraphicsDevice::GetDirectXDevice() const
    {
        return m_Device;
    }

    SharedPtr<ICommandQueue> D3D12GraphicsDevice::GetGraphicsCommandQueue() const
    {
        return m_GraphicsCommandQueue;
    }

    SharedPtr<D3D12DescriptorHeap> D3D12GraphicsDevice::GetDirectXRtvDescriptorHeap() const
    {
        return m_RtvDescriptorHeap;
    }

    bool D3D12GraphicsDevice::HasTearingSupport() const
    {
        BOOL allowTearing;
        DirectX::ThrowIfFailed(
            m_Factory->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING,
                                           &allowTearing, sizeof(BOOL)) == S_OK,
            "Failed to check a DirectX 12 device's feature support.");

        return (allowTearing == TRUE);
    }

    void D3D12GraphicsDevice::D3D12DebugMessageCallback(D3D12_MESSAGE_CATEGORY /* category */, D3D12_MESSAGE_SEVERITY severity,
                                                        D3D12_MESSAGE_ID /* id */, LPCSTR desc, void* /* context */)
    {
        LogSeverity logSeverity = DirectX::ConvertDirectXToEngine(severity);
        StringView severityString;

        switch (severity)
        {
        case D3D12_MESSAGE_SEVERITY_CORRUPTION: severityString = "Corruption"; break;
        case D3D12_MESSAGE_SEVERITY_ERROR:      severityString = "Error";      break;
        case D3D12_MESSAGE_SEVERITY_WARNING:    severityString = "Warning";    break;
        case D3D12_MESSAGE_SEVERITY_INFO:       severityString = "Info";       break;
        case D3D12_MESSAGE_SEVERITY_MESSAGE:    severityString = "Unknown";    break;
        default:
            KITSUNE_UNREACHABLE();
        }

        GetGlobalLogger()->LogFormat(
            logSeverity,
            "A DirectX 12 error has occured!\n\nSeverity: {0}\nMessage: {1}",
            severityString, desc);
    }
}
