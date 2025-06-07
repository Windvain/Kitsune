#include "Application/Application.h"

#include "Foundation/Memory/Memory.h"
#include "Foundation/Logging/GlobalLog.h"

#include "RenderingCore/IGraphicsDevice.h"

#define CONV_TO_MB 1048576

namespace Kitsune
{
    template<>
    class Formatter<PhysicalDeviceType>
    {
    public:
        void Parse(const ParseContext& context)
        {
        }

        template<WritableIterator<char> Iter>
        Iter Format(const FormatContext<PhysicalDeviceType, Iter>& context)
        {
            StringView str = (context.GetValue() == PhysicalDeviceType::Discrete)   ? "Discrete" :
                             (context.GetValue() == PhysicalDeviceType::Integrated) ? "Integrated" :
                                                                                      "Software";

            return Algorithms::Copy(str.GetBegin(), str.GetEnd(), context.GetOutput());
        }
    };
}

using namespace Kitsune;

class Sandbox : public Application
{
public:
    Sandbox(const ApplicationSpecs& specs)
        : Application(specs)
    {
        /* Create Graphics Device */
        GraphicsDeviceSpecs deviceSpecs;
        deviceSpecs.Backend = GraphicsBackend::DirectX12;
        deviceSpecs.GpuHint = GpuPreference::PowerSaving;
        deviceSpecs.UseDebug = true;

        m_Device = CreateGraphicsDevice(deviceSpecs);

        /* Log GPU Information */
        SharedPtr<IPhysicalDevice> gpuHandle = m_Device->GetPhysicalDevice();
        KITSUNE_TRACE_FORMAT("Using GPU: {0} [Type: {1}, Vendor: {2}, Dedicated sysMem: {3} MB, Dedicated vidMem: {4} MB, Shared sysMem: {5} MB, Total Mem: {6} MB]",
            gpuHandle->GetName(), gpuHandle->GetType(), gpuHandle->GetVendorName(),
            gpuHandle->GetDedicatedSystemMemory() / CONV_TO_MB,
            gpuHandle->GetDedicatedVideoMemory() / CONV_TO_MB,
            gpuHandle->GetSharedSystemMemory() / CONV_TO_MB,
            gpuHandle->GetTotalAvailableGraphicsMemory() / CONV_TO_MB);

        /* Retrieve the command queue */
        m_CommandQueue = m_Device->GetGraphicsCommandQueue();

        /* Create Swap Chain */
        SwapChainSpecs swapChainSpecs;
        swapChainSpecs.BufferCount = s_BufferCount;
        swapChainSpecs.VsyncEnabled = true;
        swapChainSpecs.Window = GetWindow();

        m_SwapChain = m_Device->CreateSwapChain(swapChainSpecs);
    }

    void OnUpdate()
    {
        m_CommandQueue->BeginCommandList();
        {
            m_CommandQueue->SetRenderTargets({ m_SwapChain->GetCurrentBackBuffer() });
            m_CommandQueue->ClearRenderTargets(0.5f, 0.0f, 1.0f, 1.0f);
        }
        m_CommandQueue->EndCommandList();
        m_CommandQueue->ExecuteCommandLists();

        m_SwapChain->Present();
        m_CommandQueue->WaitFinished();
    }

    ~Sandbox()
    {
    }

private:
    static constexpr Uint32 s_BufferCount = 3;

private:
    SharedPtr<IGraphicsDevice> m_Device;
    SharedPtr<ISwapChain> m_SwapChain;

    SharedPtr<ICommandQueue> m_CommandQueue;
};

Application* Kitsune::CreateApplication(const CommandLineArguments& /* args */)
{
    ApplicationSpecs specs;
    specs.Name = "Sandbox";
    specs.ViewportSize = { 640, 480 };
    specs.WindowPositionHint = WindowPositionHint::ScreenCenter;

    return Memory::New<Sandbox>(specs);
}
