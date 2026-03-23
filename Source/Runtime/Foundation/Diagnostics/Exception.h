#pragma once

#include <exception>

namespace Kitsune
{
    namespace Details
    {
        class ExceptionData;
    }

    // The engine's own class for an exception. Practically the same as a regular
    // std::exception, but with additional information such as the exception name
    // and description.
    class Exception : public std::exception
    {
    public:
        Exception() noexcept;
        Exception(const char* name, const char* description) noexcept;

        virtual ~Exception() noexcept;

    public:
        const char* GetName() const noexcept;
        const char* GetDescription() const noexcept;

    public:
        // Override the what() member function of std::exception, some compilers
        // will log it to the console if the exception escapes our try/catch block.
        const char* what() const noexcept override
        {
            return GetName();
        }

    private:
        Details::ExceptionData* m_Data;
    };
}
