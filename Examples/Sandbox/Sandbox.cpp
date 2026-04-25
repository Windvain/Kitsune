#include "Application/Application.h"
#include "Foundation/Memory/Memory.h"

#include "GraphicsCore/GpuInstance.h"

#include "Foundation/Algorithms/Contains.h"
#include "Foundation/Filesystem/FileStream.h"

using namespace Kitsune;

class Sandbox : public Application
{
public:
    Sandbox(const ApplicationSpecifications& specs)
        : Application(specs)
    {
        DisplayManager* displayManager = DisplayManager::GetInstance();
        Window* window = displayManager->GetPrimaryWindow();

        m_GpuInstance = GpuInstance::Create({
            .DebugEnabled = false,
            .Backend = GraphicsApi::Vulkan,
            .Name = "Sandbox"
        });

        m_Surface = m_GpuInstance->RequestSurface(window);
        m_Device = m_GpuInstance->RequestDevice({
            .SupportedSurface = m_Surface,
            .Extensions = GpuDeviceExtension::None,
            .CommandQueues = {
                {
                    .Type = CommandQueueType::Graphics,
                    .Count = 3,
                    .Flags = CommandQueueFlag::Presentable,
                    .Priorities = { 1.0f }
                }
            },
            .Preference = GpuDevicePreference::HighPerformance
        });

        RenderSurfaceCapabilities surfaceCapabilities =
            m_Surface->GetCapabilities(m_Device);

        auto supportedFormats = surfaceCapabilities.TextureFormats;
        auto supportedPresentModes = surfaceCapabilities.PresentModes;

        TextureFormat usedSurfaceFormat = SurfaceFormat;
        SurfacePresentMode usedPresentMode = PresentMode;

        if (!Algorithms::Contains(supportedFormats.GetBegin(), supportedFormats.GetEnd(),
                                  SurfaceFormat))
        {
            usedSurfaceFormat = supportedFormats[0];
        }

        if (!Algorithms::Contains(
                supportedPresentModes.GetBegin(), supportedPresentModes.GetEnd(),
                PresentMode))
        {
            usedPresentMode = SurfacePresentMode::Fifo;
        }

        m_Surface->ConfigureSwapChain(m_Device, {
            .ImageCount = Maths::Minimum(
                surfaceCapabilities.MinimumImageCount + 1,
                FrameCount),
            .Extents = window->GetSize(),
            .Format = usedSurfaceFormat,
            .PresentMode = usedPresentMode
        });

        m_GraphicsQueue = m_Device->GetQueue(0, 0);

        FileStream stream("Triangle.spv", FileAccessMode::Read);
        Usize shaderSize = stream.Seek(0, SeekOrigin::End);

        Array<Byte> shaderSource(shaderSize, Byte());

        stream.Seek(0, SeekOrigin::Begin);
        stream.Read(shaderSource.Data(), shaderSize);

        SharedPtr<ShaderModule> shaderModule = m_Device->CreateShaderModule(
            shaderSource.Data(), shaderSource.Size());

        m_RenderPipeline = m_Device->CreateRenderPipeline({
            .Topology = PrimitiveTopology::TriangleList,
            .FillMode = PolygonFillMode::Solid,
            .VertexShader = shaderModule,
            .FragmentShader = shaderModule,
            .RenderTargetFormat = usedSurfaceFormat
        });

        m_CommandPool = m_Device->CreateCommandPool(m_GraphicsQueue);
        for (Uint32 index = 0; index < MaxInFlightFrames; ++index)
        {
            m_CommandLists.PushBack(m_CommandPool->AllocateCommandList());
            m_AcquireSemaphore.PushBack(m_Device->MakeSemaphore());

            m_FrameFences.PushBack(m_Device->CreateFence());
        }

        SwapChainConfiguration swapChainConfig = m_Surface->GetSwapChainConfiguration();
        for (Uint32 index = 0; index < swapChainConfig.ImageCount; ++index)
        {
            SharedPtr<Texture> backBuffer = m_Surface->GetBackBuffer(index);
            m_BackBuffers.PushBack(m_Device->CreateTextureView(backBuffer, {
                .Dimension = TextureViewDimension::Texture2D,
                .Format = usedSurfaceFormat
            }));

            m_DrawFinishedSemaphore.PushBack(m_Device->MakeSemaphore());
        }
    }

