#pragma once

#include <concepts>
#include "Foundation/Common/Types.h"

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

    // A concept representing a stream of untyped data in the form of bytes, which
    // can be written to.
    template<typename T>
    concept WritableStream = requires (T stream, const T constStream,
                                       const Byte* data, Usize dataCount)
    {
        stream.Write(data, dataCount);
        { constStream.IsWritable() } -> std::convertible_to<bool>;
    };

    // A concept representing a stream of untyped data in the form of bytes, which
    // can be read from.
    template<typename T>
    concept ReadableStream = requires (T stream, const T constStream,
                                       Byte* buffer, Usize bufferSize)
    {
        { stream.Read(buffer, bufferSize) } -> std::convertible_to<bool>;
        { constStream.IsReadable() } -> std::convertible_to<bool>;
    };

    // A concept representing a stream of untyped data in the form of bytes, which
    // can be written to or read from.
    template<typename T>
    concept Stream = WritableStream<T> || ReadableStream<T>;

    // A concept representing a stream which can be flushed.
    template<typename T>
    concept FlushableStream =
        WritableStream<T> &&
        requires (T stream)
        {
            stream.Flush();
        };

    // A stream which has a known length.
    template<typename T>
    concept BoundedStream =
        Stream<T> &&
        requires (const T stream)
        {
            { stream.Length() } -> std::same_as<Usize>;
        };

    // A stream which can be seeked.
    template<typename T>
    concept SeekableStream =
        BoundedStream<T> &&
        requires (T stream, const T constStream, Ptrdiff offset, SeekOrigin origin)
        {
            { stream.Seek(offset, origin) } -> std::same_as<Usize>;
            { constStream.IsSeekable() } -> std::convertible_to<bool>;

            { constStream.GetPosition() } -> std::same_as<Usize>;
        };

    // A concept representing a type which writes typed data to streams.
    // It is meant as a higher-level abstraction to streams, used for interpreting text,
    // or any type at all, into bytes which can be written to streams.
    template<typename T>
    concept Writer = requires (
        T writer,
        const typename T::ValueType* data, Usize dataCount)
    {
        typename T::ValueType;

        writer.Write(data, dataCount);
        writer.Flush();
    };

    // A concept which represents a type which reads raw data from a stream and
    // interprets it as typed data.
    template<typename T>
    concept Reader = requires (
        T reader,
        typename T::ValueType* buffer, Usize bufferCount)
    {
        { reader.Read(buffer, bufferCount) } -> std::same_as<Usize>;
    };
}
