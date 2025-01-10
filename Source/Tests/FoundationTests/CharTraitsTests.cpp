#include <gtest/gtest.h>

#include "CompareStrings.h"
#include "Foundation/String/CharTraits.h"

#include <cstring>

using namespace Kitsune;

#define EXPECT_SGN_EQ(x, y) EXPECT_EQ(KITSUNE_SIGN(x), KITSUNE_SIGN(y))

TEST(CharTraitsTests, Copy)
{
    char literal[] = "Hello, World!";
    wchar_t wliteral[] = L"Hello, Worthmrtld!";
    char8_t u8literal[] = u8"Hello, Woraw1234ld!";
    char16_t u16literal[] = u"Hello, Worldadad!";
    char32_t u32literal[] = U"Hello, World! Weeee";

    char buffer[KITSUNE_ARRAY_SIZE(literal)];
    wchar_t wbuffer[KITSUNE_ARRAY_SIZE(wliteral)];
    char8_t u8buffer[KITSUNE_ARRAY_SIZE(u8literal)];
    char16_t u16buffer[KITSUNE_ARRAY_SIZE(u16literal)];
    char32_t u32buffer[KITSUNE_ARRAY_SIZE(u32literal)];

    EXPECT_EQ(CharTraits<char>::Copy(buffer, literal, KITSUNE_ARRAY_SIZE(buffer)), buffer);
    EXPECT_EQ(CharTraits<wchar_t>::Copy(wbuffer, wliteral, KITSUNE_ARRAY_SIZE(wbuffer)), wbuffer);
    EXPECT_EQ(CharTraits<char8_t>::Copy(u8buffer, u8literal, KITSUNE_ARRAY_SIZE(u8buffer)), u8buffer);
    EXPECT_EQ(CharTraits<char16_t>::Copy(u16buffer, u16literal, KITSUNE_ARRAY_SIZE(u16buffer)), u16buffer);
    EXPECT_EQ(CharTraits<char32_t>::Copy(u32buffer, u32literal, KITSUNE_ARRAY_SIZE(u32buffer)), u32buffer);

    EXPECT_GENERAL_STREQ(buffer, literal);
    EXPECT_GENERAL_STREQ(wbuffer, wliteral);
    EXPECT_GENERAL_STREQ(u8buffer, u8literal);
    EXPECT_GENERAL_STREQ(u16buffer, u16literal);
    EXPECT_GENERAL_STREQ(u32buffer, u32literal);
}

TEST(CharTraitsTests, Length)
{
    char buffer[] = "Hello, World!";
    wchar_t wbuffer[] = L"Hello, World!";
    char8_t u8buffer[] = u8"Hello, World!";
    char16_t u16buffer[] = u"Hello, World!";
    char32_t u32buffer[] = U"Hello, World!";

    EXPECT_EQ(CharTraits<char>::Length(buffer), 13);
    EXPECT_EQ(CharTraits<wchar_t>::Length(wbuffer), 13);
    EXPECT_EQ(CharTraits<char8_t>::Length(u8buffer), 13);
    EXPECT_EQ(CharTraits<char16_t>::Length(u16buffer), 13);
    EXPECT_EQ(CharTraits<char32_t>::Length(u32buffer), 13);
}

