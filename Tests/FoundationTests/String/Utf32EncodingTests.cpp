#include "Foundation/String/Utf32Encoding.h"
#include <gtest/gtest.h>

#include "Foundation/Containers/Array.h"
#include "Foundation/String/StringView.h"

#include "Foundation/Iterators/BackInsertIterator.h"

using namespace Kitsune;

template<typename T>
class Utf32EncodingTests : public ::testing::Test
{
public:
    using CharType = T;
    using EncodingType = Utf32Encoding<T>;

    using CodepointType = typename EncodingType::CodepointType;

    static_assert(TextEncoding<Utf32Encoding<T>>,
                  "Utf32Encoding<T> does not satisfy the requirements of TextEncoding.");

protected:
    Utf32EncodingTests() { /* ... */ }
    ~Utf32EncodingTests() { /* ... */ }

protected:
    // Terrible naming but ┑(￣Д ￣)┍
    BasicStringView<T> GetSmileyFaceAndRandom()
    {
        if constexpr (std::is_same_v<T, wchar_t>)
            return L"😁🤧🤕👾";
        else /* std:is_same_v<T, char16_t> */
            return U"😁🤧🤕👾";
    }

    BasicStringView<T> GetInvalidString()
    {
        if constexpr (std::is_same_v<T, wchar_t>)
            return L"\xD800🦐💩";
        else /* std:is_same_v<T, char16_t> */
            return U"\xD87F🦐💩";
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
        return { 0xD800, 0x2022, 0x2022 };
    }
};

using Utf32EncodingTestsImpl =
    ::testing::Types<
        char32_t

        // Some platforms define wchar_t as having a size of 2 bytes.
        // For example, Windows.
#if !defined(KITSUNE_OS_WINDOWS)
        , wchar_t
#endif
    >;

TYPED_TEST_SUITE(Utf32EncodingTests, Utf32EncodingTestsImpl);

TYPED_TEST(Utf32EncodingTests, MaxCodepointValue)
{
    using Encoding = typename TestFixture::EncodingType;
    EXPECT_EQ(Encoding::MaxCodepointValue(), 0x10FFFF);
}

TYPED_TEST(Utf32EncodingTests, DecodeSingleValid)
{
    using Encoding = typename TestFixture::EncodingType;
    auto string = this->GetSmileyFaceAndRandom();

    typename Encoding::CodepointType codepoint;
    auto result = Encoding::DecodeSingle(string.GetBegin(), string.GetEnd(), &codepoint);

    EXPECT_EQ(result.InputPosition, string.GetBegin() + 1);
    EXPECT_EQ(result.OutputPosition, &codepoint + 1);

    EXPECT_EQ(codepoint, 0x1F601);
}

TYPED_TEST(Utf32EncodingTests, DecodeSingleInvalid)
{
    using Encoding = typename TestFixture::EncodingType;
    auto string = this->GetInvalidString();

    typename Encoding::CodepointType codepoint = 0;
    auto result = Encoding::DecodeSingle(string.GetBegin(), string.GetEnd(), &codepoint);

    EXPECT_EQ(result.InputPosition, string.GetBegin());
    EXPECT_EQ(result.OutputPosition, &codepoint);

    EXPECT_EQ(codepoint, 0);
}

TYPED_TEST(Utf32EncodingTests, EncodeSingleValid)
{
    using Encoding = typename TestFixture::EncodingType;
    using T = typename TestFixture::CharType;

    auto codepoints = this->GetSmileyFaceAndRandomCodepoints();

    Array<T> string;
    auto result = Encoding::EncodeSingle(codepoints.GetBegin(), codepoints.GetEnd(),
                                         BackInsertIterator<decltype(string)>(string));

    EXPECT_EQ(result.InputPosition, codepoints.GetBegin() + 1);

    EXPECT_EQ(string.Size(), 1);
    EXPECT_EQ(string[0], T(0x1F601));
}

TYPED_TEST(Utf32EncodingTests, EncodeSingleInvalid)
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
