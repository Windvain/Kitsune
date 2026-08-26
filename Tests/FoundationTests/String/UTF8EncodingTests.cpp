#include <gtest/gtest.h>
#include "TestContainer.h"

#include "Foundation/String/TextEncoding.h"
#include "Foundation/String/UTF8Encoding.h"

#if 0
namespace
{
    using namespace Kitsune;
    using Testing::ForwardNonOwningTestContainer;

    template<typename T>
    class UTF8EncodingTests : public ::testing::Test
    {
    protected:
        using CharType = T;
        using StringType = std::basic_string<T>;

        using EncodingType = UTF8Encoding<T>;

        static_assert(sizeof(T) == 1, "The size of the character should be equal to 1.");
        static_assert(
            TextEncoding<EncodingType>,
            "UTF8Encoding does not satisfy the requirements of TextEncoding.");

    protected:
        [[nodiscard]]
        inline StringType GetString(const char* string)
        {
            if constexpr (std::is_same_v<T, char8_t>)
                return StringType(reinterpret_cast<const char8_t*>(string));
            else
                return string;
        }
    };

    using UTF8EncodingTestsTypes = ::testing::Types<char, char8_t>;
    TYPED_TEST_SUITE(UTF8EncodingTests, UTF8EncodingTestsTypes);

    TYPED_TEST(UTF8EncodingTests, MaxCodepointValue)
    {
        using Encoding = typename TestFixture::EncodingType;
        EXPECT_EQ(Encoding::MaxCodepointValue(), 0x10FFFF);
    }

    TYPED_TEST(UTF8EncodingTests, MaxCodeunits)
    {
        using Encoding = typename TestFixture::EncodingType;
        EXPECT_EQ(Encoding::MaxCodeunits(), 4);
    }

    TYPED_TEST(UTF8EncodingTests, GetPreamble)
    {
        using Encoding = typename TestFixture::EncodingType;
        EXPECT_TRUE(Encoding::GetPreamble().IsEmpty());
    }

    TYPED_TEST(UTF8EncodingTests, GetReplacement)
    {
        using Encoding = typename TestFixture::EncodingType;
        auto replacement = this->GetString("�");

        EXPECT_EQ(Encoding::GetReplacement(), replacement.c_str());
    }

    TYPED_TEST(UTF8EncodingTests, DecodeValid)
    {
        using T = typename TestFixture::CharType;
        using Encoding = typename TestFixture::EncodingType;

        using Codepoint = typename Encoding::CodepointType;

        std::vector<std::tuple<const char*, int, Codepoint>> cases = {
            { "Hx", 1, 0x0048 },
            { "£0", 2, 0x00A3 },
            { "൴f", 3, 0x0D74 },
            { "😊c", 4, 0x1F60A }
        };

        for (auto [cstring, units, value] : cases)
        {
            auto string = this->GetString(cstring);
            ForwardNonOwningTestContainer<T, 13> container(string.data());

            typename Encoding::CodepointType codepoint;
            auto [iter, outIter] = Encoding::Decode(
                container.GetBegin(), container.GetEnd(), &codepoint);

            EXPECT_EQ(codepoint, value);

            EXPECT_EQ(iter.Pointer(), &string[units]);
            EXPECT_EQ(outIter, std::addressof(codepoint) + 1);
        }
    }

    TYPED_TEST(UTF8EncodingTests, DecodeContinuousByteAsHeader)
    {
        using T = typename TestFixture::CharType;
        using Encoding = typename TestFixture::EncodingType;

        for (char start = '\x80'; start <= '\xBF'; ++start)
        {
            std::string concat = std::string(&start) + "Flash";
            auto string = this->GetString(concat.c_str());

            ForwardNonOwningTestContainer<T, 2> container(string.data());
            typename Encoding::CodepointType codepoint = 23;

            auto [iter, outIter] = Encoding::Decode(
                container.GetBegin(), container.GetEnd(), &codepoint);

            EXPECT_EQ(codepoint, 23);

            EXPECT_EQ(iter.Pointer(), string.c_str());
            EXPECT_EQ(outIter, std::addressof(codepoint));
        }
    }

