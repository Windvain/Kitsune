#include "Foundation/String/Encode.h"

#include <gtest/gtest.h>
#include "TestStrings.h"
#include "TestContainer.h"

#include "Foundation/Algorithms/Copy.h"
#include "Foundation/Containers/Array.h"
#include "Foundation/Iterators/BackInsertIterator.h"

#include "Foundation/String/Utf8Encoding.h"
#include "Foundation/String/Utf16Encoding.h"
#include "Foundation/String/Utf32Encoding.h"

using namespace Kitsune;
using namespace Kitsune::Testing;

template<typename T>
class EncodeStringTests : public ::testing::Test
{
public:
    using EncodingType = T;

    using CodeunitType = typename EncodingType::CodeunitType;
    using CodepointType = typename EncodingType::CodepointType;

protected:
    EncodeStringTests() { /* ... */ }
    ~EncodeStringTests() { /* ... */ }

protected:
    const CodeunitType* GetExpectedOutput() const
    {
        if constexpr (std::is_same_v<CodeunitType, char>)
            return "🦐💩🐼🐾👾🐼🐾🤬🦐🙈";
        else if constexpr (std::is_same_v<CodeunitType, char8_t>)
            return u8"🦐💩🐼🐾👾🐼🐾🤬🦐🙈";
        else if constexpr (std::is_same_v<CodeunitType, char16_t>)
            return u"🦐💩🐼🐾👾🐼🐾🤬🦐🙈";
        else if constexpr (std::is_same_v<CodeunitType, char32_t>)
            return U"🦐💩🐼🐾👾🐼🐾🤬🦐🙈";
    }

    const CodepointType* GetTextToEncode() const
    {
        return reinterpret_cast<const CodepointType*>(U"🦐💩🐼🐾👾🐼🐾🤬🦐🙈");
    }
};

using EncodeStringTestsImpl =
    ::testing::Types<
        Utf8Encoding<char>,
        Utf8Encoding<char8_t>,
        Utf16Encoding<char16_t>,
        Utf32Encoding<char32_t>>;

TYPED_TEST_SUITE(EncodeStringTests, EncodeStringTestsImpl);

TYPED_TEST(EncodeStringTests, Encode)
{
    using Encoding = typename TestFixture::EncodingType;

    using Codeunit = typename TestFixture::CodeunitType;
    using Codepoint = typename TestFixture::CodepointType;

    ForwardTestContainer<Codepoint, 11> container;
    Array<Codeunit> output;

    const auto* text = this->GetTextToEncode();
    Algorithms::Copy(text, text + 11, container.GetBegin());

    auto result = Encode<Encoding>(
        container.GetBegin(), container.GetEnd(),
        BackInsertIterator<decltype(output)>(output));

    EXPECT_GENERAL_STREQ(output.Data(), this->GetExpectedOutput());
    EXPECT_EQ(result.InputPosition, container.GetEnd());

    /* Can't check OutputPosition, it's a BackInsertIterator */
}
