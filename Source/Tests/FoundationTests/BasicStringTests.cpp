#include <gtest/gtest.h>

#include "TestContainer.h"
#include "IteratorWrappers.h"
#include "CompareStrings.h"

#include "Foundation/String/String.h"

using namespace Kitsune;

namespace BasicStringTesting
{
    class TestAllocator : public Kitsune::GlobalAllocator
    {
    public:
        TestAllocator() = default;
        explicit TestAllocator(int id) : ID(id) { /* ... */ }

        TestAllocator(const TestAllocator&) = default;
        ~TestAllocator() = default;

    public:
        TestAllocator& operator=(const TestAllocator&) = default;
        TestAllocator& operator=(TestAllocator&& a) { ID = std::exchange(a.ID, 0); return *this; }

    public:
        bool operator==(const TestAllocator& alloc) const { return (ID == alloc.ID); }

    public:
        int ID = 0;
    };
}

using Testing::TestContainer, Testing::ForwardIteratorWrapper,
      BasicStringTesting::TestAllocator;

TEST(BasicStringTests, DefaultCtor)
{
    BasicString<char32_t> str{};
    EXPECT_TRUE(str.IsEmpty());
    EXPECT_EQ(str.Size(), 0);
}

TEST(BasicStringTests, AllocCtor)
{
    TestAllocator alloc = TestAllocator(32);
    BasicString<char32_t, TestAllocator> str(alloc);

    EXPECT_TRUE(str.IsEmpty());
    EXPECT_EQ(str.Size(), 0);
    EXPECT_EQ(str.GetAllocator(), alloc);
}

TEST(BasicStringTests, CapacityCtor)
{
    BasicString<char32_t> localStr((Usize)0);      // why
    BasicString<char32_t> heapStr(1000);

    EXPECT_TRUE(localStr.IsLocal());
    EXPECT_EQ(localStr.Size(), 0);

    EXPECT_FALSE(heapStr.IsLocal());
    EXPECT_EQ(heapStr.Size(), 0);
    EXPECT_GE(heapStr.Capacity(), 1000);
}

TEST(BasicStringTests, FillCtor)
{
    BasicString<char32_t> str(50, U'a');

    EXPECT_EQ(str.Size(), 50);
    EXPECT_GE(str.Capacity(), 50);

    for (int i = 0; i < 50; ++i)
        EXPECT_EQ(str.Raw()[i], U'a');
}

TEST(BasicStringTests, CstringLenCtor)
{
    const char16_t* literal = u"Lorem ipsum dolor sit amet, consectetur adipiscing elit.";
    BasicString<char16_t> str(literal, 56);

    EXPECT_EQ(str.Size(), 56);
    EXPECT_GE(str.Capacity(), 56);

    EXPECT_GENERAL_STREQ(str.Raw(), literal);
}

TEST(BasicStringTests, CstringCtor)
{
    const char16_t* literal = u"Lorem ipsum dolor sit amet, consectetur adipiscing elit.";
    BasicString<char16_t> str(literal);

    EXPECT_EQ(str.Size(), 56);
    EXPECT_GE(str.Capacity(), 56);

    EXPECT_GENERAL_STREQ(str.Raw(), literal);
}

TEST(BasicStringTests, RangeCtor)
{
    char32_t rawArray[5] = { U'h', U'e', U'l', U'l', U'o' };
    TestContainer<ForwardIteratorWrapper<char32_t>> cont(rawArray, rawArray + 5);

    BasicString<char32_t> str(cont.Begin, cont.End);

    EXPECT_EQ(str.Size(), 5);
    EXPECT_GE(str.Capacity(), 5);

    EXPECT_GENERAL_STREQ(str.Raw(), U"hello");
}

TEST(BasicStringTests, StringViewCtor)
{
    BasicStringView<char32_t> strv = U"Hello, World!";
    BasicString<char32_t> str(strv);

    EXPECT_EQ(str.Size(), strv.Size());
    EXPECT_GENERAL_STREQ(str.Raw(), strv.Data());
}

TEST(BasicStringTests, CopyCtor)
{
    BasicString<char16_t> str = u"Ut fermentum libero a lectus tincidunt, non condimentum tortor vulputate.";
    BasicString<char16_t> copy = str;

    EXPECT_EQ(copy.Size(), str.Size());
    EXPECT_GENERAL_STREQ(copy.Raw(), str.Raw());
}

