#pragma once

#include "Foundation/Memory/SharedPtr.h"
#include "Foundation/Containers/Array.h"

#include "Foundation/Utilities/NonCopyable.h"

namespace Kitsune
{
    class Fence;
    class Semaphore;

    class CommandList;
    class RenderSurface;

    // The type of command queue.
    enum class CommandQueueType
    {
        Graphics,       //< Supports graphics, compute, and copy operations.
        Compute,        //< Supports compute and copy operations.
        Transfer        //< Only supports copy operations.
    };

    // A queue of commands to be executed on the GPU.
    class CommandQueue : public NonCopyable
    {
    public:
        virtual ~CommandQueue() = default;

        virtual void Submit(
            const Array<SharedPtr<CommandList>>& commandLists,
            const SharedPtr<Semaphore>& waitSemaphore,
            SharedPtr<Semaphore>& signaledSemaphore,
            SharedPtr<Fence>& signaledFence) = 0;

        virtual void Present(
            const SharedPtr<RenderSurface>& surface,
            Uint32 backBufferIndex,
            const SharedPtr<Semaphore>& waitSemaphore) = 0;
    };
}
