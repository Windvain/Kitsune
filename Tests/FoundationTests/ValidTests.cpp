#include "Foundation/String/Valid.h"

#include <gtest/gtest.h>
#include "TestContainer.h"

#include "Foundation/Algorithms/Copy.h"
#include "Foundation/String/StringView.h"

#include "Foundation/String/Utf8Encoding.h"
#include "Foundation/String/Utf16Encoding.h"
#include "Foundation/String/Utf32Encoding.h"

using namespace Kitsune;
using namespace Kitsune::Testing;

template<typename T>
class ValidStringTests : public ::testing::Test
{
public:
    using EncodingType = T;
    using CodeunitType = typename EncodingType::CodeunitType;

protected:
    ValidStringTests() { /* ... */ }
    ~ValidStringTests() { /* ... */ }

protected:
    BasicStringView<CodeunitType> GetValidText() const
    {
        if constexpr (std::is_same_v<CodeunitType, char>)
            return "🦐💩ÿ";
        else if constexpr (std::is_same_v<CodeunitType, char8_t>)
            return u8"🦐💩ÿ";
        else if constexpr (std::is_same_v<CodeunitType, char16_t>)
            return u"🦐💩🐼🐾👾";
        else if constexpr (std::is_same_v<CodeunitType, char32_t>)
            return U"🦐💩🐼🐾👾🐼🐾🤬🦐🙈";
    }

    BasicStringView<CodeunitType> GetInvalidText() const
    {
        if constexpr (std::is_same_v<CodeunitType, char>)
            return "🦐💩\x82";
        else if constexpr (std::is_same_v<CodeunitType, char8_t>)
            return reinterpret_cast<const char8_t*>("🦐💩\x82");
        else if constexpr (std::is_same_v<CodeunitType, char16_t>)
            return u"🦐💩ÿ\xD872🐾👾";
        else if constexpr (std::is_same_v<CodeunitType, char32_t>)
            return U"🦐💩🐼🐾👾🐼\xD800\xDB02🤬🦐🙈";
    }
};

using ValidStringTestsImpl =
    ::testing::Types<
        Utf8Encoding<char>,
        Utf8Encoding<char8_t>,
        Utf16Encoding<char16_t>,
        Utf32Encoding<char32_t>>;

TYPED_TEST_SUITE(ValidStringTests, ValidStringTestsImpl);

TYPED_TEST(ValidStringTests, Valid)
{
    using Encoding = typename TestFixture::EncodingType;
    using Codeunit = typename TestFixture::CodeunitType;

    ForwardTestContainer<Codeunit, 11> container;
    auto validText = this->GetValidText();

    Algorithms::Copy(validText.GetBegin(), validText.GetEnd(), container.GetBegin());
    EXPECT_TRUE(IsValidEncoding<Encoding>(container.GetBegin(), container.GetEnd()));
}

TYPED_TEST(ValidStringTests, Invalid)
{
    using Encoding = typename TestFixture::EncodingType;
    using Codeunit = typename TestFixture::CodeunitType;

    ForwardTestContainer<Codeunit, 11> container;
    auto validText = this->GetInvalidText();

    Algorithms::Copy(validText.GetBegin(), validText.GetEnd(), container.GetBegin());

    EXPECT_FALSE(IsValidEncoding<Encoding>(container.GetBegin(), container.GetEnd()));
}