TEST(BasicStringTests, MoveCtor)
{
    const char16_t* literal = u"Fusce et tortor porttitor, maximus leo a, mollis ligula.";

    BasicString<char16_t> str = literal;
    BasicString<char16_t> move = std::move(str);

    EXPECT_TRUE(str.IsEmpty());
    EXPECT_EQ(str.Size(), 0);

    EXPECT_EQ(move.Size(), std::char_traits<char16_t>::length(literal));
    EXPECT_GENERAL_STREQ(move.Raw(), literal);
}

TEST(BasicStringTests, InitListCtor)
{
    BasicString<char32_t> str = { U'h', U'e', U'l', U'l', U'o' };

    EXPECT_EQ(str.Size(), 5);
    EXPECT_GE(str.Capacity(), 5);

    EXPECT_GENERAL_STREQ(str.Raw(), U"hello");
}

TEST(BasicStringTests, CopyAssignSameAlloc)
{
    BasicString<char32_t> str = U"In in accumsan nibh, vitae malesuada neque.";
    BasicString<char32_t> copy = U"Donec quis nunc in augue semper dictum.";

    copy = str;

    EXPECT_EQ(copy.Size(), str.Size());
    EXPECT_GENERAL_STREQ(str.Raw(), copy.Raw());
}

TEST(BasicStringTests, CopyAssignDiffAlloc)
{
    TestAllocator alloc = TestAllocator(4);
    TestAllocator diffAlloc = TestAllocator(43);

    BasicString<char32_t, TestAllocator> str(U"In in accumsan nibh, vitae malesuada neque.", alloc);
    BasicString<char32_t, TestAllocator> copy(U"Donec quis nunc in augue semper dictum.", diffAlloc);

    copy = str;

    EXPECT_EQ(copy.Size(), str.Size());
    EXPECT_GENERAL_STREQ(str.Raw(), copy.Raw());
    EXPECT_EQ(copy.GetAllocator(), str.GetAllocator());
}

TEST(BasicStringTests, MoveAssignSameAlloc)
{
    const char32_t* literal = U"In in accumsan nibh, vitae malesuada neque.";

    BasicString<char32_t> str = literal;
    BasicString<char32_t> move = U"Donec quis nunc in augue semper dictum.";

    move = std::move(str);

    EXPECT_TRUE(str.IsEmpty());
    EXPECT_EQ(str.Size(), 0);

    EXPECT_EQ(move.Size(), std::char_traits<char32_t>::length(literal));
    EXPECT_GENERAL_STREQ(move.Raw(), literal);
}

TEST(BasicStringTests, MoveAssignDiffAlloc)
{
    TestAllocator alloc = TestAllocator(54);
    TestAllocator diffAlloc = TestAllocator(32);

    BasicString<char32_t, TestAllocator> str(U"In in accumsan nibh, vitae malesuada neque.", alloc);
    BasicString<char32_t, TestAllocator> move(U"Donec quis nunc in augue semper dictum.", diffAlloc);

    move = std::move(str);

    EXPECT_TRUE(str.IsEmpty());
    EXPECT_EQ(str.Size(), 0);

    EXPECT_EQ(move.Size(), 43);
    EXPECT_GENERAL_STREQ(move.Raw(), U"In in accumsan nibh, vitae malesuada neque.");
    EXPECT_EQ(move.GetAllocator(), alloc);
}

TEST(BasicStringTests, CstringAssign)
{
    const char32_t* literal = U"Donec tincidunt dolor eu augue ultrices cursus.";
    BasicString<char32_t> str = U"Nullam justo justo, tempor id quam nec, sodales volutpat magna.";

    str = literal;

    EXPECT_EQ(str.Size(), std::char_traits<char32_t>::length(literal));
    EXPECT_GENERAL_STREQ(str.Raw(), literal);
}

TEST(BasicStringTests, CharAssign)
{
    BasicString<char16_t> str = u"Donec tempor dictum turpis non efficitur.";
    str = 'c';

    EXPECT_EQ(str.Size(), 1);
    EXPECT_EQ(str.Raw()[0], 'c');
    EXPECT_EQ(str.Raw()[1], '\0');
}

TEST(BasicStringTests, InitListAssign)
{
    BasicString<char16_t> str = u"Donec tempor dictum turpis non efficitur.";
    str = { u'c', u'u', u'p', u's' };

    EXPECT_EQ(str.Size(), 4);
    EXPECT_GENERAL_STREQ(str.Raw(), u"cups");
}

