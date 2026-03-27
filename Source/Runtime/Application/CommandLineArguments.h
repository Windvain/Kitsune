#pragma once

#include "Foundation/Common/Types.h"

#include "Foundation/Containers/Array.h"
#include "Foundation/String/StringView.h"

#include "Foundation/Diagnostics/OutOfRangeException.h"
#include "Foundation/Diagnostics/InvalidArgumentException.h"

namespace Kitsune
{
    // Container class used to store command-line arguments in a more C++ way.
    class CommandLineArguments
    {
    public:
        using ValueType = const StringView;

        using Iterator = Array<StringView>::ConstIterator;
        using ConstIterator = Array<StringView>::ConstIterator;

    public:
        CommandLineArguments() = default;
        inline CommandLineArguments(int argc, char** argv)
        {
            // POSIX specifies that argc can be set to zero/negative values.
            if (argc <= 0)
            {
                throw InvalidArgumentException(
                    "CommandLineArguments should be supplied with at "
                    "least one argument!");
            }

            m_Arguments.Reserve(argc);

            for (int i = 0; i < argc; ++i)
                m_Arguments.PushBack(argv[i]);
        }

        inline explicit CommandLineArguments(const Array<StringView>& args)
            : m_Arguments(args)
        {
        }

        inline explicit CommandLineArguments(Array<StringView>&& args)
            : m_Arguments(Move(args))
        {
        }

    public:
        inline const StringView& operator[](Index index) const
        {
            if (index >= Count())
                throw OutOfRangeException();

            return m_Arguments[index];
        }

    public:
        [[nodiscard]]
        inline Usize Count() const
        {
            return m_Arguments.Size();
        }

    public:
        [[nodiscard]]
        inline Iterator GetBegin() const
        {
            return m_Arguments.GetBegin();
        }

        [[nodiscard]]
        inline Iterator GetEnd() const
        {
            return m_Arguments.GetEnd();
        }

    public:
        // Should not be called by engine/client code.
        // Made public so that the compiler can generate code for range-based for loops.
        [[nodiscard]] inline Iterator begin() const { return GetBegin(); }
        [[nodiscard]] inline Iterator end() const { return GetEnd(); }

    private:
        Array<StringView> m_Arguments;
    };
}
