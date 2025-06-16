#pragma once

#include "Foundation/Utilities/NonCopyable.h"

namespace Kitsune
{
    class ITexture : public NonCopyable
    {
    public:
        virtual ~ITexture() { /* ... */ }
    };
}
