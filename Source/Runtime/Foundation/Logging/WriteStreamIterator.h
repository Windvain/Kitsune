#pragma once

#include "Foundation/IO/Stream.h"

namespace Kitsune
{
    // An output iterator which is used for writing elements into a stream. The
    // assignment operator writes to the kept output stream using the `Write()`
    // member function. The increment and dereference operators are no-ops.
    template<typename T>
    class WriteStreamIterator
    {
    public:
        using ValueType = T;
        using DifferenceType = Ptrdiff;

    public:
        WriteStreamIterator() = default;
        inline explicit WriteStreamIterator(OutputStream<T>& stream)
            : m_Stream(&stream)
        {
        }

    public:
        inline WriteStreamIterator& operator=(const T& value)
        {
            m_Stream->Write(&value, 1);
            return *this;
        }

    public:
        inline WriteStreamIterator& operator++()
        {
            return *this;
        }

        inline WriteStreamIterator operator++(int)
        {
            return *this;
        }

        inline WriteStreamIterator& operator*()
        {
            return *this;
        }

    public:
        inline OutputStream<T>* GetStream() const
        {
            return m_Stream;
        }

    private:
        OutputStream<T>* m_Stream = nullptr;
    };
}
