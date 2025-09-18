#pragma once

#include "Foundation/Common/Types.h"

namespace Kitsune
{
    template<typename T>
    class IWriteStream
    {
    public:
        virtual ~IWriteStream() { /* ... */ }

        virtual void Write(const T* ptr, Usize count) = 0;
        virtual void Flush() { /* ... */ }
    };

    template<typename T>
    class IReadStream
    {
    public:
        virtual ~IReadStream() { /* ... */ }

        virtual void Read(IWriteStream<T>& stream) = 0;
        virtual void Read(IWriteStream<T>& stream, char delimiter) = 0;
    };

    template<typename T>
    class IReadWriteStream : public IWriteStream<T>, public IReadStream<T>
    {
    };
}
