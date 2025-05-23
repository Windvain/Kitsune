#pragma once

#include "Foundation/Memory/SharedPtr.h"
#include "Foundation/Containers/Array.h"

#include "RenderingCore/IFence.h"
#include "RenderingCore/ICommandBuffer.h"

namespace Kitsune
{
    class ICommandQueue
    {
    public:
        virtual ~ICommandQueue() { /* ... */ }

    public:
        virtual CommandBufferType GetType() const = 0;

    public:
        virtual void Submit(const Array<SharedPtr<ICommandBuffer>>& buffers) = 0;
        virtual void Signal(const SharedPtr<IFence>& fence) = 0;
    };
}
