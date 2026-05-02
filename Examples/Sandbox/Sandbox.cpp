#include "Application/Application.h"
#include "Foundation/Memory/Memory.h"

#include "GraphicsCore/GpuInstance.h"
#include "GraphicsCore/CommandList.h"

using namespace Kitsune;

class Sandbox : public Application
{
public:
    Sandbox(const ApplicationSpecifications& specs)
        : Application(specs)
    {
        DisplayManager* displayManager = DisplayManager::GetInstance();
        m_Window = displayManager->GetPrimaryWindow();

        m_GpuInstance = GpuInstance::Create({
            .DebugEnabled = false,
            .Backend = GraphicsApi::Vulkan,
            .Name = "Sandbox"
        });

        m_Surface = m_GpuInstance->RequestSurface(m_Window);
        m_Device = m_GpuInstance->RequestDevice({
            .SupportedSurface = m_Surface,
            .Features = GpuDeviceFeature::WireframeRendering,
            .CommandQueues = {
                {
                    .Type = CommandQueueType::Graphics,
                    .Count = 1,
                    .Flags = CommandQueueFlag::Presentable,
                    .Priorities = { 1.0f }
                }
            },
            .Preference = GpuDevicePreference::HighPerformance
        });

        m_GraphicsQueue = m_Device->GetCommandQueue(0, 0);
        m_SwapChain = m_Device->CreateSwapChain(
            m_Surface, m_GraphicsQueue,
            {
                .ImageCount = FrameCount,
                .Extent = m_Window->GetSize(),
                .Format = TextureFormat::R8G8B8A8Srgb,
                .PresentMode = PresentMode::Mailbox,
            });

        SharedPtr<ShaderModule> shaderModule = m_Device->CreateShaderModule("./Triangle.spv");
        m_RenderPipeline = m_Device->CreateRenderPipeline({
            .Topology = PrimitiveTopology::TriangleList,
            .FillMode = PolygonFillMode::Solid,
            .FrontFace = FrontFace::Clockwise,
            .CullMode = CullMode::Back,
            .VertexShader = shaderModule,
            .FragmentShader = shaderModule,
            .Format = TextureFormat::R8G8B8A8Srgb
        });

        m_CommandPool = m_Device->CreateCommandPool(m_GraphicsQueue);
        for (Uint32 index = 0; index < MaxInFlightFrames; ++index)
        {
            m_CommandLists.PushBack(m_CommandPool->AllocateCommandList(
                CommandListLevel::Primary));

            m_AcquireSemaphore.PushBack(m_Device->MakeSemaphore());
            m_FrameFences.PushBack(m_Device->CreateFence(FenceFlag::Signaled));
        }

        for (Uint32 index = 0; index < m_SwapChain->GetImageCount(); ++index)
        {
            SharedPtr<Texture> backBuffer = m_SwapChain->GetBackBuffer(index);
            m_BackBufferViews.PushBack(m_Device->CreateTextureView(backBuffer, {
                .Type = TextureViewType::Texture2D,
                .Format = m_SwapChain->GetSurfaceFormat(),
                .Mapping = {
                    .Red = TextureComponentSwizzle::Red,
                    .Green = TextureComponentSwizzle::Green,
                    .Blue = TextureComponentSwizzle::Blue,
                    .Alpha = TextureComponentSwizzle::Alpha
                }
            }));

            m_DrawFinishedSemaphore.PushBack(m_Device->MakeSemaphore());
        }
    }

    void OnUpdate() override
    {
        if (m_Window->GetSize() != m_WindowSize)
        {
            OnSwapChainInvalidation();
            m_WindowSize = m_Window->GetSize();
        }

        OnDraw();
    }

public:
    void OnSwapChainInvalidation()
    {
        m_GraphicsQueue->WaitIdle();
        m_SwapChain->Resize(m_Window->GetSize());

        m_BackBufferViews.Clear();
        for (Uint32 index = 0; index < m_SwapChain->GetImageCount(); ++index)
        {
            SharedPtr<Texture> backBuffer = m_SwapChain->GetBackBuffer(index);
            m_BackBufferViews.PushBack(m_Device->CreateTextureView(backBuffer, {
                .Type = TextureViewType::Texture2D,
                .Format = m_SwapChain->GetSurfaceFormat(),
                .Mapping = {
                    .Red = TextureComponentSwizzle::Red,
                    .Green = TextureComponentSwizzle::Green,
                    .Blue = TextureComponentSwizzle::Blue,
                    .Alpha = TextureComponentSwizzle::Alpha
                }
            }));
        }
    }

