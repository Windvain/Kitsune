#pragma once

#include "Foundation/Streams/Stream.h"
#include "Foundation/Memory/AddressOf.h"

namespace Kitsune
{
    // An output iterator which is used for outputting elements into a writer.
    template<Writer T>
    class WriterIterator
    {
    public:
        using ValueType = typename T::ValueType;
        using DifferenceType = Ptrdiff;

    public:
        WriterIterator() = default;
        inline explicit WriterIterator(T& writer)
            : m_Writer(AddressOf(writer))
        {
        }

    public:
        inline WriterIterator& operator=(const ValueType& value)
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
        inline T* GetWriter() const
        {
            return m_Writer;
        }

    private:
        T* m_Writer = nullptr;
    };
}
