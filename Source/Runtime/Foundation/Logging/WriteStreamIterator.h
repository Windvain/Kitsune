#pragma once

#include "Foundation/Logging/IStream.h"

namespace Kitsune
{
    template<typename T>
    class WriteStreamIterator
    {
    public:
        using ValueType = T;
        using DifferenceType = Ptrdiff;

    public:
        WriteStreamIterator() = default;
        inline explicit WriteStreamIterator(IWriteStream<T>& stream)
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
        inline WriteStreamIterator& operator++()   { return *this; }
        inline WriteStreamIterator operator++(int) { return *this; }

        inline WriteStreamIterator& operator*() { return *this; }

    public:
        inline IWriteStream<T>* GetStream() const { return m_Stream; }

    private:
        IWriteStream<T>* m_Stream;
    };
}