    void OnUpdate()
    {
        m_FrameFences[m_FrameIndex]->Wait(UINT64_MAX);
        m_FrameFences[m_FrameIndex]->Reset();

        Uint32 index = m_Surface->AcquireNextImage(m_AcquireSemaphore[m_FrameIndex]);
        RecordCommandList(index);

        m_GraphicsQueue->Submit(
            { m_CommandLists[m_FrameIndex] },
            m_AcquireSemaphore[m_FrameIndex],
            m_DrawFinishedSemaphore[index],
            m_FrameFences[m_FrameIndex]);

        m_GraphicsQueue->Present(m_Surface, index, m_DrawFinishedSemaphore[index]);
        m_FrameIndex = (m_FrameIndex + 1) % MaxInFlightFrames;
    }

public:
    void RecordCommandList(Uint32 index)
    {
        SharedPtr<CommandList>& commandList = m_CommandLists[m_FrameIndex];
        commandList->Reset();

        SwapChainConfiguration swapChainConfig = m_Surface->GetSwapChainConfiguration();

        SharedPtr<Texture> backBuffer = m_Surface->GetBackBuffer(index);
        SharedPtr<TextureView>& backBufferView = m_BackBuffers[index];

        commandList->Begin();
        {
            RenderingSpecifications renderingSpecs = {
                .RenderArea = Rect2<Uint32>({ 0, 0 }, swapChainConfig.Extents)
            };

            commandList->BindRenderPipeline(m_RenderPipeline);
            commandList->SetViewport(
                Rect2<float>({ 0.0f, 0.0f }, swapChainConfig.Extents),
                0.0f,
                1.0f);

            commandList->SetScissor(renderingSpecs.RenderArea);
            commandList->TextureBarrier(backBuffer, {
                TextureUsage::Undefined,
                TextureUsage::RenderAttachment
            });

            commandList->BeginRendering(backBufferView, renderingSpecs);
            {
                commandList->Draw(3, 1, 0, 0);
            }
            commandList->EndRendering();
            commandList->TextureBarrier(backBuffer, {
                TextureUsage::RenderAttachment,
                TextureUsage::Presentation
            });
        }
        commandList->End();
    }

public:
    static constexpr Uint32 FrameCount = 3;
    static constexpr Uint32 MaxInFlightFrames = 2;

    static constexpr TextureFormat SurfaceFormat = TextureFormat::Rgba8Srgb;
    static constexpr SurfacePresentMode PresentMode = SurfacePresentMode::Mailbox;

private:
    SharedPtr<GpuInstance> m_GpuInstance;
    SharedPtr<RenderSurface> m_Surface;
    SharedPtr<GpuDevice> m_Device;

    SharedPtr<CommandQueue> m_GraphicsQueue;
    SharedPtr<RenderPipeline> m_RenderPipeline;

    SharedPtr<CommandPool> m_CommandPool;
    Array<SharedPtr<CommandList>> m_CommandLists;

    Array<SharedPtr<TextureView>> m_BackBuffers;
    Array<SharedPtr<Semaphore>> m_AcquireSemaphore;
    Array<SharedPtr<Semaphore>> m_DrawFinishedSemaphore;
    Array<SharedPtr<Fence>> m_FrameFences;

    Uint32 m_FrameIndex = 0;
};

Application* Kitsune::CreateApplication(const CommandLineArguments& /* args */)
{
    ApplicationSpecifications specs;
    specs.Name = "Sandbox";
    specs.WindowPosition = { 300, 300 };
    specs.ViewportSize = { 1280, 960 };

    return Memory::New<Sandbox>(specs);
}
