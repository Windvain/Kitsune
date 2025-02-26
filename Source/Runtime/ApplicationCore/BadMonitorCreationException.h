#pragma once

#include "Foundation/Diagnostics/IException.h"

namespace Kitsune
{
    class BadMonitorCreationException: public IException
    {
    public:
        BadMonitorCreationException() = default;
        BadMonitorCreationException(const char* desc) : m_Description(desc) { /* ... */ }

    public:
        const char* GetName() const noexcept override { return "BadMonitorCreationException"; }
        const char* GetDescription() const noexcept override { return m_Description; }

    private:
        const char* m_Description = "Unknown monitor error";
    };
}
