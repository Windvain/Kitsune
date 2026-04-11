#pragma once

#include "Foundation/Streams/Stream.h"

namespace Kitsune
{
    // An output iterator which is used for outputting elements into a writer.
    template<typename T>
    class WriterIterator
    {
    public:
        using ValueType = T;
        using DifferenceType = Ptrdiff;

    public:
        WriterIterator() = default;
        inline explicit WriterIterator(Writer<T>& writer)
            : m_Writer(&writer)
        {
        }

    public:
        inline WriterIterator& operator=(const T& value)
        {
            m_Writer->Write(&value, 1);
            return *this;
        }

    public:
        inline WriterIterator& operator++() { return *this; }
        inline WriterIterator operator++(int) { return *this; }

        inline WriterIterator& operator*()
        {
            return *this;
        }

    public:
        [[nodiscard]]
        inline Writer<T>* GetWriter() const
        {
            return m_Writer;
        }

    private:
        Writer<T>* m_Writer = nullptr;
    };
}
