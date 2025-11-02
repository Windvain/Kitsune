#include "CompareStrings.h"
#include "IteratorWrappers.h"

#include "Foundation/String/ToCodepoints.h"

#include "Foundation/String/String.h"
#include "Foundation/Containers/Array.h"
#include "Foundation/Iterators/BackInsertIterator.h"

using namespace Kitsune;

TEST(ToCodepointsTests, ToCodepointsChar)
{
    String string = "á♣♫ñ∙❤️😂😶‍🌫️😱";
    Array<Unicode::Codepoint> codepoints;

    Unicode::ToCodepoints(Testing::ForwardIteratorWrapper(string.GetBegin()), Testing::ForwardIteratorWrapper((string.GetEnd())),
                          BackInsertIterator<decltype(codepoints)>(codepoints));

    Unicode::Codepoint ExpectedCodepoints[] = { 0x00E1, 0x2663, 0x266B, 0x00F1, 0x2219, 0x2764, 0xFE0F, 0x1F602, 0x1F636, 0x200D, 0x1F32B, 0xFE0F, 0x1F631 };
    Uint32 i = 0;

    for (auto it = codepoints.GetBegin(); it != codepoints.GetEnd(); ++it)
    {
        EXPECT_EQ(*it, ExpectedCodepoints[i++]);
    }
}

TEST(ToCodepointsTests, ToCodepointsChar8_t)
{
    U8String string = u8"á♣♫ñ∙❤️😂😶‍🌫️😱";
    Array<Unicode::Codepoint> codepoints;

    Unicode::ToCodepoints(Testing::ForwardIteratorWrapper(string.GetBegin()), Testing::ForwardIteratorWrapper(string.GetEnd()),
                          BackInsertIterator<decltype(codepoints)>(codepoints));


    Unicode::Codepoint ExpectedCodepoints[] = { 0x00E1, 0x2663, 0x266B, 0x00F1, 0x2219, 0x2764, 0xFE0F, 0x1F602, 0x1F636, 0x200D, 0x1F32B, 0xFE0F, 0x1F631 };
    Uint32 i = 0;

    for (auto it = codepoints.GetBegin(); it != codepoints.GetEnd(); ++it)
    {
        EXPECT_EQ(*it, ExpectedCodepoints[i++]);
    }
}

TEST(ToCodepointsTests, ToCodepointsChar16_t)
{
    U16String string = u"á♣♫ñ∙❤️😂😶‍🌫️😱";
    Array<Unicode::Codepoint> codepoints;

    Unicode::ToCodepoints(Testing::ForwardIteratorWrapper(string.GetBegin()), Testing::ForwardIteratorWrapper(string.GetEnd()),
                          BackInsertIterator<decltype(codepoints)>(codepoints));


    Unicode::Codepoint ExpectedCodepoints[] = { 0x00E1, 0x2663, 0x266B, 0x00F1, 0x2219, 0x2764, 0xFE0F, 0x1F602, 0x1F636, 0x200D, 0x1F32B, 0xFE0F, 0x1F631 };
    Uint32 i = 0;

    for (auto it = codepoints.GetBegin(); it != codepoints.GetEnd(); ++it)
    {
        EXPECT_EQ(*it, ExpectedCodepoints[i++]);
    }
}

TEST(ToCodepointsTests, ToCodepointsChar32_t)
{
    U32String string = U"á♣♫ñ∙❤️😂😶‍🌫️😱";
    Array<Unicode::Codepoint> codepoints;

    Unicode::ToCodepoints(Testing::ForwardIteratorWrapper(string.GetBegin()), Testing::ForwardIteratorWrapper(string.GetEnd()),
                          BackInsertIterator<decltype(codepoints)>(codepoints));


    Unicode::Codepoint ExpectedCodepoints[] = { 0x00E1, 0x2663, 0x266B, 0x00F1, 0x2219, 0x2764, 0xFE0F, 0x1F602, 0x1F636, 0x200D, 0x1F32B, 0xFE0F, 0x1F631 };
    Uint32 i = 0;

    for (auto it = codepoints.GetBegin(); it != codepoints.GetEnd(); ++it)
    {
        EXPECT_EQ(*it, ExpectedCodepoints[i++]);
    }
}

TEST(ToCodepointsTests, ToCodepointsWchar_t)
{
    WideString string = L"á♣♫ñ∙❤️😂😶‍🌫️😱";
    Array<Unicode::Codepoint> codepoints;

    Unicode::ToCodepoints(Testing::ForwardIteratorWrapper(string.GetBegin()), Testing::ForwardIteratorWrapper(string.GetEnd()),
                          BackInsertIterator<decltype(codepoints)>(codepoints));


    Unicode::Codepoint ExpectedCodepoints[] = { 0x00E1, 0x2663, 0x266B, 0x00F1, 0x2219, 0x2764, 0xFE0F, 0x1F602, 0x1F636, 0x200D, 0x1F32B, 0xFE0F, 0x1F631 };
    Uint32 i = 0;

    for (auto it = codepoints.GetBegin(); it != codepoints.GetEnd(); ++it)
    {
        EXPECT_EQ(*it, ExpectedCodepoints[i++]);
    }
}
