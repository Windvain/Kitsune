#pragma once

#include "Foundation/Memory/SharedPtr.h"
#include "Foundation/Containers/Array.h"

#include "Foundation/Utilities/EnumFlags.h"
#include "Foundation/Utilities/NonCopyable.h"

namespace Kitsune
{
    class Fence;
    class Semaphore;
    class CommandList;

    // The type of command queue.
    enum class CommandQueueType
    {
        Graphics,       //< Supports graphics, compute, and copy operations.
        Compute,        //< Supports compute and copy operations.
        Transfer        //< Only supports copy operations.
    };

    // Specifies flags for the command queue.
    enum class CommandQueueFlag
    {
        None = 0,
        Presentable = 1 << 0    //< The command queue can be used for presentation.
    };

    KITSUNE_OVERLOAD_FLAGS_OPERATORS(CommandQueueFlag);

    // Specifies the command queue(s) to be requested from the device.
    struct CommandQueueSpecifications
    {
        CommandQueueType Type;
        Uint32 Count;

        CommandQueueFlag Flags;
        Array<float> Priorities = { 1.0f };
    };

    // Additional information for submitting command lists to a queue.
    struct CommandQueueSubmitInformation
    {
        Array<SharedPtr<Semaphore>> Waited;
        Array<SharedPtr<Semaphore>> Signaled;
    };

    // A queue of commands to be executed on the GPU.
    class CommandQueue : public NonCopyable
    {
    public:
        virtual ~CommandQueue() = default;

    public:
        virtual void Submit(
            const Array<SharedPtr<CommandList>>& commandLists,
            const CommandQueueSubmitInformation& information,
            const SharedPtr<Fence>& fence) = 0;

        virtual void WaitIdle() = 0;
    };
}
