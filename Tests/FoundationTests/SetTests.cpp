#include <gtest/gtest.h>
#include "TestContainer.h"

#include "Foundation/Containers/Set.h"

using namespace Kitsune;
using namespace Kitsune::Testing;

namespace
{
    template<typename T>
    class StatefulCompare
    {
    public:
        StatefulCompare(bool flip = false)
            : Flip(flip)
        {
        }

        bool operator()(const T& lhs, const T& rhs)
        {
            return Flip ? (lhs > rhs) : (lhs < rhs);
        }

    public:
        bool Flip;
    };

    class MyAllocator
    {
    public:
        MyAllocator(int value = 0)
            : Value(value)
        {
        }

    public:
        KITSUNE_FORCEINLINE void* Allocate(Usize bytes)
        {
            return Memory::Allocate(bytes);
        }

        KITSUNE_FORCEINLINE void* Allocate(Usize bytes, Usize alignment)
        {
            return Memory::Allocate(bytes, alignment);
        }

        KITSUNE_FORCEINLINE void Free(void* pointer, Usize bytes)
        {
            Memory::Free(pointer, bytes);
        }

    public:
        int Value;
    };

    inline bool operator==(const MyAllocator&, const MyAllocator&)
    {
        return true;
    }
}

static_assert(Kitsune::ForwardIterator<Set<int>::Iterator>,
              "Set<T>'s iterator doesn't satisfy ForwardIterator.");

static_assert(Kitsune::ForwardIterator<Set<int>::ConstIterator>,
              "Set<T>'s iterator doesn't satisfy ForwardIterator.");

TEST(SetTests, DefaultConstructor)
{
    Set<int, StatefulCompare<int>, MyAllocator> set;

    /* Can't check StatefulCompare. */

    EXPECT_EQ(set.Size(), 0);
    EXPECT_EQ(set.GetAllocator().Value, 0);
}

TEST(SetTests, CompareAllocConstructor)
{
    Set<int, StatefulCompare<int>, MyAllocator> set(
        StatefulCompare<int>{true}, MyAllocator(2));

    /* Can't check StatefulCompare. */

    EXPECT_EQ(set.Size(), 0);
    EXPECT_EQ(set.GetAllocator().Value, 2);
}

TEST(SetTests, AllocConstructor)
{
    Set<int, StatefulCompare<int>, MyAllocator> set(MyAllocator(2));

    /* Can't check StatefulCompare. */

    EXPECT_EQ(set.Size(), 0);
    EXPECT_EQ(set.GetAllocator().Value, 2);
}

TEST(SetTests, RangeCompareAllocatorConstructor)
{
    ForwardTestContainer<int, 5> container = { 23, 1, 5, 9, 5 };
    int expected[4] = { 23, 9, 5, 1 };

    const Set<int, StatefulCompare<int>, MyAllocator> set(
        container.GetBegin(), container.GetEnd(),
        StatefulCompare<int>{true}, MyAllocator(231));

    EXPECT_EQ(set.Size(), 4);
    EXPECT_EQ(set.GetAllocator().Value, 231);

    Index index = 0;
    for (auto iter = set.GetBegin(); iter != set.GetEnd(); ++iter)
    {
        EXPECT_EQ(*iter, expected[index]);
        ++index;
    }
}

TEST(SetTests, RangeAllocatorConstructor)
{
    ForwardTestContainer<int, 5> container = { 23, 1, 5, 9, 5 };
    int expected[4] = { 1, 5, 9, 23 };

    Set<int, StatefulCompare<int>, MyAllocator> set(
        container.GetBegin(), container.GetEnd(),
        MyAllocator(231));

    EXPECT_EQ(set.Size(), 4);
    EXPECT_EQ(set.GetAllocator().Value, 231);

    Index index = 0;
    for (auto iter = set.GetBegin(); iter != set.GetEnd(); ++iter)
    {
        EXPECT_EQ(*iter, expected[index]);
        ++index;
    }
}

TEST(SetTests, CopyConstructor)
{
    ForwardTestContainer<int, 8> container = { 23, 1, 5, 9, 5, 23, 44, 19 };
    Set<int, StatefulCompare<int>, MyAllocator> set(
        container.GetBegin(), container.GetEnd(),
        StatefulCompare<int>{true}, MyAllocator(231));

    Set<int, StatefulCompare<int>, MyAllocator> copy = set;

    EXPECT_EQ(set.Size(), copy.Size());
    EXPECT_EQ(copy.GetAllocator(), set.GetAllocator());

    for (auto iter1 = set.GetBegin(), iter2 = copy.GetBegin(); iter1 != set.GetEnd();
         ++iter1, ++iter2)
    {
        EXPECT_EQ(*iter1, *iter2);
    }
}

