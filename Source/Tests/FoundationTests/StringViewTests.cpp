#include <gtest/gtest.h>

#include "CompareStrings.h"
#include "Foundation/String/StringView.h"

using namespace Kitsune;

TEST(BasicStringViewTests, DefaultCtor)
{
    BasicStringView<char32_t> str;
    EXPECT_EQ(str.Size(), 0);
    EXPECT_EQ(str.Data(), nullptr);
}

TEST(BasicStringViewTests, CstringCtor)
{
    BasicStringView<wchar_t> str = L"Hello, World!";

    EXPECT_EQ(str.Size(), 13);
    EXPECT_GENERAL_STREQ(str.Data(), L"Hello, World!");
}

TEST(BasicStringViewTests, CstringLenCtor)
{
    BasicStringView<char16_t> str(u"Hello, there!", 13);

    EXPECT_EQ(str.Size(), 13);
    EXPECT_GENERAL_STREQ(str.Data(), u"Hello, there!");
}

TEST(BasicStringViewTests, CopyCtor)
{
    BasicStringView<char32_t> str = U"I am a string.";
    BasicStringView<char32_t> copy = str;

    EXPECT_EQ(copy.Size(), str.Size());
    EXPECT_GENERAL_STREQ(copy.Data(), str.Data());
}

TEST(BasicStringViewTests, CopyAssignCtor)
{
    BasicStringView<char32_t> str = U"I am a string.";
    BasicStringView<char32_t> copy = U"I am a random string.";

    copy = str;

    EXPECT_EQ(copy.Size(), str.Size());
    EXPECT_GENERAL_STREQ(copy.Data(), str.Data());
}

TEST(BasicStringViewTests, Subscript)
{
    BasicStringView<char16_t> str = u"Hello!";
    EXPECT_EQ(str[0], 'H');
    EXPECT_EQ(str[1], 'e');
    EXPECT_EQ(str[2], 'l');
    EXPECT_EQ(str[3], 'l');
    EXPECT_EQ(str[4], 'o');
    EXPECT_EQ(str[5], '!');
}

TEST(BasicStringViewTests, FrontBack)
{
    BasicStringView<char32_t> str = U"Cow Horse Doctor";
    EXPECT_EQ(str.Front(), 'C');
    EXPECT_EQ(str.Back(), 'r');
}

TEST(BasicStringViewTests, Data)
{
    const char16_t* cstring = u"Hello there!";
    BasicStringView<char16_t> str = cstring;

    EXPECT_EQ(str.Data(), cstring);
}

TEST(BasicStringViewTests, Size)
{
    BasicStringView<char> str("Hello!", 3);
    EXPECT_EQ(str.Size(), 3);
}

TEST(BasicStringViewTests, IsEmpty)
{
    BasicStringView<char16_t> str = u"Hello, World!";
    BasicStringView<char16_t> empty = u"";

    EXPECT_TRUE(empty.IsEmpty());
    EXPECT_FALSE(str.IsEmpty());
}

// TEST(BasicStringViewTests, Length)
// {
//     BasicStringView<char8_t> str = u8"┬↔╓Æ╧ª";
//     BasicStringView<char8_t> empty = u8"";

//     EXPECT_EQ(str.Length(), 6);
//     EXPECT_EQ(empty.Length(), 0);
// }

TEST(BasicStringViewTests, RemovePrefix)
{
    BasicStringView<char16_t> str = u"Hello, World!";
    str.RemovePrefix(5);

    EXPECT_EQ(str.Size(), 8);
    EXPECT_EQ(std::char_traits<char16_t>::compare(str.Data(), u", World!", 8), 0);
}

TEST(BasicStringViewTests, RemoveSuffix)
{
    BasicStringView<char16_t> str = u"Hello, World!";
    str.RemoveSuffix(5);

    EXPECT_EQ(str.Size(), 8);
    EXPECT_EQ(std::char_traits<char16_t>::compare(str.Data(), u"Hello, W", 8), 0);
}

TEST(BasicStringViewTests, Substring)
{
    BasicStringView<char16_t> str = u"Hello, World!";
    BasicStringView<char16_t> substr = str.Substring(7, 5);        // "World"

    EXPECT_EQ(substr.Size(), 5);
    EXPECT_EQ(std::char_traits<char16_t>::compare(substr.Data(), u"World", 5), 0);
}

TEST(BasicStringViewTests, Iterators)
{
    BasicStringView<char> str = "Pig Listen Broken";
    EXPECT_EQ(*str.GetBegin(), str.Front());
    EXPECT_EQ(str.GetEnd(), str.GetBegin() + str.Size());

    EXPECT_EQ(*str.GetReverseBegin(), str.Back());
    EXPECT_EQ(str.GetReverseEnd(), str.GetReverseBegin() + (Ptrdiff)str.Size());
}

// TEST(BasicStringViewTests, CodepointIterators)
// {
//     BasicStringView<char> str = "🌞♦Nâeÿ♠n┘l";
//     auto end = str.GetCodepointBegin();

//     for (Usize i = 0; i < 10; ++i)
//         ++end;

//     EXPECT_EQ(*str.GetCodepointBegin(), 0x1F31E);
//     EXPECT_EQ(str.GetCodepointEnd(), end);
// }

TEST(BasicStringViewTests, RangedForLoop)
{
    BasicStringView<char8_t> str = u8"abcdefghijklmnopqrstuvwxyz";
    char8_t i = 'a';

    for (char8_t e : str)
    {
        EXPECT_EQ(e, i);
        ++i;
    }
}

TEST(BasicStringViewTests, Equal)
{
    BasicStringView<char32_t> arr = U"Cras risus odio, tempus feugiat velit maximus, sollicitudin elementum magna.";
    BasicStringView<char32_t> arr2 = U"Cras risus odio, tempus feugiat velit maximus, sollicitudin elementum magna.";

    BasicStringView<char32_t> diff = U"Cras risus odio, tempus feugiat velit maximus, sollicitudin elementum magna";

    EXPECT_TRUE(arr == arr2);
    EXPECT_FALSE(arr == diff);
}

TEST(BasicStringViewTests, EqualCstr)
{
    BasicStringView<char32_t> arr = U"Cras risus odio, tempus feugiat velit maximus, sollicitudin elementum magna.";
    const char32_t* arr2 = U"Cras risus odio, tempus feugiat velit maximus, sollicitudin elementum magna.";

    const char32_t* diff = U"Cras risus odio, tempus feugiat velit maximus, sollicitudin elementum magna";

    EXPECT_TRUE(arr == arr2);
    EXPECT_FALSE(arr == diff);
}
