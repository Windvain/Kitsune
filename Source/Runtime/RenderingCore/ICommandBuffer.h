#pragma once

#include "Foundation/Memory/SharedPtr.h"

namespace Kitsune
{
    class ISwapChain;

    enum class CommandBufferType
    {
        Graphics,
        Transfer,
        Compute
    };

    class ICommandBuffer
    {
    public:
        virtual ~ICommandBuffer() { /* ... */ }

    public:
        virtual CommandBufferType GetType() const = 0;

    public:
        virtual void BeginRecording(const SharedPtr<ISwapChain>& swapChain) = 0;
        virtual void EndRecording() = 0;

    public:
        // TODO: Change arguments to Color4().
        virtual void ClearColor(float red, float green, float blue, float alpha) = 0;
    };
}
