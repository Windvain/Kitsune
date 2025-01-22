#pragma once

#include "Foundation/Common/Types.h"
#include "Foundation/Diagnostics/Assert.h"

#include "Foundation/Diagnostics/OutOfRangeException.h"

namespace Kitsune
{
    class CommandLineArguments
    {
    public:
        using ArgumentType = const char*;
        using Iterator = ArgumentType*;

    public:
        CommandLineArguments() = default;
        CommandLineArguments(int argc, char** argv)
            : m_Count(static_cast<Usize>(argc)),
              m_Arguments(const_cast<const char**>(argv))
        {
            // Clients *may* input negative or zero values.
            // POSIX does specify that `argc` can be 0, but just because it can be inputted
            // doesn't mean it should be.
            KITSUNE_ASSERT(argc >= 0, "Arguments count should be non-negative.");
        }

    public:
        inline const char* operator[](Index index) const
        {
            if (index >= m_Count) throw OutOfRangeException();
            return m_Arguments[index];
        }

    public:
        inline Usize Count() const { return m_Count; }
        inline const char** Data() const { return m_Arguments; }

    public:
        inline Iterator GetBegin() const { return m_Arguments; }
        inline Iterator GetEnd()   const { return (m_Arguments + m_Count); }

    public:
        inline Iterator begin() const { return GetBegin(); }
        inline Iterator end()   const { return GetEnd(); }

    private:
        Usize m_Count;
        const char** m_Arguments;
    };
}