TEST(BasicStringTests, StringViewAssign)
{
    BasicStringView<char16_t> strv = u"Donec tempor dictum turpis non efficitur.";
    BasicString<char16_t> str = u"Bear School Rubber";

    str = strv;

    EXPECT_EQ(str.Size(), strv.Size());
    EXPECT_GENERAL_STREQ(str.Raw(), strv.Data());
}

TEST(BasicStringTests, Subscript)
{
    BasicString<char16_t> str = u"Duis nunc nisi, tempus in lacus nec, ullamcorper gravida dui";
    for (Usize i = 0; i < str.Size(); ++i)
        EXPECT_EQ(str[i], str.Raw()[i]);
}

TEST(BasicStringTests, StringViewConvertion)
{
    BasicString<char8_t> str = u8"Phasellus euismod faucibus urna, aliquet elementum massa posuere eget.";
    BasicStringView<char8_t> strv = str;

    EXPECT_EQ(strv.Size(), str.Size());
    EXPECT_GENERAL_STREQ(strv.Data(), str.Raw());
}

TEST(BasicStringTests, FrontBack)
{
    BasicString<char> str = "Donec dolor tellus, sagittis et maximus tincidunt, tincidunt in mi.";
    EXPECT_EQ(str.Front(), str.Data()[0]);
    EXPECT_EQ(str.Back(), str.Data()[str.Size() - 1]);
}

TEST(BasicStringTests, DataRaw)
{
    BasicString<char> str = "Cras consectetur ante porta lorem feugiat convallis.";
    EXPECT_EQ(str.Raw(), &(str.Front()));
    EXPECT_EQ(str.Data(), &(str.Front()));
}

TEST(BasicStringTests, Capacity)
{
    BasicString<char32_t> str = U"Aliquam urna ipsum, pellentesque non malesuada et, porttitor at orci.";
    EXPECT_GE(str.Capacity(), str.Size());
}

TEST(BasicStringTests, IsLocal)
{
    BasicString<char32_t> local = U"c";
    BasicString<char32_t> heapAllocated = U"Nunc imperdiet erat arcu, a gravida augue dignissim quis.";

    EXPECT_TRUE(local.IsLocal());
    EXPECT_FALSE(heapAllocated.IsLocal());
}

TEST(BasicStringTests, Size)
{
    BasicString<char32_t> empty;
    BasicString<char32_t> str = U"Maecenas vitae aliquam tortor, eu volutpat risus.";

    EXPECT_EQ(empty.Size(), 0);
    EXPECT_EQ(str.Size(), 49);
}

TEST(BasicStringTests, GetAllocator)
{
    TestAllocator alloc = TestAllocator(32);
    BasicString<char32_t, TestAllocator> str(alloc);

    EXPECT_EQ(str.GetAllocator(), alloc);
}

TEST(BasicStringTests, IsEmpty)
{
    BasicString<char> empty;
    BasicString<char> str = "Suspendisse potenti.";

    EXPECT_FALSE(str.IsEmpty());
    EXPECT_TRUE(empty.IsEmpty());
}

TEST(BasicStringTests, Iterators)
{
    BasicString<char> str = "Fusce vulputate rhoncus purus, tempus luctus erat tempor nec.";
    EXPECT_EQ(*str.GetBegin(), str.Front());
    EXPECT_EQ(str.GetEnd(), str.GetBegin() + str.Size());

    EXPECT_EQ(*str.GetReverseBegin(), str.Back());
    EXPECT_EQ(str.GetReverseEnd(), str.GetReverseBegin() + static_cast<Ptrdiff>(str.Size()));
}

TEST(BasicStringTests, Reserve)
{
    BasicString<char16_t> str = u"Hi.";
    str.Reserve(124);

    EXPECT_GE(str.Capacity(), 124);
    EXPECT_EQ(str.Size(), 3);
    EXPECT_GENERAL_STREQ(str.Raw(), u"Hi.");
}

TEST(BasicStringTests, ShrinkToFit)
{
    const char16_t heapStrLiteral[] = u"Suspendisse sit amet nunc mauris.";

    BasicString<char16_t> localStr = u"Hi..";
    BasicString<char16_t> heapStr = heapStrLiteral;

    localStr.ShrinkToFit();
    heapStr.ShrinkToFit();

    EXPECT_TRUE(localStr.IsLocal());
    EXPECT_EQ(heapStr.Capacity(), heapStr.Size());

    EXPECT_GENERAL_STREQ(heapStr.Raw(), heapStrLiteral);
}

