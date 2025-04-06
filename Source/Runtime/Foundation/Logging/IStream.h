#pragma once

#include "Foundation/Common/Types.h"

namespace Kitsune
{
    template<typename T>
    class IWriteStream
    {
    public:
        virtual void Write(const T* ptr, Usize count) = 0;
        virtual void Flush() { /* ... */ }
    };

    template<typename T>
    class IReadStream
    {
    public:
        virtual void Read(Usize count) = 0;
        virtual void Read() = 0;
    };

    template<typename T>
    class IStream : public IWriteStream<T>, public IReadStream<T>
    {
    };
}
