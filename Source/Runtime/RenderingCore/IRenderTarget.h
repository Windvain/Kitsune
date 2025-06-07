#pragma once

#include "RenderingCore/ITexture.h"

#include "Foundation/Memory/SharedPtr.h"
#include "Foundation/Utilities/NonCopyable.h"

namespace Kitsune
{
    struct RenderTargetSpecs
    {
        SharedPtr<ITexture> Texture;
    };

    class IRenderTarget : public NonCopyable
    {
    public:
        virtual ~IRenderTarget() { /* ... */ }

    public:
        virtual SharedPtr<ITexture> GetTexture() const = 0;
    };
}
