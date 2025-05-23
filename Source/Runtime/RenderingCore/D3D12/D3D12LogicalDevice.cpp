#include "RenderingCore/D3D12/D3D12LogicalDevice.h"

#include "Foundation/Logging/GlobalLog.h"
#include "ApplicationCore/Windows/WindowsWindow.h"

#include "RenderingCore/D3D12/D3D12Fence.h"
#include "RenderingCore/D3D12/D3D12Instance.h"
#include "RenderingCore/D3D12/D3D12SwapChain.h"
#include "RenderingCore/D3D12/D3D12CommandBuffer.h"

namespace Kitsune
{
    D3D12LogicalDevice::D3D12LogicalDevice(const DirectX::ComPtr<IDXGIFactory6>& factory,
                                           const DirectX::ComPtr<IDXGIAdapter>& adapter, bool useDebug)
        : m_Factory(factory)
    {
        DirectX::ThrowIfFailed(
            ::D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_Device)),
            "Failed to create a DirectX 12 device.");

        if (useDebug)
        {
            if (SUCCEEDED(m_Device.As(&m_InfoQueue)))
            {
                m_InfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
                m_InfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
                m_InfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);

                m_InfoQueue->RegisterMessageCallback(D3D12DebugMessageCallback, D3D12_MESSAGE_CALLBACK_FLAG_NONE,
                                                     nullptr, &m_CallbackCookie);
            }
        }

        m_CommandQueues[int(CommandBufferType::Graphics)] = MakeShared<D3D12CommandQueue>(m_Device, D3D12_COMMAND_LIST_TYPE_DIRECT);
        m_CommandQueues[int(CommandBufferType::Transfer)] = MakeShared<D3D12CommandQueue>(m_Device, D3D12_COMMAND_LIST_TYPE_COPY);
        m_CommandQueues[int(CommandBufferType::Compute)] = MakeShared<D3D12CommandQueue>(m_Device, D3D12_COMMAND_LIST_TYPE_COMPUTE);
    }

    D3D12LogicalDevice::~D3D12LogicalDevice()
    {
        m_InfoQueue->UnregisterMessageCallback(m_CallbackCookie);
    }

    SharedPtr<ISwapChain> D3D12LogicalDevice::CreateSwapChain(const SwapChainSpecs& specs)
    {
        auto commandQueue = dynamic_cast<D3D12CommandQueue*>(GetCommandQueue(CommandBufferType::Graphics).Get());
        return MakeShared<D3D12SwapChain>(m_Factory, m_Device, commandQueue->GetDirectXCommandQueue(), specs);
    }

    SharedPtr<ICommandBuffer> D3D12LogicalDevice::CreateCommandBuffer(CommandBufferType type)
    {
        return MakeShared<D3D12CommandBuffer>(m_Device, DirectX::ConvertEngineToDirectX(type));
    }

    SharedPtr<IFence> D3D12LogicalDevice::CreateFence(Uint64 initialValue)
    {
        return MakeShared<D3D12Fence>(m_Device, initialValue);
    }

    void D3D12LogicalDevice::D3D12DebugMessageCallback(D3D12_MESSAGE_CATEGORY /* category */, D3D12_MESSAGE_SEVERITY severity,
        D3D12_MESSAGE_ID /* id */, LPCSTR desc, void* /* context */)
    {
        LogSeverity logSeverity = (severity == D3D12_MESSAGE_SEVERITY_CORRUPTION) ? LogSeverity::Fatal :
                                  (severity == D3D12_MESSAGE_SEVERITY_ERROR)      ? LogSeverity::Error :
                                  (severity == D3D12_MESSAGE_SEVERITY_WARNING)    ? LogSeverity::Warning :
                                                                                    LogSeverity::Info;

        GetGlobalLogger()->LogFormat(logSeverity, "D3D12 Error: {0}", desc);
    }
}
