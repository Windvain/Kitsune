#pragma once

#include "GraphicsCore/Fence.h"
#include "GraphicsCore/SwapChain.h"
#include "GraphicsCore/RenderPipeline.h"

#include "Foundation/Memory/SharedPtr.h"
#include "Foundation/Containers/Array.h"

#include "Foundation/String/StringView.h"
#include "Foundation/Utilities/EnumFlags.h"
#include "Foundation/Utilities/NonCopyable.h"

namespace Kitsune
{
    class CommandPool;

    class ShaderModule;
    class RenderSurface;

    // Specifies the preferences for picking a GPU. This enum is intended
    // to be used with the DX12 backend, but other rendering backends can
    // choose to honor these preferences.
    enum class GpuDevicePreference
    {
        Unspecified,
        BatterySaver,
        HighPerformance
    };

    // Specifies the features which a GPU can support.
    enum class GpuDeviceFeature
    {
        None = 0,
        WireframeRendering = 1 << 0     // Enables the use of PolygonFillMode::Wireframe.
    };

    KITSUNE_OVERLOAD_FLAGS_OPERATORS(GpuDeviceFeature);

    // Provides access to a low-level graphics API.
    class GpuDevice : public NonCopyable
    {
    public:
        virtual ~GpuDevice() = default;

    public:
        [[nodiscard]]
        virtual SharedPtr<CommandQueue> GetCommandQueue(
            Uint32 index,
            Uint32 queueIndex) const = 0;

        [[nodiscard]]
        virtual GpuDeviceFeature GetFeatures() const = 0;

    public:
        [[nodiscard]]
        virtual SharedPtr<CommandPool> CreateCommandPool(
            const SharedPtr<CommandQueue>& commandQueue) = 0;

        [[nodiscard]]
        virtual SharedPtr<Fence> CreateFence(FenceFlag flags) = 0;

        [[nodiscard]]
        virtual SharedPtr<RenderPipeline> CreateRenderPipeline(
            const RenderPipelineSpecifications& specifications) = 0;

        [[nodiscard]]
        virtual SharedPtr<Semaphore> MakeSemaphore() = 0;

        [[nodiscard]]
        virtual SharedPtr<SwapChain> CreateSwapChain(
            const SharedPtr<RenderSurface>& surface,
            const SharedPtr<CommandQueue>& presentQueue,
            const SwapChainConfiguration& configuration) = 0;

        [[nodiscard]]
        virtual SharedPtr<TextureView> CreateTextureView(
            const SharedPtr<Texture>& texture,
            const TextureViewSpecifications& specifications) = 0;

    public:
        [[nodiscard]]
        virtual SharedPtr<ShaderModule> CreateShaderModule(Array<Byte>&& shaderCode) = 0;

        [[nodiscard]]
        SharedPtr<ShaderModule> CreateShaderModule(
            const Byte* shaderCode, Usize shaderCodeSize);

        [[nodiscard]]
        SharedPtr<ShaderModule> CreateShaderModule(StringView shaderPath);
    };
}
