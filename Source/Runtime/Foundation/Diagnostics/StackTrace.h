#pragma once

#include "Foundation/Memory/SharedPtr.h"
#include "Foundation/Containers/Array.h"

#include "Foundation/Diagnostics/StackFrame.h"

namespace Kitsune
{
    namespace Details
    {
        using StackTraceFillCallback = void(*)(SharedPtr<StackFrame>, void*);
        KITSUNE_API_ void DoBackTrace(Usize skipCount, Usize maxDepth,
                                      StackTraceFillCallback callback, void* data);

        template<Allocator Alloc>
        class StackTraceIterator
        {
        private:
            using IteratorType = Array<SharedPtr<StackFrame>, Alloc>::ConstIterator;

        public:
            using ValueType = const StackFrame;
            using DifferenceType = typename IteratorTraits<IteratorType>::DifferenceType;

        public:
            inline StackTraceIterator() = default;
            inline StackTraceIterator(IteratorType iterator)
                : m_Iterator(iterator)
            {
            }

        public:
            inline const ValueType& operator*() const
            {
                return **m_Iterator;
            }

            inline const ValueType& operator[](Index index) const
            {
                return *(*this + index);
            }

        public:
            inline StackTraceIterator& operator++()
            {
                ++m_Iterator;
                return *this;
            }

            inline StackTraceIterator operator++(int)
            {
                IteratorType copy = *this;
                ++m_Iterator;

                return copy;
            }

            inline StackTraceIterator& operator--()
            {
                --m_Iterator;
                return *this;
            }

            inline StackTraceIterator operator--(int)
            {
                IteratorType copy = *this;
                --m_Iterator;

                return copy;
            }

            inline StackTraceIterator& operator+=(DifferenceType n)
            {
                m_Iterator += n;
                return *this;
            }

            inline StackTraceIterator operator+(DifferenceType n) const
            {
                IteratorType copy = *this;
                copy += n;

                return copy;
            }

            inline StackTraceIterator& operator-=(DifferenceType n)
            {
                m_Iterator -= n;
                return *this;
            }

            inline StackTraceIterator operator-(DifferenceType n) const
            {
                IteratorType copy = *this;
                copy -= n;

                return copy;
            }

        public:
            IteratorType GetBase() const { return m_Iterator; }

        private:
            IteratorType m_Iterator;
        };

        template<Allocator Alloc>
        inline StackTraceIterator<Alloc> operator+(typename StackTraceIterator<Alloc>::DifferenceType n,
                                                   StackTraceIterator<Alloc> it)
        {
            return (it + n);
        }

        template<Allocator Alloc>
        inline bool operator==(StackTraceIterator<Alloc> it1, StackTraceIterator<Alloc> it2)
        {
            return (it1.GetBase() == it2.GetBase());
        }

        template<Allocator Alloc>
        inline bool operator!=(StackTraceIterator<Alloc> it1, StackTraceIterator<Alloc> it2)
        {
            return (it1.GetBase() != it2.GetBase());
        }

        template<Allocator Alloc>
        inline bool operator>=(StackTraceIterator<Alloc> it1, StackTraceIterator<Alloc> it2)
        {
            return (it1.GetBase() >= it2.GetBase());
        }

        template<Allocator Alloc>
        inline bool operator<=(StackTraceIterator<Alloc> it1, StackTraceIterator<Alloc> it2)
        {
            return (it1.GetBase() <= it2.GetBase());
        }

        template<Allocator Alloc>
        inline bool operator>(StackTraceIterator<Alloc> it1, StackTraceIterator<Alloc> it2)
        {
            return (it1.GetBase() > it2.GetBase());
        }

        template<Allocator Alloc>
        inline bool operator<(StackTraceIterator<Alloc> it1, StackTraceIterator<Alloc> it2)
        {
            return (it1.GetBase() < it2.GetBase());
        }

        template<Allocator Alloc>
        inline auto operator-(StackTraceIterator<Alloc> it1, StackTraceIterator<Alloc> it2)
            -> decltype(it2.GetBase() - it1.GetBase())
        {
            return (it2.GetBase() - it1.GetBase());
        }
    }

    template<Allocator Alloc>
    class BasicStackTrace
    {
    public:
        using ValueType = StackFrame;

        using Iterator = Details::StackTraceIterator<Alloc>;
        using ConstIterator = Iterator;

        using ReverseIterator = Kitsune::ReverseIterator<ConstIterator>;
        using ReverseConstIterator = Kitsune::ReverseIterator<ConstIterator>;

    public:
        inline BasicStackTrace() = default;

    public:
        inline const ValueType& operator[](Index index) const
        {
            if (index >= m_StackFrames.Size())
                throw OutOfRangeException();

            return *m_StackFrames[index];
        }

    public:
        [[nodiscard]] inline Usize Size() const { return m_StackFrames.Size(); }
        [[nodiscard]] inline bool IsEmpty() const { return m_StackFrames.IsEmpty(); }

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
        inline void Swap(BasicStackTrace<Alloc>& stackTrace)
        {
            m_StackFrames.Swap(stackTrace.m_StackFrames);
        }

    public:
        inline bool operator==(const BasicStackTrace<Alloc>& stackTrace) const
        {
            return (m_StackFrames == stackTrace.m_StackFrames);
        }

    public:
        inline static BasicStackTrace<Alloc> Current(Usize skipCount = 1,
                                                     Usize maxDepth = Usize(-1))
        {
            Array<SharedPtr<StackFrame>, Alloc> frames;
            Details::DoBackTrace(skipCount, maxDepth, FillCallback, &frames);

            for (auto& stackFrame : frames)
                KITSUNE_ASSERT(stackFrame.Get() != nullptr, "StackFrame should never contain null pointers.");

            return BasicStackTrace<Alloc>(Move(frames));
        }

    private:
        inline BasicStackTrace(Array<SharedPtr<StackFrame>, Alloc>&& frames)
            : m_StackFrames(Move(frames))
        {
        }

        inline static void FillCallback(SharedPtr<StackFrame> frame, void* data)
        {
            auto& frames = *reinterpret_cast<Array<SharedPtr<StackFrame>, Alloc>*>(data);
            frames.PushBack(Move(frame));
        }

    public:
        // Should not be called by engine/client code.
        // Made public so that the compiler can generate code for range-based for loops.
        inline ConstIterator begin() const { return GetBegin(); }
        inline ConstIterator end() const { return GetEnd(); }

    private:
        Array<SharedPtr<ValueType>, Alloc> m_StackFrames;
    };

    using StackTrace = BasicStackTrace<GlobalAllocator>;
}
