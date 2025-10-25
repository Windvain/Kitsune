#pragma once

#include <exception>
#include "Foundation/Common/Types.h"

namespace Kitsune
{
    class IException : public std::exception
    {
    public:
        IException() noexcept;
        IException(const char* name, const char* desc) noexcept;

        virtual ~IException() noexcept;

    public:
        const char* GetName() const noexcept;
        const char* GetDescription() const noexcept;

    public:
        const char* what() const noexcept override
        {
            return GetName();
        }
    };
}
