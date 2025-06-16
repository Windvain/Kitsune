#pragma once

#include "ApplicationCore/IWindow.h"
#include "RenderingCore/IRenderTarget.h"

#include "Foundation/Memory/SharedPtr.h"
#include "Foundation/Utilities/NonCopyable.h"

namespace Kitsune
{
    struct SwapChainSpecs
    {
        SharedPtr<IWindow> Window;
        Uint32 BufferCount;

        bool VsyncEnabled;
    };

    class ISwapChain : public NonCopyable
    {
    public:
        virtual ~ISwapChain() { /* ... */ }

    public:
        virtual void Present() = 0;
        virtual SharedPtr<IRenderTarget> GetCurrentBackBuffer() const = 0;
    };
}
