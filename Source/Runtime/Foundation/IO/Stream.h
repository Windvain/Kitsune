#pragma once

#include "Foundation/Common/Types.h"

namespace Kitsune
{
    template<typename T>
    class OutputStream
    {
    public:
        virtual ~OutputStream() { /* ... */ }

        virtual void Write(const T* pointer, Usize count) = 0;
        virtual void Flush() { /* ... */ }
    };

    template<typename T>
    class InputStream
    {
    public:
        virtual ~InputStream() { /* ... */ }

        virtual void Read(OutputStream<T>& stream) = 0;
        virtual void Read(OutputStream<T>& stream, char delimiter) = 0;
    };

    template<typename T>
    class InputOutputStream : public OutputStream<T>, public InputStream<T>
    {
    };
}
