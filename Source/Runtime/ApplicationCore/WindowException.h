#pragma once

#include "Foundation/Diagnostics/IException.h"

namespace Kitsune
{
    class WindowException : public IException
    {
    public:
        WindowException() = default;
        WindowException(const char* desc) : m_Description(desc) { /* ... */ }

    public:
        const char* GetName() const noexcept override { return "WindowException"; }
        const char* GetDescription() const noexcept override { return m_Description; }

    private:
        const char* m_Description = "Unknown window error";
    };
}
