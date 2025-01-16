#pragma once

#include "Foundation/Diagnostics/IException.h"

namespace Kitsune
{
    class FormatException : public IException
    {
    public:
        FormatException() = default;
        FormatException(const char* desc) : m_Description(desc) { /* ... */ }

    public:
        const char* GetName() const noexcept override { return "FormatException"; }
        const char* GetDescription() const noexcept override { return m_Description; }

    private:
        const char* m_Description = "Unknown format error";
    };
}
