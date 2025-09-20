#pragma once

#include <exception>

#include "Foundation/Common/Types.h"
#include "Foundation/Common/Macros.h"

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

    private:
        void WriteExceptionData(const void* ptr, Usize bytes);

    public:
        // Make it easier to debug..
        const char* what() const noexcept override
        {
            return GetName();
        }
    };
}