TEST(BasicStringTests, Clear)
{
    BasicString<char32_t> str = U"Vestibulum ullamcorper nulla eu lectus venenatis, sit amet congue massa egestas.";
    str.Clear();

    EXPECT_EQ(str.Size(), 0);
    EXPECT_EQ(str.Data()[0], '\0');
    EXPECT_GE(str.Capacity(), 0);
}

TEST(BasicStringTests, InsertCstring)
{
    BasicString<char32_t> str = U"Fusce eget ex efficitur, aliquet felis";
    const char32_t* expected = U"Fusce eget ex efficitur, mollis diam cursus, aliquet felis.";

    str.Insert(str.GetBegin() + 25, U"mollis diam cursus, ");
    str.Insert(str.GetEnd(), U".");

    EXPECT_GENERAL_STREQ(str.Raw(), expected);
    EXPECT_EQ(str.Size(), 59);
}

TEST(BasicStringTests, InsertString)
{
    BasicString<char32_t> str = U"Fusce eget ex efficitur, aliquet felis.";
    BasicString<char32_t> inserted = U"mollis diam cursus, ";

    const char32_t* expected = U"Fusce eget ex efficitur, mollis diam cursus, aliquet felis.";
    str.Insert(str.GetBegin() + 25, inserted);

    EXPECT_GENERAL_STREQ(str.Raw(), expected);
    EXPECT_EQ(str.Size(), 59);
}

TEST(BasicStringTests, InsertChar)
{
    BasicString<char16_t> str = u"In facilisis nisl in placerat lacini.";
    const char16_t* expected = u"In facilisis nisl in placerat lacinia.";

    str.Insert(str.GetEnd() - 1, u'a');

    EXPECT_EQ(str.Size(), 38);
    EXPECT_GENERAL_STREQ(str.Raw(), expected);
}

TEST(BasicStringTests, InsertMultiChar)
{
    BasicString<char16_t> str = u"In facilisis nisl in placerat lacinia";
    const char16_t* expected = u"In facilisis nisl in placerat lacinia..........";

    str.Insert(str.GetEnd(), 10, u'.');

    EXPECT_EQ(str.Size(), 47);
    EXPECT_GENERAL_STREQ(str.Raw(), expected);
}

TEST(BasicStringTests, InsertStringView)
{
    BasicString<char32_t> str = U"Fusce eget ex efficitur, aliquet felis.";
    BasicStringView<char32_t> inserted = U"mollis diam cursus, ";

    const char32_t* expected = U"Fusce eget ex efficitur, mollis diam cursus, aliquet felis.";
    str.Insert(str.GetBegin() + 25, inserted);

    EXPECT_GENERAL_STREQ(str.Raw(), expected);
    EXPECT_EQ(str.Size(), 59);
}

TEST(BasicStringTests, RemoveChar)
{
    BasicString<char16_t> str = u"Quipsque diam ex, porttitor eget risus id, cursus dapibus tellus.";
    const char16_t* expected = u"Quisque diam ex, porttitor eget risus id, cursus dapibus tellus.";
    str.Remove(str.GetBegin() + 3);

    EXPECT_EQ(str.Size(), 64);
    EXPECT_GENERAL_STREQ(str.Raw(), expected);
}

TEST(BasicStringTests, RemoveRange)
{
    BasicString<char16_t> str = u"Hi!!!!! Quisque diam ex, porttitor eget risus id, cursus dapibus tellus.";
    const char16_t* expected = u"Quisque diam ex, porttitor eget risus id, cursus dapibus tellus.";

    str.Remove(str.GetBegin(), str.GetBegin() + 8);

    EXPECT_EQ(str.Size(), 64);
    EXPECT_GENERAL_STREQ(str.Raw(), expected);
}

TEST(BasicStringTests, PushBack)
{
    BasicString<char32_t> str = U"Donec egestas sem eget augue molestie, sit amet condimentum mauris aliquet";
    const char32_t* expected = U"Donec egestas sem eget augue molestie, sit amet condimentum mauris aliquet.";

    str.PushBack(U'.');

    EXPECT_GENERAL_STREQ(str.Raw(), expected);
    EXPECT_EQ(str.Size(), 75);
}

TEST(BasicStringTests, PopBack)
{
    BasicString<char32_t> str = U"Donec egestas sem eget augue molestie, sit amet condimentum mauris aliquet..";
    const char32_t* expected = U"Donec egestas sem eget augue molestie, sit amet condimentum mauris aliquet.";

    str.PopBack();

    EXPECT_GENERAL_STREQ(str.Raw(), expected);
    EXPECT_EQ(str.Size(), 75);
}

