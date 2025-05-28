#include "CompareStrings.h"
#include "Foundation/String/FromCodepoints.h"

#include "Foundation/String/String.h"
#include "Foundation/Containers/Array.h"
#include "Foundation/Iterators/BackInsertIterator.h"

using namespace Kitsune;

TEST(FromCodepointsTests, CodepointsToUtf8)
{
    String str;
    Array<Unicode::Codepoint> codepoints = { 0x00BB, 0x005F, 0x00FC, 0x00FF, 0x2194, 0x2563, 0x1F923, 0x1F495, 0x1F3B6 };

    Unicode::CodepointsToUtf8(codepoints.GetBegin(), codepoints.GetEnd(), BackInsertIterator<String>(str));
    EXPECT_GENERAL_STREQ(str.Raw(), "»_üÿ↔╣🤣💕🎶");
}

TEST(FromCodepointsTests, CodepointsToUtf8Char8_t)
{
    U8String str;
    Array<Unicode::Codepoint> codepoints = { 0x00BB, 0x005F, 0x00FC, 0x00FF, 0x2194, 0x2563, 0x1F923, 0x1F495, 0x1F3B6 };

    Unicode::CodepointsToUtf8(codepoints.GetBegin(), codepoints.GetEnd(), BackInsertIterator<U8String>(str));
    EXPECT_GENERAL_STREQ(str.Raw(), u8"»_üÿ↔╣🤣💕🎶");
}

TEST(FromCodepointsTests, CodepointsToUtf16)
{
    U16String str;
    Array<Unicode::Codepoint> codepoints = { 0x252C, 0x00FF, 0x00BC, 0x2194, 0x2518 };

    Unicode::CodepointsToUtf16(codepoints.GetBegin(), codepoints.GetEnd(), BackInsertIterator<U16String>(str));
    EXPECT_GENERAL_STREQ(str.Raw(), u"┬ÿ¼↔┘");
}

TEST(FromCodepointsTests, CodepointsToUtf32)
{
    U32String str;
    Array<Unicode::Codepoint> codepoints = { 0x252C, 0x00FF, 0x00BC, 0x2194, 0x2518 };

    Unicode::CodepointsToUtf32(codepoints.GetBegin(), codepoints.GetEnd(), BackInsertIterator<U32String>(str));
    EXPECT_GENERAL_STREQ(str.Raw(), U"┬ÿ¼↔┘");
}

#if defined(_WIN32)
TEST(FromCodepointsTests, CodepointsToUtf16Wchar_t)
{
    static_assert(sizeof(wchar_t) == 2, "sizeof(wchar_t) should be equal to 2.");

    WideString str;
    Array<Unicode::Codepoint> codepoints = { 0x252C, 0x00FF, 0x00BC, 0x2194, 0x2518 };

    Unicode::CodepointsToUtf16(codepoints.GetBegin(), codepoints.GetEnd(), BackInsertIterator<WideString>(str));
    EXPECT_GENERAL_STREQ(str.Raw(), L"┬ÿ¼↔┘");
}
#else
TEST(FromCodepointsTests, CodepointsToUtf32Wchar_t)
{
    static_assert(sizeof(wchar_t) == 4, "sizeof(wchar_t) should be equal to 4.");

    WideString str;
    Array<Unicode::Codepoint> codepoints = { 0x252C, 0x00FF, 0x00BC, 0x2194, 0x2518 };

    Unicode::CodepointsToUtf32(codepoints.GetBegin(), codepoints.GetEnd(), BackInsertIterator<WideString>(str));
    EXPECT_GENERAL_STREQ(str.Raw(), L"┬ÿ¼↔┘");
}
#endif

TEST(FromCodepointsTests, FromCodepointsUtf8)
{
    String str;
    Array<Unicode::Codepoint> codepoints = { 0x00BB, 0x005F, 0x00FC, 0x00FF, 0x2194, 0x2563, 0x1F923, 0x1F495, 0x1F3B6 };

    Unicode::FromCodepoints(codepoints.GetBegin(), codepoints.GetEnd(), BackInsertIterator<String>(str));
    EXPECT_GENERAL_STREQ(str.Raw(), "»_üÿ↔╣🤣💕🎶");
}

TEST(FromCodepointsTests, FromCodepointsUtf8Char8_t)
{
    U8String str;
    Array<Unicode::Codepoint> codepoints = { 0x00BB, 0x005F, 0x00FC, 0x00FF, 0x2194, 0x2563, 0x1F923, 0x1F495, 0x1F3B6 };

    Unicode::FromCodepoints(codepoints.GetBegin(), codepoints.GetEnd(), BackInsertIterator<U8String>(str));
    EXPECT_GENERAL_STREQ(str.Raw(), u8"»_üÿ↔╣🤣💕🎶");
}

TEST(FromCodepointsTests, FromCodepointsUtf16)
{
    U16String str;
    Array<Unicode::Codepoint> codepoints = { 0x252C, 0x00FF, 0x00BC, 0x2194, 0x2518 };

    Unicode::FromCodepoints(codepoints.GetBegin(), codepoints.GetEnd(), BackInsertIterator<U16String>(str));
    EXPECT_GENERAL_STREQ(str.Raw(), u"┬ÿ¼↔┘");
}

TEST(FromCodepointsTests, FromCodepointsUtf32)
{
    U32String str;
    Array<Unicode::Codepoint> codepoints = { 0x252C, 0x00FF, 0x00BC, 0x2194, 0x2518 };

    Unicode::FromCodepoints(codepoints.GetBegin(), codepoints.GetEnd(), BackInsertIterator<U32String>(str));
    EXPECT_GENERAL_STREQ(str.Raw(), U"┬ÿ¼↔┘");
}

#if defined(_WIN32)
TEST(FromCodepointsTests, FromCodepointsUtf16Wchar_t)
{
    static_assert(sizeof(wchar_t) == 2, "sizeof(wchar_t) should be equal to 2.");

    WideString str;
    Array<Unicode::Codepoint> codepoints = { 0x252C, 0x00FF, 0x00BC, 0x2194, 0x2518 };

    Unicode::FromCodepoints(codepoints.GetBegin(), codepoints.GetEnd(), BackInsertIterator<WideString>(str));
    EXPECT_GENERAL_STREQ(str.Raw(), L"┬ÿ¼↔┘");
}
#else
TEST(FromCodepointsTests, FromCodepointsUtf32Wchar_t)
{
    static_assert(sizeof(wchar_t) == 4, "sizeof(wchar_t) should be equal to 4.");

    WideString str;
    Array<Unicode::Codepoint> codepoints = { 0x252C, 0x00FF, 0x00BC, 0x2194, 0x2518 };

    Unicode::FromCodepoints(codepoints.GetBegin(), codepoints.GetEnd(), BackInsertIterator<WideString>(str));
    EXPECT_GENERAL_STREQ(str.Raw(), L"┬ÿ¼↔┘");
}
#endif
