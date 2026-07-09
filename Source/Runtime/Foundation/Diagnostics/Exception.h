#pragma once

#include <exception>
#include "Foundation/Common/Macros.h"

namespace Kitsune
{
    namespace Details
    {
        class ExceptionData;
    }

    // Forward-declare this, because including backtrace MIGHT cause cyclical
    // dependencies.
    class Backtrace;

    // The engine's own class for an exception. Practically the same as a regular
    // std::exception, but with additional information such as the exception name
    // and description.
    class KITSUNE_API Exception : public std::exception
    {
    public:
        Exception() noexcept;
        Exception(const char* name, const char* description) noexcept;

        ~Exception() noexcept override;

    public:
        [[nodiscard]] const char* GetName() const noexcept;
        [[nodiscard]] const char* GetDescription() const noexcept;

        [[nodiscard]]
        Backtrace* GetBacktrace() const noexcept;

    public:
        // Override the what() member function of std::exception, some compilers
        // will log it to the console if the exception escapes our try/catch block.
        [[nodiscard]]
        inline const char* what() const noexcept override
        {
            return GetName();
        }

    private:
        Details::ExceptionData* m_Data;
    };
}
