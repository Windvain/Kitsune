#pragma once

#include "Foundation/Common/Types.h"
#include "Foundation/Diagnostics/Assert.h"

#include "Foundation/String/String.h"
#include "Foundation/Containers/Array.h"

#include "Foundation/Diagnostics/OutOfRangeException.h"
#include "Foundation/Diagnostics/InvalidArgumentException.h"

namespace Kitsune
{
    class CommandLineArguments
    {
    public:
        using Iterator = Array<String>::ConstIterator;

    public:
        CommandLineArguments() = default;
        inline CommandLineArguments(int argc, char** argv)
        {
            // Clients *may* input negative or zero values.
            // POSIX does specify that `argc` can be 0, but just because it can be inputted
            // doesn't mean it should be.
            KITSUNE_ASSERT(argc > 0, "Command line arguments should not be empty.");

            m_Arguments.Reserve(argc);

            for (int i = 0; i < argc; ++i)
                m_Arguments.PushBack(argv[i]);
        }

        inline CommandLineArguments(const Array<String>& args)
            : m_Arguments(args)
        {
        }

        inline CommandLineArguments(Array<String>&& args)
            : m_Arguments(Move(args))
        {
        }

    public:
        inline const String& operator[](Index index) const
        {
            if (index >= Count()) throw OutOfRangeException();
            return m_Arguments[index];
        }

    public:
        inline Usize Count() const { return m_Arguments.Size(); }

    public:
        inline Iterator GetBegin() const { return m_Arguments.GetBegin(); }
        inline Iterator GetEnd()   const { return m_Arguments.GetEnd(); }

    public:
        inline Iterator begin() const { return GetBegin(); }
        inline Iterator end()   const { return GetEnd(); }

    private:
        Array<String> m_Arguments;
    };
}