TEST(SetTests, MoveConstructor)
{
    ForwardTestContainer<int, 8> container = { 23, 1, 5, 9, 5, 23, 44, 19 };
    int expected[6] = { 44, 23, 19, 9, 5, 1 };

    Set<int, StatefulCompare<int>, MyAllocator> set(
        container.GetBegin(), container.GetEnd(),
        StatefulCompare<int>{true}, MyAllocator(231));

    Set<int, StatefulCompare<int>, MyAllocator> moved = Move(set);

    EXPECT_EQ(set.Size(), 0);

    EXPECT_EQ(moved.Size(), 6);
    EXPECT_EQ(moved.GetAllocator().Value, 231);

    Index index = 0;
    for (auto iter1 = moved.GetBegin(); iter1 != moved.GetEnd(); ++iter1)
    {
        EXPECT_EQ(*iter1, expected[index]);
        ++index;
    }
}

TEST(SetTests, InitListCompareAllocatorConstructor)
{
    int expected[4] = { 23, 9, 5, 1 };
    Set<int, StatefulCompare<int>, MyAllocator> set(
        { 23, 1, 5, 9, 5 },
        StatefulCompare<int>{true}, MyAllocator(231));

    EXPECT_EQ(set.Size(), 4);
    EXPECT_EQ(set.GetAllocator().Value, 231);

    Index index = 0;
    for (auto iter = set.GetBegin(); iter != set.GetEnd(); ++iter)
    {
        EXPECT_EQ(*iter, expected[index]);
        ++index;
    }
}

TEST(SetTests, InitListAllocatorConstructor)
{
    int expected[4] = { 1, 5, 9, 23 };
    Set<int, StatefulCompare<int>, MyAllocator> set({ 23, 1, 5, 9, 5 }, MyAllocator(231));

    EXPECT_EQ(set.Size(), 4);
    EXPECT_EQ(set.GetAllocator().Value, 231);

    Index index = 0;
    for (auto iter = set.GetBegin(); iter != set.GetEnd(); ++iter)
    {
        EXPECT_EQ(*iter, expected[index]);
        ++index;
    }
}

TEST(SetTests, Destructor)
{
    /* Can't test this. */
}

TEST(SetTests, CopyAssign)
{
    ForwardTestContainer<int, 8> container = { 23, 1, 5, 9, 5, 23, 44, 19 };
    Set<int, StatefulCompare<int>, MyAllocator> set(
        container.GetBegin(), container.GetEnd(),
        StatefulCompare<int>{true}, MyAllocator(231));

    Set<int, StatefulCompare<int>, MyAllocator> copy = { 34, 121, 11, 3, 85 };
    copy = set;

    EXPECT_EQ(set.Size(), copy.Size());
    EXPECT_EQ(copy.GetAllocator(), set.GetAllocator());

    for (auto iter1 = set.GetBegin(), iter2 = copy.GetBegin(); iter1 != set.GetEnd();
         ++iter1, ++iter2)
    {
        EXPECT_EQ(*iter1, *iter2);
    }
}

TEST(SetTests, MoveAssign)
{
    ForwardTestContainer<int, 8> container = { 23, 1, 5, 9, 5, 23, 44, 19 };
    int expected[6] = { 44, 23, 19, 9, 5, 1 };

    Set<int, StatefulCompare<int>, MyAllocator> set(
        container.GetBegin(), container.GetEnd(),
        StatefulCompare<int>{true}, MyAllocator(231));

    Set<int, StatefulCompare<int>, MyAllocator> moved = { 83, 12, 45, 683, 293 };
    moved = Move(set);

    EXPECT_EQ(set.Size(), 0);

    EXPECT_EQ(moved.Size(), 6);
    EXPECT_EQ(moved.GetAllocator().Value, 231);

    Index index = 0;
    for (auto iter1 = moved.GetBegin(); iter1 != moved.GetEnd(); ++iter1)
    {
        EXPECT_EQ(*iter1, expected[index]);
        ++index;
    }
}

