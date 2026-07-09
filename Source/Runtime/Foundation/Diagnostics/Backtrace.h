#pragma once

#include "Foundation/String/Format.h"
#include "Foundation/Containers/Array.h"

#include "Foundation/Diagnostics/BacktraceFrame.h"

namespace Kitsune
{
    // Represents a sequence of active functions calls leading up to
    // the call which creates this class (Backtrace::Capture()).
    class KITSUNE_API Backtrace
    {
    public:
        using ValueType = const BacktraceFrame;
        using AllocatorType = GlobalAllocator;

        using ContainerType = Array<BacktraceFrame, AllocatorType>;

        using Iterator = ValueType*;
        using ConstIterator = const ValueType*;

        using ReverseIterator = Kitsune::ReverseIterator<Iterator>;
        using ReverseConstIterator = Kitsune::ReverseIterator<ConstIterator>;

    public:
        inline Backtrace() = default;

    public:
        inline const BacktraceFrame& operator[](Index index) const
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
        [[nodiscard]]
        inline ConstIterator GetBegin() const
        {
            return m_Frames.GetBegin();
        }

        [[nodiscard]]
        inline ConstIterator GetEnd() const
        {
            return m_Frames.GetEnd();
        }

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
        inline void Swap(Backtrace& backtrace)
        {
            m_Frames.Swap(backtrace.m_Frames);
        }

    public:
        inline bool operator==(const Backtrace& backtrace) const
        {
            return (m_Frames == backtrace.m_Frames);
        }

    public:
        [[nodiscard]]
        static Backtrace Capture(
            Uint32 skipCount = 0, Uint32 maxDepth = Uint32(-1)) noexcept;

        [[nodiscard]]
        constexpr static bool IsSupported()
        {
#if defined(KITSUNE_SUPPORTS_BACKTRACES)
            return true;
#else
            return false;
#endif
        }

    private:
        inline explicit Backtrace(ContainerType&& frames)
            : m_Frames(Move(frames))
        {
        }

    public:
        // Should not be called by engine/client code.
        // Made public so that the compiler can generate code for
        // range-based for loops.
        [[nodiscard]] inline ConstIterator begin() const { return GetBegin(); }
        [[nodiscard]] inline ConstIterator end() const { return GetEnd(); }

    private:
        ContainerType m_Frames;
    };

    static_assert(
        Container<Backtrace>,
        "Backtrace doesn't satisfy the requirements of a Container.");

    template<>
    class Formatter<Backtrace, char>
    {
    public:
        template<OutputIterator<const char&> Iter>
        inline static Iter Format(
            const Backtrace& backtrace,
            const FormatContext<Iter>& context)
        {
            auto output = context.GetOutput();
            output = FormatTo(output, "Stack backtrace:\n");

            if (backtrace.IsEmpty())
            {
                output = FormatTo(output, "<empty backtrace>");
                return output;
            }

            Index index = 1;
            for (const BacktraceFrame& frame : backtrace)
            {
                // \t0: 0xBADF00D - bar::foo()
                // \t\t-> meow.cpp:3
                output = FormatTo(
                    output, "\t{0}: {1} - {2}\n\t\t-> {3}:{4}\n",
                    index,
                    frame.GetAddress(), frame.GetSymbolName(),
                    frame.GetFileName(), frame.GetLineNumber());

                ++index;
            }

            return output;
        }
    };
}
