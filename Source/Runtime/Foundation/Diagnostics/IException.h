#pragma once

#include <exception>

namespace Kitsune
{
    class IException : public std::exception
    {
    public:
        virtual const char* GetName() const = 0;
        virtual const char* GetDescription() const = 0;
    };
}
