#pragma once

#include "Foundation/Utilities/NonCopyable.h"

namespace Kitsune
{
    // A wrapper around shader code and its entry point(s).
    class ShaderModule : public NonCopyable
    {
    public:
        virtual ~ShaderModule() = default;
    };
}
