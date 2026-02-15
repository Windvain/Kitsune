#include "Foundation/String/Utf8Encoding.h"
#include <gtest/gtest.h>

#include "Foundation/Containers/Array.h"
#include "Foundation/String/StringView.h"

#include "Foundation/Iterators/BackInsertIterator.h"

using namespace Kitsune;

template<typename T>
class Utf8EncodingTests : public ::testing::Test
{
public:
    using CharType = T;
    using EncodingType = Utf8Encoding<T>;

    using CodepointType = typename EncodingType::CodepointType;

    static_assert(TextEncoding<Utf8Encoding<T>>,
                  "Utf8Encoding<T> does not satisfy the requirements of TextEncoding.");

protected:
    Utf8EncodingTests() { /* ... */ }
    ~Utf8EncodingTests() { /* ... */ }

protected:
    // Terrible naming but ┑(￣Д ￣)┍
    BasicStringView<T> GetSmileyFaceAndRandom()
    {
        if constexpr (std::is_same_v<T, char>)
            return "😁🤧🤕👾";
        else /* std:is_same_v<T, char8_t> */
            return u8"😁🤧🤕👾";
    }

    BasicStringView<T> GetInvalidString()
    {
        if constexpr (std::is_same_v<T, char>)
            return "\xD8\x00\xDB\x7F🦐💩";
        else /* std:is_same_v<T, char8_t> */
            return u8"\xD8\x00\xDB\x7F🦐💩";
    }

    Array<CodepointType> GetSmileyFaceAndRandomCodepoints()
    {
        BasicStringView<char32_t> string(U"😁🤧🤕👾");
        Array<CodepointType> array;

        for (char32_t character : string)
            array.PushBack(static_cast<CodepointType>(character));

        return array;
    }

    Array<CodepointType> GetInvalidCodepoints()
    {
        return { 0xD800, 0xDB7F, 0x2022, 0x2022 };
    }
};

using Utf8EncodingTestsImpl =
    ::testing::Types<
        char,
        char8_t>;

TYPED_TEST_SUITE(Utf8EncodingTests, Utf8EncodingTestsImpl);

TYPED_TEST(Utf8EncodingTests, MaxCodepointValue)
{
    using Encoding = typename TestFixture::EncodingType;
    EXPECT_EQ(Encoding::MaxCodepointValue(), 0x10FFFF);
}

TYPED_TEST(Utf8EncodingTests, DecodeSingleValid)
{
    using Encoding = typename TestFixture::EncodingType;
    auto string = this->GetSmileyFaceAndRandom();

    typename Encoding::CodepointType codepoint;
    auto result = Encoding::DecodeSingle(string.GetBegin(), string.GetEnd(), &codepoint);

    EXPECT_EQ(result.InputPosition, string.GetBegin() + 4);
    EXPECT_EQ(result.OutputPosition, &codepoint + 1);

    EXPECT_EQ(codepoint, 0x1F601);
}

TYPED_TEST(Utf8EncodingTests, DecodeSingleInvalid)
{
    using Encoding = typename TestFixture::EncodingType;
    auto string = this->GetInvalidString();

    typename Encoding::CodepointType codepoint = 0;
    auto result = Encoding::DecodeSingle(string.GetBegin(), string.GetEnd(), &codepoint);

    EXPECT_EQ(result.InputPosition, string.GetBegin());
    EXPECT_EQ(result.OutputPosition, &codepoint);

    EXPECT_EQ(codepoint, 0);
}

TYPED_TEST(Utf8EncodingTests, EncodeSingleValid)
{
    using Encoding = typename TestFixture::EncodingType;
    using T = typename TestFixture::CharType;

    auto codepoints = this->GetSmileyFaceAndRandomCodepoints();

    Array<T> string;
    auto result = Encoding::EncodeSingle(codepoints.GetBegin(), codepoints.GetEnd(),
                                         BackInsertIterator<decltype(string)>(string));

    EXPECT_EQ(result.InputPosition, codepoints.GetBegin() + 1);

    EXPECT_EQ(string.Size(), 4);
    EXPECT_EQ(string[0], T(0xF0));
    EXPECT_EQ(string[1], T(0x9F));
    EXPECT_EQ(string[2], T(0x98));
    EXPECT_EQ(string[3], T(0x81));
}

TYPED_TEST(Utf8EncodingTests, EncodeSingleInvalid)
{
    using Encoding = typename TestFixture::EncodingType;
    using T = typename TestFixture::CharType;

    auto codepoints = this->GetInvalidCodepoints();

    Array<T> string;
    auto result = Encoding::EncodeSingle(codepoints.GetBegin(), codepoints.GetEnd(),
                                         BackInsertIterator<decltype(string)>(string));

    EXPECT_EQ(result.InputPosition, codepoints.GetBegin());
    EXPECT_TRUE(string.IsEmpty());
}
