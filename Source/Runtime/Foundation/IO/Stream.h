#pragma once

#include "Foundation/Common/Types.h"

namespace Kitsune
{
    // An abstract class defining a stream which can be written to.
    template<typename T>
    class OutputStream
    {
    public:
        virtual ~OutputStream() = default;

        virtual void Write(const T* pointer, Usize count) = 0;
        virtual void Flush() { /* ... */ }
    };

    // An abstract class defining a stream which can be read from.
    template<typename T>
    class InputStream
    {
    public:
        virtual ~InputStream() = default;

        virtual void Read(OutputStream<T>& stream) = 0;
        virtual void Read(OutputStream<T>& stream, char delimiter) = 0;
    };

    // An abstract class defining a stream which has the characteristics of
    // both an output and an input stream.
    template<typename T>
    class InputOutputStream : public OutputStream<T>, public InputStream<T>
    {
    };
}
