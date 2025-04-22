#pragma once

#include "Foundation/Diagnostics/IException.h"

namespace Kitsune
{
    class MonitorException : public IException
    {
    public:
        MonitorException() = default;
        MonitorException(const char* desc) : m_Description(desc) { /* ... */ }

    public:
        const char* GetName() const noexcept override { return "MonitorException"; }
        const char* GetDescription() const noexcept override { return m_Description; }

    private:
        const char* m_Description = "Unknown monitor error";
    };
}
