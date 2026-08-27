#include <gtest/gtest.h>
#include "TestContainer.h"

#include "Foundation/String/TextEncoding.h"
#include "Foundation/String/UTF16Encoding.h"

namespace
{
    using namespace Kitsune;
    using Testing::ForwardNonOwningTestContainer;

    template<typename T>
    class UTF16EncodingTests : public ::testing::Test
    {
    protected:
        using CharType = T;
        using StringType = std::basic_string<T>;

        using EncodingType = UTF16Encoding<T>;

        static_assert(sizeof(T) == 2, "The size of the character should be equal to 2.");
        static_assert(
            TextEncoding<EncodingType>,
            "UTF16Encoding does not satisfy the requirements of TextEncoding.");

    protected:
        [[nodiscard]]
        inline StringType GetString(const char16_t* string)
        {
            if constexpr (std::is_same_v<T, wchar_t>)
                return StringType(reinterpret_cast<const wchar_t*>(string));
            else
                return string;
        }
    };

    using UTF16EncodingTestsTypes = ::testing::Types<

#if defined(KITSUNE_OS_WINDOWS)
        wchar_t,        // wchar_t is 2 bytes on Windows.
#endif
        char16_t>;

    TYPED_TEST_SUITE(UTF16EncodingTests, UTF16EncodingTestsTypes);

    TYPED_TEST(UTF16EncodingTests, MaxCodepointValue)
    {
        using Encoding = typename TestFixture::EncodingType;
        EXPECT_EQ(Encoding::MaxCodepointValue(), 0x10FFFF);
    }

    TYPED_TEST(UTF16EncodingTests, MaxCodeunits)
    {
        using Encoding = typename TestFixture::EncodingType;
        EXPECT_EQ(Encoding::MaxCodeunits(), 2);
    }

    TYPED_TEST(UTF16EncodingTests, GetPreamble)
    {
        using Encoding = typename TestFixture::EncodingType;
        auto expected = this->GetString(u"\uFEFF");

        EXPECT_EQ(Encoding::GetPreamble(), expected.c_str());
    }

    TYPED_TEST(UTF16EncodingTests, GetReplacement)
    {
        using Encoding = typename TestFixture::EncodingType;
        auto replacement = this->GetString(u"�");

        EXPECT_EQ(Encoding::GetReplacement(), replacement.c_str());
    }

    TYPED_TEST(UTF16EncodingTests, DecodeValid)
    {
        using T = typename TestFixture::CharType;
        using Encoding = typename TestFixture::EncodingType;

        using Codepoint = typename Encoding::CodepointType;

        std::vector<std::tuple<const char16_t*, int, Codepoint>> cases = {
            { u"Hx", 1, 0x0048 },
            { u"😊c", 2, 0x1F60A }
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

    TYPED_TEST(UTF16EncodingTests, DecodeInvalid)
    {
        using T = typename TestFixture::CharType;

        using Encoding = typename TestFixture::EncodingType;
        using Codepoint = typename Encoding::CodepointType;

        std::vector<std::u16string> cases = {
            // High surrogate w/o being followed by a low surrogate.
            u"\xD8BF",
            u"\xD8CF\xD9CC",
            u"\xD8DF\x0D74",

            // Hanging low surrogate.
            u"\xDEFF",
        };

        for (const std::u16string& invalid : cases)
        {
            std::u16string concat = invalid + u"some random string.";
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

    TYPED_TEST(UTF16EncodingTests, EncodeValid)
    {
        using T = typename TestFixture::CharType;
        using Encoding = typename TestFixture::EncodingType;

        using Codepoint = typename Encoding::CodepointType;

        struct MyStruct
        {
            std::vector<Codepoint> Codepoints;
            int UnitCount;
            std::basic_string<T> String;
        };

        std::vector<MyStruct> cases = {
            { { 0x0048, 0x0D74 }, 1, this->GetString(u"H") },
            { { 0x1F60A, 0xD800 }, 2, this->GetString(u"😊") }
        };

        for (auto& [values, units, string] : cases)
        {
            ForwardNonOwningTestContainer<Codepoint, 13> container(values.data());
            T codeunits[2];

            auto [iter, outIter] = Encoding::Encode(
                container.GetBegin(), container.GetEnd(), &codeunits[0]);

            for (Usize index = 0; index < units; ++index)
                EXPECT_EQ(codeunits[index], string[index]);

            EXPECT_EQ(iter.Pointer(), std::addressof(values[0]) + 1);
            EXPECT_EQ(outIter, &codeunits[0] + units);
        }
    }

    TYPED_TEST(UTF16EncodingTests, EncodeInvalid)
    {
        using T = typename TestFixture::CharType;
        using Encoding = typename TestFixture::EncodingType;

        using Codepoint = typename Encoding::CodepointType;
        std::vector<std::vector<Codepoint>> cases = {
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
