#include <gtest/gtest.h>
#include "TestContainer.h"

#include "Foundation/String/Decode.h"
#include "Foundation/String/Encode.h"
#include "Foundation/String/Transcode.h"

#include "Foundation/String/TextEncoding.h"

namespace
{
    using namespace Kitsune;
    using Testing::ForwardNonOwningTestContainer;

    class AddOneEncoding
    {
    public:
        using CodepointType = Uint32;
        using CodeunitType = char16_t;

    public:
        [[nodiscard]]
        inline static CodepointType MaxCodepointValue()
        {
            return 2;
        }

        [[nodiscard]]
        inline static Usize MaxCodeunits()
        {
            return 2;
        }

        [[nodiscard]]
        inline static U16StringView GetPreamble()
        {
            return u"";
        }

        [[nodiscard]]
        inline static U16StringView GetReplacement()
        {
            return u"";
        }

    public:
        template<typename Input, typename Output>
        inline static Pair<Input, Output> Decode(
            Input begin, Input end,
            Output outBegin)
        {
            if (begin == end)
                return Pair<Input, Output>(begin, outBegin);

            *outBegin = *begin + 1;
            return Pair<Input, Output>(++begin, ++outBegin);
        }

        template<typename Input, typename Output>
        inline static Pair<Input, Output> Encode(
            Input begin, Input end,
            Output outBegin)
        {
            if (begin == end)
                return Pair<Input, Output>(begin, outBegin);

            *outBegin = *begin - 1;
            return Pair<Input, Output>(++begin, ++outBegin);
        }
    };

    class AddFiveEncoding
    {
    public:
        using CodepointType = Uint32;
        using CodeunitType = char32_t;

    public:
        [[nodiscard]]
        inline static CodepointType MaxCodepointValue()
        {
            return 2;
        }

        [[nodiscard]]
        inline static Usize MaxCodeunits()
        {
            return 2;
        }

        [[nodiscard]]
        inline static U32StringView GetPreamble()
        {
            return U"";
        }

        [[nodiscard]]
        inline static U32StringView GetReplacement()
        {
            return U"";
        }

    public:
        template<typename Input, typename Output>
        inline static Pair<Input, Output> Decode(
            Input begin, Input end,
            Output outBegin)
        {
            if (begin == end)
                return Pair<Input, Output>(begin, outBegin);

            *outBegin = *begin + 5;
            return Pair<Input, Output>(++begin, ++outBegin);
        }

        template<typename Input, typename Output>
        inline static Pair<Input, Output> Encode(
            Input begin, Input end,
            Output outBegin)
        {
            if (begin == end)
                return Pair<Input, Output>(begin, outBegin);

            *outBegin = *begin - 5;
            return Pair<Input, Output>(++begin, ++outBegin);
        }
    };

    static_assert(
        TextEncoding<AddOneEncoding>,
        "AddOneEncoding does not satisfy the requirements of a text encoding.");

    static_assert(
        TextEncoding<AddFiveEncoding>,
        "AddFiveEncoding does not satisfy the requirements of a text encoding.");

    // Decode(Iter, Iter, OutIter)
    TEST(EncodingHelperTest, Decode)
    {
        std::vector<Uint32> text = { 54, 2, 1, 95, 18, 11, 569, 9, 1, 0, 1123 };
        ForwardNonOwningTestContainer<Uint32, 11> container(text.data());

        char16_t output[11];
        auto [iter, outIter] = Decode<AddOneEncoding>(
            container.GetBegin(), container.GetEnd(),
            &output[0]);

        std::vector<char16_t> expected = { 55, 3, 2, 96, 19, 12, 570, 10, 2, 1, 1124 };
        for (int index = 0; index < 11; ++index)
            EXPECT_EQ(output[index], expected[index]);

        EXPECT_EQ(iter, container.GetEnd());
        EXPECT_EQ(outIter, std::addressof(output[0]) + 11);
    }

    // Encode(Iter, Iter, OutIter)
    TEST(EncodingHelperTest, Encode)
    {
        std::vector<char16_t> text = { 54, 2, 1, 95, 18, 11, 569, 9, 1, 7, 1123 };
        ForwardNonOwningTestContainer<char16_t, 11> container(text.data());

        char16_t output[11];
        auto [iter, outIter] = Encode<AddOneEncoding>(
            container.GetBegin(), container.GetEnd(),
            &output[0]);

        std::vector<char16_t> expected = { 53, 1, 0, 94, 17, 10, 568, 8, 0, 6, 1122 };
        for (int index = 0; index < 11; ++index)
            EXPECT_EQ(output[index], expected[index]);

        EXPECT_EQ(iter, container.GetEnd());
        EXPECT_EQ(outIter, std::addressof(output[0]) + 11);
    }

    // Transcode(Iter, Iter, OutIter)
    TEST(EncodingHelperTest, Transcode)
    {
        std::vector<char16_t> text = { 54, 9, 7, 95, 18, 11, 59, 9, 7, 17, 123 };
        ForwardNonOwningTestContainer<char16_t, 11> container(text.data());

        char32_t output[11];
        auto [iter, outIter] = Transcode<AddOneEncoding, AddFiveEncoding>(
            container.GetBegin(), container.GetEnd(),
            &output[0]);

        std::vector<char32_t> expected = { 50, 5, 3, 91, 14, 7, 55, 5, 3, 13, 119 };
        for (int index = 0; index < 11; ++index)
            EXPECT_EQ(output[index], expected[index]);

        EXPECT_EQ(iter, container.GetEnd());
        EXPECT_EQ(outIter, std::addressof(output[0]) + 11);
    }
}
