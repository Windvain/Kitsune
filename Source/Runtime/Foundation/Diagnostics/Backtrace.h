#pragma once

#include "Foundation/String/Format.h"
#include "Foundation/Containers/Array.h"

#include "Foundation/Diagnostics/BacktraceFrame.h"

namespace Kitsune
{
    namespace Details
    {
        // Just in case the backtrace is configured to use a custom memory API.
        using BacktraceAllocator = GlobalAllocator;
    }

    // Represents a sequence of active functions calls leading up to
    // the call which creates this class (Backtrace::Capture()).
    class Backtrace
    {
    public:
        using ValueType = BacktraceFrame;

        using Iterator = const ValueType*;
        using ConstIterator = Iterator;

        using ReverseIterator = Kitsune::ReverseIterator<ConstIterator>;
        using ReverseConstIterator = Kitsune::ReverseIterator<ConstIterator>;

    public:
        inline Backtrace() = default;

    public:
        inline const BacktraceFrame& operator[](const Index index)
        {
            if (index >= m_Frames.Size())
                throw OutOfRangeException();

            return m_Frames[index];
        }

    public:
        [[nodiscard]]
        inline Usize Size() const
        {
            return m_Frames.Size();
        }

        [[nodiscard]]
        inline bool IsEmpty() const
        {
            return m_Frames.IsEmpty();
        }

    public:
        [[nodiscard]] inline ConstIterator GetBegin() const { return m_Frames.GetBegin(); }
        [[nodiscard]] inline ConstIterator GetEnd() const { return m_Frames.GetEnd(); }

        [[nodiscard]]
        inline ReverseConstIterator GetReverseBegin() const
        {
            return m_Frames.GetReverseBegin();
        }

        [[nodiscard]]
        inline ReverseConstIterator GetReverseEnd() const
        {
            return m_Frames.GetReverseEnd();
        }

    public:
        inline void Swap(Backtrace& stackTrace)
        {
            m_Frames.Swap(stackTrace.m_Frames);
        }

    public:
        inline bool operator==(const Backtrace& stackTrace) const
        {
            return (m_Frames == stackTrace.m_Frames);
        }

    public:
        [[nodiscard]]
        static Backtrace Capture(Uint32 skipCount = 0,
                                 Uint32 maxDepth = Uint32(-1)) noexcept;

        [[nodiscard]]
        inline static bool IsSupported();

    private:
        inline Backtrace(Array<BacktraceFrame, Details::BacktraceAllocator>&& backtraceArray)
            : m_Frames(Move(backtraceArray))
        {
        }

    public:
        // Should not be called by engine/client code.
        // Made public so that the compiler can generate code for range-based for loops.
        inline ConstIterator begin() const { return GetBegin(); }
        inline ConstIterator end() const { return GetEnd(); }

    private:
        Array<BacktraceFrame, Details::BacktraceAllocator> m_Frames;
    };

    template<>
    class Formatter<Backtrace, char>
    {
    public:
        template<OutputIterator<const char&> Iter>
        inline static Iter Format(const Backtrace& backtrace,
                                  const FormatContext<Iter>& context)
        {
            auto output = context.GetOutput();
            output = FormatTo(output, "Stack backtrace:\n");

            if (backtrace.IsEmpty())
            {
                output = FormatTo(output, "<empty stacktrace>");
                return output;
            }

            Index index = 1;
            for (const BacktraceFrame& frame : backtrace)
            {
                //     0: 0xBADF00D - bar::foo()
                //         -> meow.cpp:3
                output = FormatTo(output, "\t{0}: {1} - {2}\n\t\t-> {3}:{4}\n",
                                  index,
                                  frame.GetAddress(), frame.GetSymbolName(),
                                  frame.GetFileName(), frame.GetLineNumber());

                ++index;
            }

            return output;
        }
    };
}
