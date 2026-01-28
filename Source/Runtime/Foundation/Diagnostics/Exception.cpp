#include "Foundation/Diagnostics/Exception.h"
#include "Launch/EngineLoop.h"

namespace Kitsune
{
    Exception::Exception() noexcept
        : Exception("<unknown>", "")
    {
    }

    Exception::Exception(const char* name, const char* description) noexcept
    {
        m_Name = name;
        m_Description = description;

        EngineLoop::GetInstance()->CaptureExceptionBacktrace();
    }

    Exception::~Exception() noexcept
    {
    }

    const char* Exception::GetName() const noexcept
    {
        return m_Name;
    }

    const char* Exception::GetDescription() const noexcept
    {
        return m_Description;
    }
}
