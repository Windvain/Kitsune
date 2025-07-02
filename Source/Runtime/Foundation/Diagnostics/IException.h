#pragma once

#include <exception>

#include "Foundation/Common/Types.h"
#include "Foundation/Common/Macros.h"

namespace Kitsune
{
    class IException : public std::exception
    {
    public:
        KITSUNE_API_ IException() noexcept;
        KITSUNE_API_ IException(const char* name, const char* desc) noexcept;

        KITSUNE_API_ virtual ~IException() noexcept;

    public:
        KITSUNE_API_ const char* GetName() const noexcept;
        KITSUNE_API_ const char* GetDescription() const noexcept;

    private:
        KITSUNE_API_ void WriteExceptionData(const void* ptr, Usize bytes);

    public:
        // Make it easier to debug..
        const char* what() const noexcept override
        {
            return GetName();
        }
    };
}
