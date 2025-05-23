#include "Application/Application.h"

#include "Foundation/Memory/Memory.h"
#include "Foundation/Logging/GlobalLog.h"

#include "RenderingCore/IGraphicsInstance.h"

using namespace Kitsune;

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

class Sandbox : public Application
{
public:
    Sandbox(const ApplicationSpecs& specs)
        : Application(specs)
    {
        SharedPtr<IGraphicsInstance> instance = CreateGraphicsInstance(GraphicsBackend::DirectX12);
        Array<SharedPtr<IPhysicalDevice>> physDevices = instance->EnumeratePhysicalDevices(PhysicalDevicePreference::PowerSaving);

        Uint32 index = 0;
        for (SharedPtr<IPhysicalDevice>& physDevice : physDevices)
        {
            ++index;
            if (physDevice->GetType() == PhysicalDeviceType::Software)
                continue;

            KITSUNE_TRACE_FORMAT("Using GPU {0}: {1} [Type: {2}, Vendor: {3}, Dedicated sysMem: {4} MB, Dedicated vidMem: {5} MB, Shared sysMem: {6} MB, Total Mem: {7} MB]",
                index, physDevice->GetName(), physDevice->GetType(), physDevice->GetVendorName(),
                physDevice->GetDedicatedSystemMemory() / 1048576, physDevice->GetDedicatedVideoMemory() / 1048576,
                physDevice->GetSharedSystemMemory() / 1048576, physDevice->GetTotalAvailableGraphicsMemory() / 1048576);

            break;
        }

        m_Device = instance->CreateGraphicsDevice(physDevices[0]);
        m_CommandQueue = m_Device->GetCommandQueue(CommandBufferType::Graphics);

        SwapChainSpecs swapChainSpecs;
        swapChainSpecs.BufferCount = s_BufferCount;
        swapChainSpecs.Vsync = true;
        swapChainSpecs.Window = GetWindow();

        m_SwapChain = m_Device->CreateSwapChain(swapChainSpecs);

        for (Uint32 i = 0; i < s_BufferCount; ++i)
            m_GraphicsCommandBuffers.PushBack(m_Device->CreateCommandBuffer(CommandBufferType::Graphics));
    }

    void OnUpdate()
    {
        Uint32 frameIndex = m_SwapChain->GetCurrentBackBufferIndex();
        auto& currentCmdBuffer = m_GraphicsCommandBuffers[frameIndex];

        currentCmdBuffer->BeginRecording(m_SwapChain);

        // Clear the render target with the specified colour.
        currentCmdBuffer->ClearColor(0.5f, 0.0f, 1.0f, 1.0f);

        currentCmdBuffer->EndRecording();
        m_CommandQueue->Submit({ currentCmdBuffer });

        m_SwapChain->GetFence()->Signal();
        m_CommandQueue->Signal(m_SwapChain->GetFence());

        m_SwapChain->Present();
        m_SwapChain->WaitForPreviousFrame();
    }

    ~Sandbox()
    {
        m_SwapChain->GetFence()->Signal();
        m_CommandQueue->Signal(m_SwapChain->GetFence());
    }

private:
    static constexpr Uint32 s_BufferCount = 3;

private:
    SharedPtr<ILogicalDevice> m_Device;
    SharedPtr<ISwapChain> m_SwapChain;

    SharedPtr<ICommandQueue> m_CommandQueue;
    Array<SharedPtr<ICommandBuffer>> m_GraphicsCommandBuffers;
};

Application* Kitsune::CreateApplication(const CommandLineArguments& /* args */)
{
    ApplicationSpecs specs;
    specs.Name = "Sandbox";
    specs.ViewportSize = { 640, 480 };
    specs.WindowPositionHint = WindowPositionHint::ScreenCenter;

    return Memory::New<Sandbox>(specs);
}
