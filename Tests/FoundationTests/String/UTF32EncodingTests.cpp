#include <gtest/gtest.h>
#include "TestContainer.h"

#include "Foundation/String/TextEncoding.h"
#include "Foundation/String/UTF32Encoding.h"

namespace
{
    using namespace Kitsune;
    using Testing::ForwardNonOwningTestContainer;

    template<typename T>
    class UTF32EncodingTests : public ::testing::Test
    {
    protected:
        using CharType = T;
        using StringType = std::basic_string<T>;

        using EncodingType = UTF32Encoding<T>;

        static_assert(sizeof(T) == 4, "The size of the character should be equal to 4.");
        static_assert(
            TextEncoding<EncodingType>,
            "UTF32Encoding does not satisfy the requirements of TextEncoding.");

    protected:
        [[nodiscard]]
        inline StringType GetString(const char32_t* string)
        {
            if constexpr (std::is_same_v<T, wchar_t>)
                return StringType(reinterpret_cast<const wchar_t*>(string));
            else
                return string;
        }
    };

    using UTF32EncodingTestsTypes = ::testing::Types<

#if defined(KITSUNE_OS_LINUX)
        wchar_t,        // wchar_t is 2 bytes on Windows.
#endif
        char32_t>;

    TYPED_TEST_SUITE(UTF32EncodingTests, UTF32EncodingTestsTypes);

    TYPED_TEST(UTF32EncodingTests, MaxCodepointValue)
    {
        using Encoding = typename TestFixture::EncodingType;
        EXPECT_EQ(Encoding::MaxCodepointValue(), 0x10FFFF);
    }

    TYPED_TEST(UTF32EncodingTests, MaxCodeunits)
    {
        using Encoding = typename TestFixture::EncodingType;
        EXPECT_EQ(Encoding::MaxCodeunits(), 1);
    }

    TYPED_TEST(UTF32EncodingTests, GetPreamble)
    {
        using Encoding = typename TestFixture::EncodingType;
        auto expected = this->GetString(U"\uFEFF");

        EXPECT_EQ(Encoding::GetPreamble(), expected.c_str());
    }

    TYPED_TEST(UTF32EncodingTests, GetReplacement)
    {
        using Encoding = typename TestFixture::EncodingType;
        auto replacement = this->GetString(U"�");

        EXPECT_EQ(Encoding::GetReplacement(), replacement.c_str());
    }

    TYPED_TEST(UTF32EncodingTests, DecodeValid)
    {
        using T = typename TestFixture::CharType;
        using Encoding = typename TestFixture::EncodingType;

        using Codepoint = typename Encoding::CodepointType;
        std::vector<std::pair<const char32_t*, Codepoint>> cases = {
            { U"Hx", 0x0048 },
            { U"😊c", 0x1F60A }
        };

        for (auto [cstring, value] : cases)
        {
            auto string = this->GetString(cstring);
            ForwardNonOwningTestContainer<T, 13> container(string.data());

            typename Encoding::CodepointType codepoint;
            auto [iter, outIter] = Encoding::Decode(
                container.GetBegin(), container.GetEnd(), &codepoint);

            EXPECT_EQ(codepoint, value);

            EXPECT_EQ(iter.Pointer(), &string[1]);
            EXPECT_EQ(outIter, std::addressof(codepoint) + 1);
        }
    }

    TYPED_TEST(UTF32EncodingTests, DecodeInvalid)
    {
        using T = typename TestFixture::CharType;

        using Encoding = typename TestFixture::EncodingType;
        using Codepoint = typename Encoding::CodepointType;

        std::vector<std::u32string> cases = {
            U"\xD8BF",      // High surrogate.
            U"\xDEFF",      // Low surrogate.
            U"\x110000"     // Higher value than 0x10FFFF.
        };

        for (const std::u32string& invalid : cases)
        {
            std::u32string concat = invalid + U"some random string.";
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

    TYPED_TEST(UTF32EncodingTests, EncodeValid)
    {
        using T = typename TestFixture::CharType;
        using Encoding = typename TestFixture::EncodingType;

        using Codepoint = typename Encoding::CodepointType;

        using Arr = std::array<Codepoint, 2>;
        std::vector<std::pair<Arr, std::basic_string<T>>> cases = {
            { { 0x0048, 0x0D74 }, this->GetString(U"H") },
            { { 0x1F60A, 0xD800 }, this->GetString(U"😊") }
        };

        for (auto& [values, string] : cases)
        {
            ForwardNonOwningTestContainer<Codepoint, 13> container(values.data());
            T codeunits;

            auto [iter, outIter] = Encoding::Encode(
                container.GetBegin(), container.GetEnd(), &codeunits);

            EXPECT_EQ(codeunits, string[0]);

            EXPECT_EQ(iter.Pointer(), std::addressof(values[0]) + 1);
            EXPECT_EQ(outIter, &codeunits + 1);
        }
    }

    TYPED_TEST(UTF32EncodingTests, EncodeInvalid)
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
