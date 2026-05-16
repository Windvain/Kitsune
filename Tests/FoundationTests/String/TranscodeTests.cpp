#include "Foundation/String/Transcode.h"

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
class TranscodeStringTests : public ::testing::Test
{
public:
    using FromEncoding = typename T::first_type;
    using ToEncoding = typename T::second_type;

    using FromCodeunit = typename FromEncoding::CodeunitType;
    using ToCodeunit = typename ToEncoding::CodeunitType;

protected:
    TranscodeStringTests() { /* ... */ }
    ~TranscodeStringTests() { /* ... */ }

protected:
    const FromCodeunit* GetTextToTranscode() const
    {
        if constexpr (std::is_same_v<FromCodeunit, char>)
            return "🦐💩ÿ";
        else if constexpr (std::is_same_v<FromCodeunit, char8_t>)
            return u8"🦐💩ÿ";
        else if constexpr (std::is_same_v<FromCodeunit, char16_t>)
            return u"🦐💩🐼🐾👾";
        else if constexpr (std::is_same_v<FromCodeunit, char32_t>)
            return U"🦐💩🐼🐾👾🐼🐾🤬🦐🙈";
    }

    const ToCodeunit* GetExpectedOutput() const
    {
        // Couldn't be arsed to think of a better solution, headache.
        if constexpr (std::is_same_v<FromCodeunit, char> ||
                      std::is_same_v<FromCodeunit, char8_t>)
        {
            if constexpr (std::is_same_v<ToCodeunit, char16_t>)
                return u"🦐💩ÿ";
            else /* ToCodeunit == char32_t */
                return U"🦐💩ÿ";
        }
        else if constexpr (std::is_same_v<FromCodeunit, char16_t>)
        {
            if constexpr (std::is_same_v<ToCodeunit, char>)
                return "🦐💩🐼🐾👾";
            else if constexpr (std::is_same_v<ToCodeunit, char8_t>)
                return u8"🦐💩🐼🐾👾";
            else if constexpr (std::is_same_v<ToCodeunit, char32_t>)
                return U"🦐💩🐼🐾👾";
        }
        else if constexpr (std::is_same_v<FromCodeunit, char32_t>)
        {
            if constexpr (std::is_same_v<ToCodeunit, char>)
                return "🦐💩🐼🐾👾🐼🐾🤬🦐🙈";
            else if constexpr (std::is_same_v<ToCodeunit, char8_t>)
                return u8"🦐💩🐼🐾👾🐼🐾🤬🦐🙈";
            else if constexpr (std::is_same_v<ToCodeunit, char16_t>)
                return u"🦐💩🐼🐾👾🐼🐾🤬🦐🙈";
        }
    }
};

using TranscodeStringTestsImpl =
    ::testing::Types<
        std::pair<Utf8Encoding<char>, Utf16Encoding<char16_t>>,
        std::pair<Utf8Encoding<char8_t>, Utf32Encoding<char32_t>>,
        std::pair<Utf16Encoding<char16_t>, Utf8Encoding<char8_t>>,
        std::pair<Utf16Encoding<char16_t>, Utf32Encoding<char32_t>>,
        std::pair<Utf32Encoding<char32_t>, Utf8Encoding<char>>,
        std::pair<Utf32Encoding<char32_t>, Utf16Encoding<char16_t>>>;

TYPED_TEST_SUITE(TranscodeStringTests, TranscodeStringTestsImpl);

TYPED_TEST(TranscodeStringTests, Transcode)
{
    using FromEncoding = typename TestFixture::FromEncoding;
    using ToEncoding = typename TestFixture::ToEncoding;

    using FromCodeunit = typename TestFixture::FromCodeunit;
    using ToCodeunit = typename TestFixture::ToCodeunit;

    ForwardTestContainer<FromCodeunit, 11> container;
    Array<ToCodeunit> output;

    const auto* text = this->GetTextToTranscode();
    Algorithms::Copy(text, text + 11, container.GetBegin());

    auto result = Transcode<FromEncoding, ToEncoding>(
        container.GetBegin(), container.GetEnd(),
        BackInsertIterator<decltype(output)>(output));

    EXPECT_GENERAL_STREQ(output.Data(), this->GetExpectedOutput());
    EXPECT_EQ(result.InputPosition, container.GetEnd());

    /* Can't check OutputPosition, it's a BackInsertIterator */
}
