#pragma once

#include "Foundation/Common/Types.h"
#include "Foundation/Common/Macros.h"

#include "Foundation/Diagnostics/LogicException.h"

namespace Kitsune
{
    // Describes the position at which to offset the stream pointer.
    enum class SeekOrigin
    {
        Begin,      //< The stream pointer's position will be calculated
                    //  from the beginning of the stream.
        Current,    //< The stream pointer's position will be calculated
                    //  from the current position.
        End         //< The stream pointer's position will be calculated
                    //  from the end of the stream.
    };

    // An abstract class representing a stream of data (bytes) which
    // can be written to, or read from.
    class Stream
    {
    public:
        virtual ~Stream() = default;

    public:
        virtual void Write(const Byte* data, Usize dataCount) = 0;
        virtual Usize Read(Byte* buffer, Usize bufferSize) = 0;

        [[noreturn]]
        inline virtual Usize Seek(Ptrdiff offset, SeekOrigin origin)
        {
            KITSUNE_UNUSED(offset);
            KITSUNE_UNUSED(origin);

            throw LogicException("The stream is not seekable.");
        }

        inline Usize Seek(Ptrdiff offset)
        {
            return Seek(offset, SeekOrigin::Begin);
        }

    public:
        [[nodiscard]] virtual bool IsWritable() const = 0;
        [[nodiscard]] virtual bool IsReadable() const = 0;

        [[nodiscard]]
        inline virtual bool IsSeekable() const
        {
            return false;
        }

    public:
        inline virtual void Flush()
        {
        }

        [[nodiscard, noreturn]]
        inline virtual Usize GetPosition() const
        {
            throw LogicException(
                "Cannot get the position of an unseekable stream.");
        }
    };

    // An abstract class which writes data (types) to streams.
    template<typename T>
    class Writer
    {
    public:
        virtual ~Writer() = default;

    public:
        virtual void Write(const T* data, Usize dataCount) = 0;

        inline virtual void Flush()
        {
        }
    };
}
