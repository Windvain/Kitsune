#include <gtest/gtest.h>

#include "TestContainer.h"
#include "PushBackContainer.h"
#include "Foundation/Iterators/BackInsertIterator.h"

#include "Foundation/String/Valid.h"
#include "Foundation/String/Decode.h"

#include "Foundation/String/Encode.h"
#include "Foundation/String/Transcode.h"

namespace
{
    using namespace Kitsune;
    using Testing::ForwardTestContainer, Testing::PushBackContainer;

    class TestEncoding
    {
    public:
        using CodepointType = long;
        using CodeunitType = Int32;

        template<typename Input, typename Output>
        inline static DecodeResult<Input, Output> DecodeSingle(
            Input begin, Input end,
            Output outBegin)
        {
            if (begin == end)
                return DecodeResult<Input, Output>(begin, outBegin);

            *outBegin = *begin + 1;
            return DecodeResult<Input, Output>(++begin, ++outBegin);
        }

        template<typename Input, typename Output>
        inline static EncodeResult<Input, Output> EncodeSingle(
            Input begin, Input end,
            Output outBegin)
        {
            if (begin == end)
                return EncodeResult<Input, Output>(begin, outBegin);

            *outBegin = *begin - 1;
            return EncodeResult<Input, Output>(++begin, ++outBegin);
        }
    };

    class TestEncoding2
    {
    public:
        using CodepointType = long;
        using CodeunitType = Int8;

        template<typename Input, typename Output>
        inline static DecodeResult<Input, Output> DecodeSingle(
            Input begin, Input end,
            Output outBegin)
        {
            if (begin == end)
                return DecodeResult<Input, Output>(begin, outBegin);

            *outBegin = *begin + 5;
            return DecodeResult<Input, Output>(++begin, ++outBegin);
        }

        template<typename Input, typename Output>
        inline static EncodeResult<Input, Output> EncodeSingle(
            Input begin, Input end,
            Output outBegin)
        {
            if (begin == end)
                return EncodeResult<Input, Output>(begin, outBegin);

            *outBegin = *begin - 5;
            return EncodeResult<Input, Output>(++begin, ++outBegin);
        }
    };

    class TestEncodingValid
    {
    public:
        using CodepointType = Int8;
        using CodeunitType = long;

        template<typename Input, typename Output>
        inline static DecodeResult<Input, Output> DecodeSingle(
            Input begin, Input end,
            Output outBegin)
        {
            if (begin == end)
                return DecodeResult<Input, Output>(begin, outBegin);

            if (*begin == 2)
                return DecodeResult<Input, Output>(begin, outBegin);

            *outBegin = *begin + 3;
            return DecodeResult<Input, Output>(++begin, ++outBegin);
        }

        template<typename Input, typename Output>
        inline static EncodeResult<Input, Output> EncodeSingle(
            Input begin, Input end,
            Output outBegin)
        {
            if (begin == end)
                return EncodeResult<Input, Output>(begin, outBegin);

            if (*begin == 5)
                return DecodeResult<Input, Output>(begin, outBegin);

            *outBegin = *begin - 3;
            return EncodeResult<Input, Output>(++begin, ++outBegin);
        }
    };

    // Decode(Iter, Iter, OutIter)
    TEST(EncodingHelperTest, Decode)
    {
        ForwardTestContainer<Int32, 11> container;
        PushBackContainer<long> output;

        Int32 text[] = { 54, 2, 1, 95, 18, 11, 569, 9, 1, 0, 1123 };
        for (int index = 0; index < 11; ++index)
            container[index] = text[index];

        auto result = Decode<TestEncoding>(
            container.GetBegin(), container.GetEnd(),
            BackInsertIterator<decltype(output)>(output));

        std::vector<long> expected = { 55, 3, 2, 96, 19, 12, 570, 10, 2, 1, 1124 };
        for (int index = 0; index < 11; ++index)
            EXPECT_EQ(output.GetVector()[index], expected[index]);

        EXPECT_EQ(result.InputPosition, container.GetEnd());
        /* Can't check OutputPosition, it's a BackInsertIterator */
    }

    // Encode(Iter, Iter, OutIter)
    TEST(EncodingHelperTest, Encode)
    {
        ForwardTestContainer<long, 11> container;
        PushBackContainer<Int32> output;

        long text[] = { 54, 2, 1, 95, 18, 11, 569, 9, 1, 0, 1123 };
        for (int index = 0; index < 11; ++index)
            container[index] = text[index];

        auto result = Encode<TestEncoding>(
            container.GetBegin(), container.GetEnd(),
            BackInsertIterator<decltype(output)>(output));

        std::vector<Int32> expected = { 53, 1, 0, 94, 17, 10, 568, 8, 0, -1, 1122 };
        for (int index = 0; index < 11; ++index)
            EXPECT_EQ(output.GetVector()[index], expected[index]);

        EXPECT_EQ(result.InputPosition, container.GetEnd());
        /* Can't check OutputPosition, it's a BackInsertIterator */
    }

    // Transcode(Iter, Iter, OutIter)
    TEST(EncodingHelperTest, Transcode)
    {
        ForwardTestContainer<Int32, 11> container;
        PushBackContainer<Int8> output;

        Int32 text[] = { 54, 2, 1, 95, 18, 11, 59, 9, 1, 0, 123 };
        for (int index = 0; index < 11; ++index)
            container[index] = text[index];

        // Int32 -> Int8
        auto result = Transcode<TestEncoding, TestEncoding2>(
            container.GetBegin(), container.GetEnd(),
            BackInsertIterator<decltype(output)>(output));

        std::vector<Int8> expected = { 50, -2, -3, 91, 14, 7, 55, 5, -3, -4, 119 };
        for (int index = 0; index < 11; ++index)
            EXPECT_EQ(output.GetVector()[index], expected[index]);

        EXPECT_EQ(result.InputPosition, container.GetEnd());
        /* Can't check OutputPosition, it's a BackInsertIterator */
    }

    // FindInvalidEncoding(Iter, Iter)
    TEST(EncodingHelperTest, FindInvalidEncodingWithValid)
    {
        ForwardTestContainer<Int32, 11> container;
        Int32 text[] = { 54, 3, 1, 95, 18, 11, 59, 9, 1, 0, 123 };

        for (int index = 0; index < 11; ++index)
            container[index] = text[index];

        EXPECT_EQ(
            FindInvalidEncoding<TestEncodingValid>(
                container.GetBegin(), container.GetEnd()),
            container.GetEnd());
    }

    // FindInvalidEncoding(Iter, Iter)
    TEST(EncodingHelperTest, FindInvalidEncodingWithInvalid)
    {
        ForwardTestContainer<Int32, 11> container;
        Int32 text[] = { 54, 2, 1, 95, 18, 11, 59, 9, 1, 0, 123 };

        for (int index = 0; index < 11; ++index)
            container[index] = text[index];

        auto iterator = container.GetBegin();
        ++iterator;

        EXPECT_EQ(
            FindInvalidEncoding<TestEncodingValid>(
                container.GetBegin(), container.GetEnd()),
            iterator);
    }
}
