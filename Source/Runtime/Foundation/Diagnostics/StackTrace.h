#pragma once

#include "Foundation/String/String.h"
#include "Foundation/Containers/Array.h"

#include "Foundation/Diagnostics/StackFrame.h"

namespace Kitsune
{
    class StackTrace
    {
    public:
        using ContainerType = Array<StackFrame>;

        using Iterator = ContainerType::ConstIterator;
        using ConstIterator = ContainerType::ConstIterator;

        using ReverseIterator = ContainerType::ReverseIterator;
        using ReverseConstIterator = ContainerType::ReverseConstIterator;

    public:
        StackTrace() = default;
        inline StackTrace(Array<StackFrame>&& stackFrames, const StringView threadName)
            : m_StackFrames(Move(stackFrames)), m_CallingThreadName(threadName)
        {
        }

    public:
        inline const StackFrame& operator[](Index index) const
        {
            if (index >= m_StackFrames.Size())
                throw OutOfRangeException();

            return m_StackFrames[index];
        }

    public:
        [[nodiscard]] inline Usize Size() const { return m_StackFrames.Size(); }
        [[nodiscard]] inline bool IsEmpty() const { return m_StackFrames.IsEmpty(); }

    public:
        [[nodiscard]]
        inline String GetCallingThreadName() const { return m_CallingThreadName; }

    public:
        [[nodiscard]] inline ConstIterator GetBegin() const { return m_StackFrames.GetBegin(); }
        [[nodiscard]] inline ConstIterator GetEnd() const { return m_StackFrames.GetEnd(); }

        [[nodiscard]]
        inline ReverseConstIterator GetReverseBegin() const
        {
            return m_StackFrames.GetReverseBegin();
        }

        [[nodiscard]]
        inline ReverseConstIterator GetReverseEnd() const
        {
            return m_StackFrames.GetReverseEnd();
        }

    public:
        inline void Swap(StackTrace& stackTrace)
        {
            m_StackFrames.Swap(stackTrace.m_StackFrames);
            m_CallingThreadName.Swap(stackTrace.m_CallingThreadName);
        }

    public:
        inline bool operator==(const StackTrace& st) const
        {
            return (m_StackFrames == st.m_StackFrames) &&
                   (m_CallingThreadName == st.m_CallingThreadName);
        }

    public:
        // Should not be called by engine/client code.
        // Made public so that the compiler can generate code for range-based for loops.
        inline ConstIterator begin() const { return GetBegin(); }
        inline ConstIterator end() const { return GetEnd(); }

    private:
        Array<StackFrame> m_StackFrames;
        String m_CallingThreadName;
    };

    KITSUNE_API_ StackTrace MakeStackTrace(Usize skipCount = 0, Usize maxDepth = Usize(-1));

    namespace Algorithms
    {
        inline void Swap(StackTrace& lhs, StackTrace& rhs)
        {
            return lhs.Swap(rhs);
        }
    }
}