    TYPED_TEST(UTF8EncodingTests, DecodeRestInvalid)
    {
        using T = typename TestFixture::CharType;

        using Encoding = typename TestFixture::EncodingType;
        using Codepoint = typename Encoding::CodepointType;

        std::vector<std::string> cases = {
            // Non-continuous bytes. (the string ending before the character
            // is fully decoded)
            "\xD0",
            "\xC0\xD0",
            "\xE6",
            "\xE4\xC2",
            "\xE5\x86",
            "\xE5\x86\xC2",
            "\xF4",
            "\xF4\xF3",
            "\xF3\x85",
            "\xF3\x85\xD3",
            "\xF3\x81\x81",
            "\xF3\x81\x81\xE2",

            // Overlong encodings. (Encoding a codepoint with more bytes than necessary)
            "\xC0\x9F",
            "\xC1\x9F",
            "\xE0\x9F",
            "\xF0\x8F",

            // Codepoints larger than U+10FFFF.
            "\xF4\x90",
            "\xF5\x81\x81\x81",
            "\xFF\x81\x86\x86"
        };

        for (const std::string& invalid : cases)
        {
            std::string concat = invalid + "some random string.";
            auto string = this->GetString(concat.c_str());

            ForwardNonOwningTestContainer<T, 8> container(string.data());
            Codepoint codepoint = 3;

            auto [iter, outIter] = Encoding::Decode(
                container.GetBegin(), container.GetEnd(), &codepoint);

            EXPECT_EQ(codepoint, 3);

            EXPECT_EQ(iter.Pointer(), &string[0]);
            EXPECT_EQ(outIter, &codepoint);
        }
    }

    TYPED_TEST(UTF8EncodingTests, EncodeValid)
    {
        using T = typename TestFixture::CharType;
        using Encoding = typename TestFixture::EncodingType;

        using Codepoint = typename Encoding::CodepointType;

        using Arr = std::array<Codepoint, 2>;
        std::vector<std::tuple<Arr, int, std::basic_string<T>>> cases = {
            { { 0x0048, 0x0D74 }, 1, this->GetString("H") },
            { { 0x00A3, 0x0011 }, 2, this->GetString("£") },
            { { 0x0D74, 0xD87F }, 3, this->GetString("൴") },
            { { 0x1F60A, 0xD800 }, 4, this->GetString("😊") }
        };

        for (auto& [values, units, string] : cases)
        {
            ForwardNonOwningTestContainer<Codepoint, 13> container(values.data());
            T codeunits[4];

            auto [iter, outIter] = Encoding::Encode(
                container.GetBegin(), container.GetEnd(), &codeunits[0]);

            for (Usize index = 0; index < units; ++index)
                EXPECT_EQ(codeunits[index], string[index]);

            EXPECT_EQ(iter.Pointer(), std::addressof(values[0]) + 1);
            EXPECT_EQ(outIter, &codeunits[0] + units);
        }
    }

    TYPED_TEST(UTF8EncodingTests, EncodeInvalid)
    {
        using T = typename TestFixture::CharType;
        using Encoding = typename TestFixture::EncodingType;

        using Codepoint = typename Encoding::CodepointType;
        std::vector<std::array<Codepoint, 2>> cases = {
            { { 0xD800, 0x0D74 } },
            { { 0xDC9A, 0x0011 } },
            { { 0xDFFF, 0xD87F } },
            { { 0x110000, 0xD800 } }
        };

        for (auto& array : cases)
        {
            ForwardNonOwningTestContainer<Codepoint, 13> container(array.data());
            T codeunits;

            auto [iter, outIter] = Encoding::Encode(
                container.GetBegin(), container.GetEnd(), &codeunits);

            EXPECT_EQ(iter, container.GetBegin());
            EXPECT_EQ(outIter, &codeunits);
        }
    }
}
#endif