    void OnDraw()
    {
        m_FrameFences[m_FrameIndex]->Wait(UINT64_MAX);
        auto [index, result] = m_SwapChain->AcquireNextImage(
            m_AcquireSemaphore[m_FrameIndex]);

        if (!result)
        {
            OnSwapChainInvalidation();
            return;
        }

        m_FrameFences[m_FrameIndex]->Reset();
        RecordCommandList(index);

        CommandQueueSubmitInformation submitInformation = {
            .Waited = { m_AcquireSemaphore[m_FrameIndex] },
            .Signaled = { m_DrawFinishedSemaphore[index] }
        };

        m_GraphicsQueue->Submit(
            { m_CommandLists[m_FrameIndex] },
            submitInformation,
            m_FrameFences[m_FrameIndex]);

        m_SwapChain->Present(index, m_DrawFinishedSemaphore[index]);
        m_FrameIndex = (m_FrameIndex + 1) % MaxInFlightFrames;
    }

    void RecordCommandList(Uint32 index)
    {
        SharedPtr<CommandList>& commandList = m_CommandLists[m_FrameIndex];
        commandList->Reset();

        Vector2<Uint32> extent = m_SwapChain->GetExtent();

        SharedPtr<Texture> backBuffer = m_SwapChain->GetBackBuffer(index);
        SharedPtr<TextureView>& backBufferView = m_BackBufferViews[index];

        commandList->Begin();
        {
            RenderingInformation renderingInfo = {
                .RenderArea = Rect2<Uint32>({ 0, 0 }, extent),
                .ClearColor = { 0.2f, 0.0f, 1.0f, 1.0f }
            };

            commandList->BindRenderPipeline(m_RenderPipeline);
            commandList->SetViewport(
                Rect2<float>({ 0.0f, 0.0f }, extent),
                0.0f,
                1.0f);

            commandList->SetScissor(renderingInfo.RenderArea);
            commandList->TextureMemoryBarrier({{
                backBuffer,
                TextureLayout::Undefined,
                TextureLayout::RenderTarget
            }});

            commandList->BeginRendering(backBufferView, renderingInfo);
            {
                commandList->Draw(3, 1, 0, 0);
            }
            commandList->EndRendering();
            commandList->TextureMemoryBarrier({{
                backBuffer,
                TextureLayout::RenderTarget,
                TextureLayout::Presentation,
            }});
        }
        commandList->End();
    }

public:
    static constexpr Uint32 FrameCount = 3;
    static constexpr Uint32 MaxInFlightFrames = 2;

private:
    SharedPtr<GpuInstance> m_GpuInstance;
    SharedPtr<RenderSurface> m_Surface;
    SharedPtr<GpuDevice> m_Device;

    SharedPtr<CommandQueue> m_GraphicsQueue;
    SharedPtr<SwapChain> m_SwapChain;
    SharedPtr<RenderPipeline> m_RenderPipeline;

    SharedPtr<CommandPool> m_CommandPool;
    Array<SharedPtr<CommandList>> m_CommandLists;
    Array<SharedPtr<TextureView>> m_BackBufferViews;

    Array<SharedPtr<Semaphore>> m_AcquireSemaphore;
    Array<SharedPtr<Semaphore>> m_DrawFinishedSemaphore;
    Array<SharedPtr<Fence>> m_FrameFences;

    Uint32 m_FrameIndex = 0;

    Window* m_Window;
    Vector2<Uint32> m_WindowSize;
};

Application* Kitsune::CreateApplication(const CommandLineArguments& /* args */)
{
    ApplicationSpecifications specs;
    specs.Name = "Sandbox";
    specs.WindowPosition = { 300, 300 };
    specs.ViewportSize = { 1280, 960 };

    return Memory::New<Sandbox>(specs);
}
