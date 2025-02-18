#pragma once

#include "Foundation/Diagnostics/IException.h"

namespace Kitsune
{
    class BadWindowCreationException: public IException
    {
    public:
        BadWindowCreationException() = default;
        BadWindowCreationException(const char* desc) : m_Description(desc) { /* ... */ }

    public:
        const char* GetName() const noexcept override { return "BadWindowCreationException"; }
        const char* GetDescription() const noexcept override { return m_Description; }

    private:
        const char* m_Description = "Unknown window error";
    };
}
