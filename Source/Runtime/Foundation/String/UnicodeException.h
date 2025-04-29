#pragma once

#include "Foundation/Common/Types.h"
#include "Foundation/Diagnostics/IException.h"

namespace Kitsune
{
    class UnicodeException : public IException
    {
    public:
        UnicodeException() = default;
        UnicodeException(const char* desc)
            : m_Description(desc)
        {
        }

    public:
        const char* GetName() const noexcept override { return "UnicodeException"; }
        const char* GetDescription() const noexcept override
        {
            return m_Description;
        }

    private:
        const char* m_Description = "Unknown unicode error occured";
    };
}
