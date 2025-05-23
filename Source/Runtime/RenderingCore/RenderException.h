#pragma once

#include "Foundation/Common/Types.h"
#include "Foundation/Diagnostics/IException.h"

namespace Kitsune
{
    class RenderException : public IException
    {
    public:
        RenderException() = default;
        RenderException(const char* desc)
            : m_Description(desc)
        {
        }

    public:
        const char* GetName() const noexcept override { return "RenderException"; }
        const char* GetDescription() const noexcept override
        {
            return m_Description;
        }

    private:
        const char* m_Description = "Unknown render error occured";
    };
}
