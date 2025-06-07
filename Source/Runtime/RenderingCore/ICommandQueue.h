#pragma once

#include "RenderingCore/IRenderTarget.h"

#include "Foundation/Containers/Array.h"
#include "Foundation/Utilities/NonCopyable.h"

namespace Kitsune
{
    enum class CommandBufferType
    {
        Graphics,
        Compute,
        Copy
    };

    class ICommandQueue : public NonCopyable
    {
    public:
        virtual ~ICommandQueue() { /* ... */ }

    public:
        virtual CommandBufferType GetType() const = 0;

    public:
        virtual void BeginCommandList() = 0;
        virtual void EndCommandList() = 0;

        virtual void ExecuteCommandLists() = 0;
        virtual void WaitFinished() = 0;

        virtual void SetRenderTargets(const Array<SharedPtr<IRenderTarget>>& targets) = 0;
        virtual void ClearRenderTargets(float red, float green, float blue, float alpha) = 0;
    };
}
