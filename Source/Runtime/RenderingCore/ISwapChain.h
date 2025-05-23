#pragma once

#include "Foundation/Maths/Vector2.h"
#include "Foundation/Memory/SharedPtr.h"

#include "ApplicationCore/IWindow.h"

namespace Kitsune
{
    class IFence;

    struct SwapChainSpecs
    {
        bool Vsync = false;
        Uint32 BufferCount;

        SharedPtr<IWindow> Window;
    };

    class ISwapChain
    {
    public:
        virtual ~ISwapChain() { /* ... */ }

    public:
        virtual void Present() = 0;
        virtual void Resize(const Vector2<Uint32>& size) = 0;

        virtual Uint32 GetCurrentBackBufferIndex() const = 0;
        virtual SharedPtr<IFence> GetFence() = 0;

        virtual void WaitForPreviousFrame() = 0;

    public:
        virtual void SetVsyncEnabled(bool vsync) = 0;
        virtual bool IsVsyncEnabled() const = 0;
    };
}
