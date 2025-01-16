#pragma once

#include <exception>

namespace Kitsune
{
    class IException : public std::exception
    {
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