TEST(CharTraitsTests, Compare)
{
    char literal[] = "Hello, World!\n";
    char sameLiteral[] = "Hello, World!\n";
    char diffLiteral[] = "Hello, Yorld!\n";

    wchar_t wliteral[] = L"Hello, World!\n";
    wchar_t wsameLiteral[] = L"Hello, World!\n";
    wchar_t wdiffLiteral[] = L"Hello, 5orld!\n";

    char8_t u8literal[] = u8"Hello, World!\n";
    char8_t u8sameLiteral[] = u8"Hello, World!\n";
    char8_t u8diffLiteral[] = u8"Hello, Porld!\n";

    char16_t u16literal[] = u"Hello, World!\n";
    char16_t u16sameLiteral[] = u"Hello, World!\n";
    char16_t u16diffLiteral[] = u"Hello, Torld!\n";

    char32_t u32literal[] = U"Hello, World!\n";
    char32_t u32sameLiteral[] = U"Hello, World!\n";
    char32_t u32diffLiteral[] = U"Hello, 1orld!\n";

    EXPECT_SGN_EQ(CharTraits<char>::Compare(literal, sameLiteral, KITSUNE_ARRAY_SIZE(literal)), 0);
    EXPECT_SGN_EQ(CharTraits<char>::Compare(literal, diffLiteral, KITSUNE_ARRAY_SIZE(literal)), std::char_traits<char>::compare(literal, diffLiteral, 15));

    EXPECT_SGN_EQ(CharTraits<wchar_t>::Compare(wliteral, wsameLiteral, KITSUNE_ARRAY_SIZE(wliteral)), 0);
    EXPECT_SGN_EQ(CharTraits<wchar_t>::Compare(wliteral, wdiffLiteral, KITSUNE_ARRAY_SIZE(wliteral)), std::char_traits<wchar_t>::compare(wliteral, wdiffLiteral, 15));

    EXPECT_SGN_EQ(CharTraits<char16_t>::Compare(u16literal, u16sameLiteral, KITSUNE_ARRAY_SIZE(u16literal)), 0);
    EXPECT_SGN_EQ(CharTraits<char16_t>::Compare(u16literal, u16diffLiteral, KITSUNE_ARRAY_SIZE(u16literal)), std::char_traits<char16_t>::compare(u16literal, u16diffLiteral, 15));

    EXPECT_SGN_EQ(CharTraits<char32_t>::Compare(u32literal, u32sameLiteral, KITSUNE_ARRAY_SIZE(u32literal)), 0);
    EXPECT_SGN_EQ(CharTraits<char32_t>::Compare(u32literal, u32diffLiteral, KITSUNE_ARRAY_SIZE(u32literal)), std::char_traits<char32_t>::compare(u32literal, u32diffLiteral, 15));

    EXPECT_SGN_EQ(CharTraits<char8_t>::Compare(u8literal, u8sameLiteral, KITSUNE_ARRAY_SIZE(u8literal)), 0);
    EXPECT_SGN_EQ(CharTraits<char8_t>::Compare(u8literal, u8diffLiteral, KITSUNE_ARRAY_SIZE(u8literal)), std::char_traits<char8_t>::compare(u8literal, u8diffLiteral, 15));
}

TEST(CharTraitsTests, FindChr)
{
    char haystack[] = "Hello, World!";
    char needle = 'l';
    char incorrectNeedle = '@';

    wchar_t whaystack[] = L"Hello, World!";
    wchar_t wneedle = L'l';
    wchar_t wincorrectNeedle = L'@';

    char8_t u8haystack[] = u8"Hello, World!";
    char8_t u8needle = u8'l';
    char8_t u8incorrectNeedle = u8'@';

    char16_t u16haystack[] = u"Hello, World!";
    char16_t u16needle = u'l';
    char16_t u16incorrectNeedle = u'@';

    char32_t u32haystack[] = U"Hello, World!";
    char32_t u32needle = U'l';
    char32_t u32incorrectNeedle = U'@';

    EXPECT_EQ(CharTraits<char>::Find(haystack, KITSUNE_ARRAY_SIZE(haystack), needle), haystack + 2);
    EXPECT_EQ(CharTraits<char>::Find(haystack, KITSUNE_ARRAY_SIZE(haystack), incorrectNeedle), nullptr);

    EXPECT_EQ(CharTraits<wchar_t>::Find(whaystack, KITSUNE_ARRAY_SIZE(whaystack), wneedle), whaystack + 2);
    EXPECT_EQ(CharTraits<wchar_t>::Find(whaystack, KITSUNE_ARRAY_SIZE(whaystack), wincorrectNeedle), nullptr);

    EXPECT_EQ(CharTraits<char16_t>::Find(u16haystack, KITSUNE_ARRAY_SIZE(u16haystack), u16needle), u16haystack + 2);
    EXPECT_EQ(CharTraits<char16_t>::Find(u16haystack, KITSUNE_ARRAY_SIZE(u16haystack), u16incorrectNeedle), nullptr);

    EXPECT_EQ(CharTraits<char8_t>::Find(u8haystack, KITSUNE_ARRAY_SIZE(u8haystack), u8needle), u8haystack + 2);
    EXPECT_EQ(CharTraits<char8_t>::Find(u8haystack, KITSUNE_ARRAY_SIZE(u8haystack), u8incorrectNeedle), nullptr);

    EXPECT_EQ(CharTraits<char32_t>::Find(u32haystack, KITSUNE_ARRAY_SIZE(u32haystack), u32needle), u32haystack + 2);
    EXPECT_EQ(CharTraits<char32_t>::Find(u32haystack, KITSUNE_ARRAY_SIZE(u32haystack), u32incorrectNeedle), nullptr);
}
