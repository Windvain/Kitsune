#pragma once

#include <exception>
#include "Foundation/Common/Macros.h"

namespace Kitsune
{
    class IException : public std::exception
    {
    public:
        KITSUNE_API_ IException();
        KITSUNE_API_ virtual ~IException();

    public:
        virtual const char* GetName() const noexcept = 0;
        virtual const char* GetDescription() const noexcept = 0;

    public:
        // Make it easier to debug..
        const char* what() const noexcept override
        {
            return GetName();
        }
    };
}
