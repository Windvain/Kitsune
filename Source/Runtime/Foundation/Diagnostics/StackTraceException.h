#pragma once

#include "Foundation/Common/Types.h"
#include "Foundation/Diagnostics/IException.h"

namespace Kitsune
{
    class StackTraceException : public IException
    {
    public:
        StackTraceException() = default;
        StackTraceException(const char* desc)
            : m_Description(desc)
        {
        }

    public:
        const char* GetName() const noexcept override { return "StackTraceException"; }
        const char* GetDescription() const noexcept override
        {
            return m_Description;
        }

    private:
        const char* m_Description = "Unknown stack trace exception";
    };
}