TEST(BasicStringTests, AppendFill)
{
    BasicString<char16_t> str = u"Aliquam lacus augue, lacinia vel sagittis fermentum, gravida et sem.";
    const char16_t* expected = u"Aliquam lacus augue, lacinia vel sagittis fermentum, gravida et sem.fffff";

    str.Append(5, 'f');

    EXPECT_GENERAL_STREQ(str.Raw(), expected);
    EXPECT_EQ(str.Size(), 73);
}

TEST(BasicStringTests, AppendString)
{
    BasicString<char16_t> str = u"Aliquam lacus augue, lacinia vel sagittis fermentum, gravida et sem.";
    BasicString<char16_t> app = u"weeee";

    str.Append(app);

    EXPECT_GENERAL_STREQ(str.Raw(), u"Aliquam lacus augue, lacinia vel sagittis fermentum, gravida et sem.weeee");
    EXPECT_EQ(str.Size(), 73);
}

TEST(BasicStringTests, AppendCstring)
{
    BasicString<char16_t> str = u"Aliquam lacus augue, lacinia vel sagittis fermentum, gravida et sem.";
    str.Append(u"weeee");

    EXPECT_GENERAL_STREQ(str.Raw(), u"Aliquam lacus augue, lacinia vel sagittis fermentum, gravida et sem.weeee");
    EXPECT_EQ(str.Size(), 73);
}

TEST(BasicStringTests, AppendRange)
{
    BasicString<char16_t> str = u"Aliquam lacus augue, lacinia vel sagittis fermentum, gravida et sem.";

    char16_t rawArray[5] = { 'w', 'e', 'e', 'e', 'e' };
    TestContainer<ForwardIteratorWrapper<char16_t>> cont(rawArray, rawArray + 5);

    str.Append(cont.Begin, cont.End);

    EXPECT_GENERAL_STREQ(str.Raw(), u"Aliquam lacus augue, lacinia vel sagittis fermentum, gravida et sem.weeee");
    EXPECT_EQ(str.Size(), 73);
}

TEST(BasicStringTests, AppendInitList)
{
    BasicString<char16_t> str = u"Aliquam lacus augue, lacinia vel sagittis fermentum, gravida et sem.";
    str.Append({ 'w', 'e', 'e', 'e', 'e' });

    EXPECT_GENERAL_STREQ(str.Raw(), u"Aliquam lacus augue, lacinia vel sagittis fermentum, gravida et sem.weeee");
    EXPECT_EQ(str.Size(), 73);
}

TEST(BasicStringTests, AppendStringView)
{
    BasicString<char16_t> str = u"Aliquam lacus augue, lacinia vel sagittis fermentum, gravida et sem.";
    BasicStringView<char16_t> app = u"weeee";

    str.Append(app);

    EXPECT_GENERAL_STREQ(str.Raw(), u"Aliquam lacus augue, lacinia vel sagittis fermentum, gravida et sem.weeee");
    EXPECT_EQ(str.Size(), 73);
}

TEST(BasicStringTests, Substring)
{
    BasicString<char16_t> str = u"Heart Star Clover";
    BasicString<char16_t> substr = str.Substring(6, 4);        // "Star"

    EXPECT_EQ(substr.Size(), 4);
    EXPECT_GENERAL_STREQ(substr.Data(), u"Star");
}

TEST(BasicStringTests, RangedForLoop)
{
    BasicString<char8_t> str = u8"abcdefghijklmnopqrstuvwxyz";
    char8_t i = 'a';

    for (char8_t e : str)
    {
        EXPECT_EQ(e, i);
        ++i;
    }
}

TEST(BasicStringTests, Equal)
{
    BasicString<char32_t> arr = U"Cras risus odio, tempus feugiat velit maximus, sollicitudin elementum magna.";
    BasicString<char32_t> arr2 = U"Cras risus odio, tempus feugiat velit maximus, sollicitudin elementum magna.";

    BasicString<char32_t> diff = U"Cras risus odio, tempus feugiat velit maximus, sollicitudin elementum magna";

    EXPECT_TRUE(arr == arr2);
    EXPECT_FALSE(arr == diff);

    EXPECT_TRUE(arr == arr2.Raw());
    EXPECT_FALSE(arr == diff.Raw());

    EXPECT_TRUE(arr.Raw() == arr2);
    EXPECT_FALSE(arr.Raw() == diff);
}
