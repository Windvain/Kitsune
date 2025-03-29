#pragma once

#include "Foundation/Common/Types.h"
#include "Foundation/Diagnostics/IException.h"

namespace Kitsune
{
    class InvalidArgumentException : public IException
    {
    public:
        InvalidArgumentException() = default;
        InvalidArgumentException(const char* desc)
            : m_Description(desc)
        {
        }

    public:
        const char* GetName() const noexcept override { return "InvalidArgumentException"; }
        const char* GetDescription() const noexcept override
        {
            return m_Description;
        }

    private:
        const char* m_Description = "Invalid argument passed";
    };
}