TEST(SetTests, InitListAssign)
{
    int expected[4] = { 1, 5, 9, 23 };
    Set<int, StatefulCompare<int>, MyAllocator> set({ 2384, 23, 12, 11 }, MyAllocator(231));

    set = { 23, 1, 5, 9, 5 };

    EXPECT_EQ(set.Size(), 4);
    EXPECT_EQ(set.GetAllocator().Value, 231);

    Index index = 0;
    for (auto iter = set.GetBegin(); iter != set.GetEnd(); ++iter)
    {
        EXPECT_EQ(*iter, expected[index]);
        ++index;
    }
}

TEST(SetTests, IsEmpty)
{
    Set<int> empty;
    Set<int> set = { 324, 1, 56, 12 };

    EXPECT_TRUE(empty.IsEmpty());
    EXPECT_FALSE(set.IsEmpty());
}

TEST(SetTests, Swap)
{
    Set<int, StatefulCompare<int>, MyAllocator> set1(
        { 23, 1287, 38, 595, 122, 38, 444 },
        StatefulCompare<int>(true), MyAllocator(12));

    Set<int, StatefulCompare<int>, MyAllocator> set2(
        { 23, 93, 1287, 123, 11 },
        StatefulCompare<int>(false), MyAllocator(4444));

    set1.Swap(set2);

    EXPECT_EQ(set1.Size(), 5);
    EXPECT_EQ(set2.Size(), 6);

    EXPECT_EQ(set1.GetAllocator().Value, 4444);
    EXPECT_EQ(set2.GetAllocator().Value, 12);

    int expected1[5] = { 11, 23, 93, 123, 1287 };
    Index index1 = 0;

    for (auto iter = set1.GetBegin(); iter != set1.GetEnd(); ++iter, ++index1)
        EXPECT_EQ(*iter, expected1[index1]);

    int expected2[6] = { 1287, 595, 444, 122, 38, 23 };
    Index index2 = 0;

    for (auto iter = set2.GetBegin(); iter != set2.GetEnd(); ++iter, ++index2)
        EXPECT_EQ(*iter, expected2[index2]);
}

TEST(SetTests, Clear)
{
    Set<int> set = { 932, 129, 54, 12, 3293 };
    set.Clear();

    EXPECT_EQ(set.Size(), 0);
    EXPECT_EQ(set.GetBegin(), set.GetEnd());
}

TEST(SetTests, InsertCopy)
{
    Set<int> set = { 23, 53, 9812, 4942 };
    int value = 341;

    set.Insert(value);

    int expected[5] = { 23, 53, 341, 4942, 9812 };
    Index index = 0;

    EXPECT_EQ(set.Size(), 5);
    for (auto iter = set.GetBegin(); iter != set.GetEnd(); ++iter, ++index)
        EXPECT_EQ(*iter, expected[index]);
}

TEST(SetTests, InsertMove)
{
    Set<int> set = { 23, 53, 9812, 4942 };
    set.Insert(341);

    int expected[5] = { 23, 53, 341, 4942, 9812 };
    Index index = 0;

    EXPECT_EQ(set.Size(), 5);
    for (auto iter = set.GetBegin(); iter != set.GetEnd(); ++iter, ++index)
        EXPECT_EQ(*iter, expected[index]);
}

TEST(SetTests, InsertRange)
{
    Set<int> set = { 23, 53, 9812, 4942 };
    ForwardTestContainer<int, 5> container = { 23, 86, 9332, 2344, 94 };

    set.Insert(container.GetBegin(), container.GetEnd());

    int expected[8] = { 23, 53, 86, 94, 2344, 4942, 9332, 9812 };
    Index index = 0;

    EXPECT_EQ(set.Size(), 8);
    for (auto iter = set.GetBegin(); iter != set.GetEnd(); ++iter, ++index)
        EXPECT_EQ(*iter, expected[index]);
}

TEST(SetTests, InsertInitList)
{
    Set<int> set = { 23, 53, 9812, 4942 };
    set.Insert({ 23, 86, 9332, 2344, 94 });

    int expected[8] = { 23, 53, 86, 94, 2344, 4942, 9332, 9812 };
    Index index = 0;

    EXPECT_EQ(set.Size(), 8);
    for (auto iter = set.GetBegin(); iter != set.GetEnd(); ++iter, ++index)
        EXPECT_EQ(*iter, expected[index]);
}
