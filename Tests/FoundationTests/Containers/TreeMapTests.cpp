#include <gtest/gtest.h>
#include "TestContainer.h"
#include "FlippableCompare.h"

#include "StatefulAllocator.h"
#include "TrackingAllocator.h"

#include "Foundation/Concepts/Container.h"
#include "Foundation/Algorithms/Advance.h"

#include "Foundation/Containers/TreeMap.h"

namespace
{
    using namespace Kitsune;
    using Testing::ForwardTestContainer, Testing::StatefulAllocator,
          Testing::TrackingAllocator, Testing::FlippableCompare;

    static_assert(Kitsune::ForwardIterator<TreeMap<int, float>::Iterator>,
                  "TreeMap<T, U>'s iterator doesn't satisfy ForwardIterator.");

    static_assert(Kitsune::ForwardIterator<TreeMap<int, long>::ConstIterator>,
                  "TreeMap<T, U>'s iterator doesn't satisfy ForwardIterator.");

    static_assert(
        Container<TreeMap<float, int>>,
        "TreeMap<T, U> does not satisfy the requirements of the Container concept.");

    // TreeMap<T, U, Comp, Alloc>::TreeMap()
    TEST(TreeMapTest, DefaultConstructor)
    {
        TreeMap<int, short, FlippableCompare<int>, StatefulAllocator> map;

        EXPECT_EQ(map.Size(), 0);
        EXPECT_EQ(map.GetAllocator().GetId(), 0);
        EXPECT_EQ(map.GetCompare(), FlippableCompare<int>());
    }

    // TreeMap<T, U, Comp, Alloc>::TreeMap()
    TEST(TreeMapTest, DefaultConstructorDoesNotAllocate)
    {
        TreeMap<int, short, FlippableCompare<int>, TrackingAllocator> map;
        EXPECT_EQ(map.GetAllocator().AllocationCount(), 0);
    }

    // TreeMap<T, U, Comp, Alloc>::TreeMap(const Comp&, const Alloc&)
    TEST(TreeMapTest, CompareAndAllocatorConstructor)
    {
        TreeMap<int, short, FlippableCompare<int>, StatefulAllocator> map(
            FlippableCompare<int>(true),
            StatefulAllocator(43));

        EXPECT_EQ(map.Size(), 0);
        EXPECT_EQ(map.GetAllocator().GetId(), 43);
        EXPECT_EQ(map.GetCompare(), FlippableCompare<int>(true));
    }

    // TreeMap<T, U, Comp, Alloc>::TreeMap(const Comp&, const Alloc&)
    TEST(TreeMapTest, CompareAndAllocatorConstructorDoesNotAllocate)
    {
        TreeMap<int, long, FlippableCompare<int>, TrackingAllocator> map(
            FlippableCompare<int>(true));

        EXPECT_EQ(map.GetAllocator().AllocationCount(), 0);
    }

    // TreeMap<T, U, Comp, Alloc>::TreeMap(const Alloc&)
    TEST(TreeMapTest, AllocatorConstructor)
    {
        TreeMap<int, long, FlippableCompare<int>, StatefulAllocator> map(
            StatefulAllocator(43));

        EXPECT_EQ(map.Size(), 0);
        EXPECT_EQ(map.GetAllocator().GetId(), 43);
        EXPECT_EQ(map.GetCompare(), FlippableCompare<int>());
    }

    // TreeMap<T, U, Comp, Alloc>::TreeMap(const Alloc&)
    TEST(TreeMapTest, AllocatorConstructorDoesNotAllocate)
    {
        TreeMap<int, long, FlippableCompare<int>, TrackingAllocator> map(
            TrackingAllocator{});

        EXPECT_EQ(map.GetAllocator().AllocationCount(), 0);
    }

    // TreeMap<T, U, Comp, Alloc>::TreeMap(Iter, Iter, const Comp&, const Alloc&)
    TEST(TreeMapTest, RangeWithCompareConstructor)
    {
        ForwardTestContainer<Pair<int, int>, 5> container = {
            Pair<int, int>{ 23, 14 },
            Pair<int, int>{ 1, 23 },
            Pair<int, int>{ 5, 9 },
            Pair<int, int>{ 9, 4 },
            Pair<int, int>{ 5, 4 }};

        TreeMap<int, int, FlippableCompare<int>, StatefulAllocator> map(
            container.GetBegin(),
            container.GetEnd(),
            FlippableCompare<int>(true),
            StatefulAllocator(2341));

        EXPECT_EQ(map.Size(), 4);
        EXPECT_EQ(map.GetAllocator().GetId(), 2341);
        EXPECT_EQ(map.GetCompare(), FlippableCompare<int>(true));

        std::vector<Pair<int, int>> expected = {
            { 23, 14 },
            { 9, 4 },
            { 5, 9 },
            { 1, 23 }
        };

        Index index = 0;
        for (auto iter = map.GetBegin(); iter != map.GetEnd(); ++iter, ++index)
            EXPECT_EQ(*iter, expected[index]);
    }

    // TreeMap<T, U, Comp, Alloc>::TreeMap(Iter, Iter, const Alloc&)
    TEST(TreeMapTest, RangeWithAllocatorOnlyConstructor)
    {
        ForwardTestContainer<Pair<int, int>, 5> container = {
            Pair<int, int>{ 23, 14 },
            Pair<int, int>{ 1, 23 },
            Pair<int, int>{ 5, 9 },
            Pair<int, int>{ 9, 4 },
            Pair<int, int>{ 5, 4 }};

        TreeMap<int, int, FlippableCompare<int>, StatefulAllocator> map(
            container.GetBegin(),
            container.GetEnd(),
            StatefulAllocator(2341));

        EXPECT_EQ(map.Size(), 4);
        EXPECT_EQ(map.GetAllocator().GetId(), 2341);
        EXPECT_EQ(map.GetCompare(), FlippableCompare<int>(false));

        std::vector<Pair<int, int>> expected = {
            { 1, 23 },
            { 5, 9 },
            { 9, 4 },
            { 23, 14 }
        };

        Index index = 0;
        for (auto iter = map.GetBegin(); iter != map.GetEnd(); ++iter, ++index)
            EXPECT_EQ(*iter, expected[index]);
    }

    // TreeMap<T, U, Comp, Alloc>::TreeMap(
    //     std::initializer_list<Pair<T, U>>, const Comp&, const Alloc&)
    TEST(TreeMapTest, InitializerListWithCompareConstructor)
    {
        TreeMap<int, int, FlippableCompare<int>, StatefulAllocator> map(
            {
                { 23, 14 },
                { 1, 23 },
                { 5, 9 },
                { 9, 4 },
                { 5, 4 }
            },
            FlippableCompare<int>(true),
            StatefulAllocator(2341));

        EXPECT_EQ(map.Size(), 4);
        EXPECT_EQ(map.GetAllocator().GetId(), 2341);
        EXPECT_EQ(map.GetCompare(), FlippableCompare<int>(true));

        std::vector<Pair<int, int>> expected = {
            { 23, 14 },
            { 9, 4 },
            { 5, 9 },
            { 1, 23 }};

        Index index = 0;
        for (auto iter = map.GetBegin(); iter != map.GetEnd(); ++iter, ++index)
            EXPECT_EQ(*iter, expected[index]);
    }

    // TreeMap<T, U, Comp, Alloc>::TreeMap(
    //     std::initializer_list<Pair<T, U>>, const Alloc&)
    TEST(TreeMapTest, InitializerListWithAllocatorOnlyConstructor)
    {
        TreeMap<int, int, FlippableCompare<int>, StatefulAllocator> map(
            {
                { 23, 14 },
                { 1, 23 },
                { 5, 9 },
                { 9, 4 },
                { 5, 4 }
            },
            StatefulAllocator(2341));

        EXPECT_EQ(map.Size(), 4);
        EXPECT_EQ(map.GetAllocator().GetId(), 2341);
        EXPECT_EQ(map.GetCompare(), FlippableCompare<int>(false));

        std::vector<Pair<int, int>> expected = {
            Pair<int, int>{ 1, 23 },
            Pair<int, int>{ 5, 9 },
            Pair<int, int>{ 9, 4 },
            Pair<int, int>{ 23, 14 }};

        Index index = 0;
        for (auto iter = map.GetBegin(); iter != map.GetEnd(); ++iter, ++index)
            EXPECT_EQ(*iter, expected[index]);
    }

    // TreeMap<T, U, Comp, Alloc>::TreeMap(const TreeMap&)
    TEST(TreeMapTest, CopyConstructor)
    {
        TreeMap<std::string, std::string, FlippableCompare<std::string>,
                StatefulAllocator> map(
            {
                { "Hello", "World" },
                { "Lorem", "ipsum" },
                { "dolor", "sit" },
                { "amet", "consectetur" }
            },
            FlippableCompare<std::string>(true),
            StatefulAllocator(2341));

        // NOLINTNEXTLINE(performance-unnecessary-copy-initialization)
        TreeMap<std::string, std::string, FlippableCompare<std::string>,
                StatefulAllocator> copy(map);

        EXPECT_EQ(map.GetAllocator().GetId(), 2341);
        EXPECT_EQ(copy.GetAllocator().GetId(), 2341);

        EXPECT_TRUE(map.GetCompare().IsFlipped());
        EXPECT_TRUE(copy.GetCompare().IsFlipped());

        EXPECT_EQ(map.Size(), 4);
        EXPECT_EQ(copy.Size(), 4);

        std::vector<Pair<std::string, std::string>> expected = {
            { "dolor", "sit" },
            { "amet", "consectetur" },
            { "Lorem", "ipsum" },
            { "Hello", "World" },
        };

        int index = 0;
        for (auto iter = map.GetBegin(), copyIter = copy.GetBegin();
             iter != map.GetEnd();
             ++iter, ++copyIter, ++index)
        {
            EXPECT_EQ(*iter, expected[index]);
            EXPECT_EQ(*copyIter, expected[index]);
        }
    }

    // TreeMap<T, U, Comp, Alloc>::TreeMap(TreeMap&&)
    TEST(TreeMapTest, MoveConstructor)
    {
        TreeMap<std::string, std::string, FlippableCompare<std::string>,
                StatefulAllocator> map(
            {
                { "Hello", "World" },
                { "Lorem", "ipsum" },
                { "dolor", "sit" },
                { "amet", "consectetur" }
            },
            FlippableCompare<std::string>(true),
            StatefulAllocator(2341));

        TreeMap<std::string, std::string, FlippableCompare<std::string>,
                StatefulAllocator> move(std::move(map));

        EXPECT_EQ(map.GetAllocator().GetId(), 0);
        EXPECT_EQ(move.GetAllocator().GetId(), 2341);

        EXPECT_FALSE(map.GetCompare().IsFlipped());
        EXPECT_TRUE(move.GetCompare().IsFlipped());

        EXPECT_EQ(map.Size(), 0);
        EXPECT_EQ(move.Size(), 4);

        std::vector<Pair<std::string, std::string>> expected = {
            { "dolor", "sit" },
            { "amet", "consectetur" },
            { "Lorem", "ipsum" },
            { "Hello", "World" },
        };

        int index = 0;
        for (auto iter = move.GetBegin(); iter != move.GetEnd(); ++iter, ++index)
            EXPECT_EQ(*iter, expected[index]);
    }

    // TreeMap<T, U, Comp, Alloc>::~TreeMap()
    TEST(TreeMapTest, Destructor)
    {
        // TODO: Find a way to test the destructor.
        EXPECT_TRUE(true);
    }

    // TreeMap<T, U, Comp, Alloc>::operator=(const TreeMap&)
    TEST(TreeMapTest, CopyAssign)
    {
        TreeMap<std::string, std::string, FlippableCompare<std::string>,
                StatefulAllocator> map(
            {
                { "Hello", "World" },
                { "Lorem", "ipsum" },
                { "dolor", "sit" },
                { "amet", "consectetur" }
            },
            FlippableCompare<std::string>(true),
            StatefulAllocator(2341));

        // NOLINTNEXTLINE(performance-unnecessary-copy-initialization)
        TreeMap<std::string, std::string, FlippableCompare<std::string>,
                StatefulAllocator> copy(
            {
                { "sed", "do" },
                { "eiusmod", "tempor" },
                { "incididunt", "ut" },
                { "labore", "et" },
                { "dolore", "magna" }
            },
            FlippableCompare<std::string>(false),
            StatefulAllocator(544));

        copy = map;

        EXPECT_EQ(map.GetAllocator().GetId(), 2341);
        EXPECT_EQ(copy.GetAllocator().GetId(), 2341);

        EXPECT_TRUE(map.GetCompare().IsFlipped());
        EXPECT_TRUE(copy.GetCompare().IsFlipped());

        EXPECT_EQ(map.Size(), 4);
        EXPECT_EQ(copy.Size(), 4);

        std::vector<Pair<std::string, std::string>> expected = {
            { "dolor", "sit" },
            { "amet", "consectetur" },
            { "Lorem", "ipsum" },
            { "Hello", "World" },
        };

        int index = 0;
        for (auto iter = map.GetBegin(), copyIter = copy.GetBegin();
             iter != map.GetEnd();
             ++iter, ++copyIter, ++index)
        {
            EXPECT_EQ(*iter, expected[index]);
            EXPECT_EQ(*copyIter, expected[index]);
        }
    }

    // TreeMap<T, U, Comp, Alloc>::operator=(TreeMap&&)
    TEST(TreeMapTest, MoveAssign)
    {
        TreeMap<std::string, std::string, FlippableCompare<std::string>,
                StatefulAllocator> map(
            {
                { "dolor", "sit" },
                { "Hello", "World" },
                { "Lorem", "ipsum" },
                { "amet", "consectetur" }
            },
            FlippableCompare<std::string>(true),
            StatefulAllocator(2341));

        TreeMap<std::string, std::string, FlippableCompare<std::string>,
                StatefulAllocator> move(
            {
                { "sed", "do" },
                { "eiusmod", "tempor" },
                { "incididunt", "ut" },
                { "labore", "et" },
                { "dolore", "magna" }
            },
            FlippableCompare<std::string>(false),
            StatefulAllocator(544));

        move = std::move(map);

        EXPECT_EQ(map.GetAllocator().GetId(), 0);
        EXPECT_EQ(move.GetAllocator().GetId(), 2341);

        EXPECT_FALSE(map.GetCompare().IsFlipped());
        EXPECT_TRUE(move.GetCompare().IsFlipped());

        EXPECT_EQ(map.Size(), 0);
        EXPECT_EQ(move.Size(), 4);

        std::vector<Pair<std::string, std::string>> expected = {
            { "dolor", "sit" },
            { "amet", "consectetur" },
            { "Lorem", "ipsum" },
            { "Hello", "World" },
        };

        int index = 0;
        for (auto iter = move.GetBegin(); iter != move.GetEnd(); ++iter, ++index)
            EXPECT_EQ(*iter, expected[index]);
    }

    // TreeMap<T, U, Comp, Alloc>::operator=(
    //     std::initializer_list<Pair<T, U>>, const Comp&, const Alloc&)
    TEST(TreeMapTest, InitializerListAssign)
    {
        TreeMap<int, int, FlippableCompare<int>, StatefulAllocator> map({
                { 4, 9 },
                { 23, 15 },
                { 1, 4 },
                { 5, 4 }},
            FlippableCompare<int>(true),
            StatefulAllocator(2341));

        map = {
            { 23, 14 },
            { 1, 23 },
            { 5, 9 },
            { 9, 4 },
            { 5, 4 }};

        EXPECT_EQ(map.Size(), 4);
        EXPECT_EQ(map.GetAllocator().GetId(), 2341);
        EXPECT_EQ(map.GetCompare(), FlippableCompare<int>(true));

        std::vector<Pair<int, int>> expected = {
            { 23, 14 },
            { 9, 4 },
            { 5, 9 },
            { 1, 23 }};

        Index index = 0;
        for (auto iter = map.GetBegin(); iter != map.GetEnd(); ++iter, ++index)
            EXPECT_EQ(*iter, expected[index]);
    }

    // TreeMap<T, U, Comp, Alloc>::operator[](const Key&)
    TEST(TreeMapTest, Subscript)
    {
        TreeMap<int, int, FlippableCompare<int>> map = {
            { 32, 541 },
            { 1, 54 },
            { 5, 432 },
            { 7, 12 },
            { 12, 9123 },
            { 1, 1 }
        };

        for (auto iter = map.GetBegin(); iter != map.GetEnd(); ++iter)
        {
            auto& [key, value] = *iter;
            EXPECT_EQ(&map[key], &value);
        }
    }

    // TreeMap<T, U, Comp, Alloc>::operator[](const Key&) const
    TEST(TreeMapTest, ConstSubscript)
    {
        const TreeMap<int, int, FlippableCompare<int>> map = {
            { 32, 541 },
            { 1, 54 },
            { 5, 432 },
            { 7, 12 },
            { 12, 9123 },
            { 1, 1 }
        };

        for (auto iter = map.GetBegin(); iter != map.GetEnd(); ++iter)
        {
            auto& [key, value] = *iter;
            EXPECT_EQ(&map[key], &value);
        }
    }

    // TreeMap<T, U, Comp, Alloc>::GetBegin()
    // TreeMap<T, U, Comp, Alloc>::GetEnd()
    TEST(TreeMapTest, Iterators)
    {
        TreeMap<int, int, FlippableCompare<int>> map = {
            { 32, 541 },
            { 1, 54 },
            { 5, 432 },
            { 7, 12 },
            { 12, 9123 },
            { 1, 1 }
        };

        ASSERT_EQ(map.Size(), 5);
        EXPECT_EQ(*map.GetBegin(), (Pair<int, int>{ 1, 54 }));

        /* TreeMap<T, U, Comp, Alloc>::GetEnd()'s iterator value is undefined, and
         * therefore cannot be tested.
         */
    }

    // TreeMap<T, U, Comp, Alloc>::IsEmpty()
    TEST(TreeMapTest, IsEmpty)
    {
        TreeMap<int, int> map = {{ 3, 9 }, { 1, 2 }, { 1, 2 }};
        TreeMap<int, int> empty;

        ASSERT_EQ(map.Size(), 2);
        ASSERT_EQ(empty.Size(), 0);

        EXPECT_FALSE(map.IsEmpty());
        EXPECT_TRUE(empty.IsEmpty());
    }

    /* TreeMap<T, U, Comp, Alloc>::Size() and TreeMap<T, U, Comp, Alloc>::GetAllocator() is
     * assumed to work. No tests.
     */

    // TreeMap<T, U, Comp, Alloc>::Clear()
    TEST(TreeMapTest, Clear)
    {
        TreeMap<int, int, LessFunctor<int>, TrackingAllocator> map = {
            { 3, 3 },
            { 1, 23 },
            { 12, 3 },
            { 1, 0 }
        };

        ASSERT_GT(map.GetAllocator().AllocationSize(), 0);
        ASSERT_GT(map.Size(), 0);

        map.Clear();

        EXPECT_EQ(map.Size(), 0);
        EXPECT_EQ(map.GetAllocator().AllocationSize(), 0);
    }

    // TreeMap<T, U, Comp, Alloc>::Swap(TreeMap&)
    TEST(TreeSetTest, Swap)
    {
        TreeMap<int, int, FlippableCompare<int>, StatefulAllocator> map1(
            {
                { 23, 4 },
                { 1287, 43 },
                { 38, 94 },
                { 595, 1 },
                { 122, 65 },
                { 38, 954 },
                { 444, 0 }
            },
            FlippableCompare<int>(true),
            StatefulAllocator(12));

        TreeMap<int, int, FlippableCompare<int>, StatefulAllocator> map2(
            {
                { 23, 94 },
                { 93, 443 },
                { 1287, 84 },
                { 123, 21 },
                { 11, 65 },
            },
            FlippableCompare<int>(false),
            StatefulAllocator(4444));

        map1.Swap(map2);

        EXPECT_EQ(map1.Size(), 5);
        EXPECT_EQ(map2.Size(), 6);

        EXPECT_EQ(map1.GetAllocator().GetId(), 4444);
        EXPECT_EQ(map2.GetAllocator().GetId(), 12);

        std::vector<Pair<int, int>> expected1 = {
            { 11, 65 },
            { 23, 94 },
            { 93, 443 },
            { 123, 21 },
            { 1287, 84 },
        };

        Index index1 = 0;
        for (auto iter = map1.GetBegin(); iter != map1.GetEnd(); ++iter, ++index1)
            EXPECT_EQ(*iter, expected1[index1]);

        std::vector<Pair<int, int>> expected2 = {
            { 1287, 43 },
            { 595, 1 },
            { 444, 0 },
            { 122, 65 },
            { 38, 94 },
            { 23, 4 },
        };

        Index index2 = 0;
        for (auto iter = map2.GetBegin(); iter != map2.GetEnd(); ++iter, ++index2)
            EXPECT_EQ(*iter, expected2[index2]);
    }

    // TreeMap<T, U, Comp, Alloc>::Emplace(Args&&...)
    TEST(TreeMapTest, Emplace)
    {
        TreeMap<std::string, std::string,
                LessFunctor<std::string>, TrackingAllocator> map = {
            { "Hello", "World" },
            { "Lorem", "ipsum" },
            { "dolor", "sit" },
            { "amet", "consectetur" }
        };

        auto [iterator, success] = map.Emplace("adipiscing", "elit");
        auto [key, value] = *iterator;

        EXPECT_EQ(key, "adipiscing");
        EXPECT_EQ(value, "elit");
        EXPECT_TRUE(success);

        auto [iterator2, success2] = map.Emplace("adipiscing", "elit");
        EXPECT_EQ(iterator2, iterator);
        EXPECT_FALSE(success2);

        std::vector<Pair<std::string, std::string>> expected = {
            { "Hello", "World" },
            { "Lorem", "ipsum" },
            { "adipiscing", "elit" },
            { "amet", "consectetur" },
            { "dolor", "sit" },
        };

        int index = 0;
        for (auto iter = map.GetBegin(); iter != map.GetEnd(); ++iter, ++index)
            EXPECT_EQ(*iter, expected[index]);
    }

    // TreeMap<T, U, Comp, Alloc>::Insert(const ValueType&)
    TEST(TreeMapTest, InsertCopy)
    {
        TreeMap<std::string, std::string,
                LessFunctor<std::string>, TrackingAllocator> map = {
            { "Hello", "World" },
            { "Lorem", "ipsum" },
            { "dolor", "sit" },
            { "amet", "consectetur" }
        };

        Pair<std::string, std::string> pair = { "adipiscing", "elit" };

        auto [iterator, success] = map.Insert(pair);
        auto [key, value] = *iterator;

        EXPECT_EQ(key, "adipiscing");
        EXPECT_EQ(value, "elit");
        EXPECT_TRUE(success);

        auto [iterator2, success2] = map.Insert(pair);
        EXPECT_EQ(iterator2, iterator);
        EXPECT_FALSE(success2);

        std::vector<Pair<std::string, std::string>> expected = {
            { "Hello", "World" },
            { "Lorem", "ipsum" },
            { "adipiscing", "elit" },
            { "amet", "consectetur" },
            { "dolor", "sit" },
        };

        int index = 0;
        for (auto iter = map.GetBegin(); iter != map.GetEnd(); ++iter, ++index)
            EXPECT_EQ(*iter, expected[index]);
    }

    // TreeMap<T, U, Comp, Alloc>::Insert(ValueType&&)
    TEST(TreeMapTest, InsertMove)
    {
        TreeMap<std::string, std::string,
                LessFunctor<std::string>, TrackingAllocator> map = {
            { "Hello", "World" },
            { "Lorem", "ipsum" },
            { "dolor", "sit" },
            { "amet", "consectetur" }
        };

        auto [iterator, success] = map.Insert({ "adipiscing", "elit" });
        auto [key, value] = *iterator;

        EXPECT_EQ(key, "adipiscing");
        EXPECT_EQ(value, "elit");
        EXPECT_TRUE(success);

        auto [iterator2, success2] = map.Insert({ "adipiscing", "elit" });
        EXPECT_EQ(iterator2, iterator);
        EXPECT_FALSE(success2);

        std::vector<Pair<std::string, std::string>> expected = {
            { "Hello", "World" },
            { "Lorem", "ipsum" },
            { "adipiscing", "elit" },
            { "amet", "consectetur" },
            { "dolor", "sit" },
        };

        int index = 0;
        for (auto iter = map.GetBegin(); iter != map.GetEnd(); ++iter, ++index)
            EXPECT_EQ(*iter, expected[index]);
    }

    // TreeMap<T, U, Comp, Alloc>::Insert(Iter, Iter)
    TEST(TreeMapTest, InsertRange)
    {
        TreeMap<std::string, std::string,
                LessFunctor<std::string>, TrackingAllocator> map = {
            { "Hello", "World" },
            { "Lorem", "ipsum" },
            { "dolor", "sit" },
            { "amet", "consectetur" }
        };

        using StringPair = Pair<std::string, std::string>;
        ForwardTestContainer<StringPair, 4> container = {
            StringPair{ "sed", "do" },
            StringPair{ "eiusmod", "tempor" },
            StringPair{ "incididunt", "ut" },
            StringPair{ "labore", "et" },
        };

        map.Insert(container.GetBegin(), container.GetEnd());
        map.Insert(container.GetBegin(), container.GetEnd());       // Should do nothing.

        std::vector<Pair<std::string, std::string>> expected = {
            { "Hello", "World" },
            { "Lorem", "ipsum" },
            { "amet", "consectetur" },
            { "dolor", "sit" },
            { "eiusmod", "tempor" },
            { "incididunt", "ut" },
            { "labore", "et" },
            { "sed", "do" },
        };

        int index = 0;
        for (auto iter = map.GetBegin(); iter != map.GetEnd(); ++iter, ++index)
            EXPECT_EQ(*iter, expected[index]);
    }

    // TreeMap<T, U, Comp, Alloc>::Insert(std::initializer_list<ValueType>)
    TEST(TreeMapTest, InsertInitializerList)
    {
        TreeMap<std::string, std::string,
                LessFunctor<std::string>, TrackingAllocator> map = {
            { "Hello", "World" },
            { "Lorem", "ipsum" },
            { "dolor", "sit" },
            { "amet", "consectetur" }
        };

        map.Insert({
            { "sed", "do" },
            { "eiusmod", "tempor" },
            { "incididunt", "ut" },
            { "labore", "et" }
        });

        std::vector<Pair<std::string, std::string>> expected = {
            { "Hello", "World" },
            { "Lorem", "ipsum" },
            { "amet", "consectetur" },
            { "dolor", "sit" },
            { "eiusmod", "tempor" },
            { "incididunt", "ut" },
            { "labore", "et" },
            { "sed", "do" },
        };

        int index = 0;
        for (auto iter = map.GetBegin(); iter != map.GetEnd(); ++iter, ++index)
            EXPECT_EQ(*iter, expected[index]);
    }

    // TreeMap<T, U, Comp, Alloc>::InsertOrAssign(Key&&, Y&&)
    TEST(TreeMapTest, InsertOrAssignMoveKey)
    {
        TreeMap<std::string, std::string,
                LessFunctor<std::string>, TrackingAllocator> map = {
            { "Hello", "World" },
            { "Lorem", "ipsum" },
            { "dolor", "sit" },
            { "amet", "consectetur" }
        };

        auto [iterator, success] = map.InsertOrAssign("adipiscing", "not elit");
        auto [key, value] = *iterator;

        EXPECT_EQ(key, "adipiscing");
        EXPECT_EQ(value, "not elit");
        EXPECT_TRUE(success);

        auto [iterator2, success2] = map.InsertOrAssign("adipiscing", "elit");
        auto [key2, value2] = *iterator;

        EXPECT_EQ(iterator2, iterator);
        EXPECT_EQ(key2, "adipiscing");
        EXPECT_EQ(value2, "elit");
        EXPECT_FALSE(success2);

        std::vector<Pair<std::string, std::string>> expected = {
            { "Hello", "World" },
            { "Lorem", "ipsum" },
            { "adipiscing", "elit" },
            { "amet", "consectetur" },
            { "dolor", "sit" },
        };

        int index = 0;
        for (auto iter = map.GetBegin(); iter != map.GetEnd(); ++iter, ++index)
            EXPECT_EQ(*iter, expected[index]);
    }

    // TreeMap<T, U, Comp, Alloc>::InsertOrAssign(const Key&, Y&&)
    TEST(TreeMapTest, InsertOrAssignCopyKey)
    {
        TreeMap<std::string, std::string,
                LessFunctor<std::string>, TrackingAllocator> map = {
            { "Hello", "World" },
            { "Lorem", "ipsum" },
            { "dolor", "sit" },
            { "amet", "consectetur" }
        };

        std::string string = "adipiscing";

        auto [iterator, success] = map.InsertOrAssign(string, "not elit");
        auto [key, value] = *iterator;

        EXPECT_EQ(key, "adipiscing");
        EXPECT_EQ(value, "not elit");
        EXPECT_TRUE(success);

        auto [iterator2, success2] = map.InsertOrAssign(string, "elit");
        auto [key2, value2] = *iterator;

        EXPECT_EQ(iterator2, iterator);
        EXPECT_EQ(key2, "adipiscing");
        EXPECT_EQ(value2, "elit");
        EXPECT_FALSE(success2);

        std::vector<Pair<std::string, std::string>> expected = {
            { "Hello", "World" },
            { "Lorem", "ipsum" },
            { "adipiscing", "elit" },
            { "amet", "consectetur" },
            { "dolor", "sit" },
        };

        int index = 0;
        for (auto iter = map.GetBegin(); iter != map.GetEnd(); ++iter, ++index)
            EXPECT_EQ(*iter, expected[index]);
    }

    // TreeMap<T, U, Comp, Alloc>::Contains(const T&)
    TEST(TreeMapTest, Contains)
    {
        TreeMap<std::string, std::string,
                LessFunctor<std::string>, TrackingAllocator> map = {
            { "Hello", "World" },
            { "Lorem", "ipsum" },
            { "dolor", "sit" },
            { "amet", "consectetur" }
        };

        EXPECT_FALSE(map.Contains("hello"));
        EXPECT_FALSE(map.Contains("lorem"));
        EXPECT_FALSE(map.Contains("Dolor"));
        EXPECT_FALSE(map.Contains("Amet"));

        EXPECT_TRUE(map.Contains("Hello"));
        EXPECT_TRUE(map.Contains("Lorem"));
        EXPECT_TRUE(map.Contains("dolor"));
        EXPECT_TRUE(map.Contains("amet"));
    }

    // TreeMap<T, U, Comp, Alloc>::Find(const T&)
    TEST(TreeMapTest, Find)
    {
        TreeMap<std::string, std::string,
                LessFunctor<std::string>, TrackingAllocator> map = {
            { "Hello", "World" },
            { "Lorem", "ipsum" },
            { "dolor", "sit" },
            { "amet", "consectetur" }
        };

        auto iter = map.Find("Hello");
        EXPECT_NE(iter, map.GetEnd());
        EXPECT_EQ(*iter, (Pair<std::string, std::string>("Hello", "World")));

        auto notFoundIter = map.Find("hello");
        EXPECT_EQ(notFoundIter, map.GetEnd());
    }

    // TreeMap<T, U, Comp, Alloc>::Find(const T&) const
    TEST(TreeMapTest, FindConst)
    {
        const TreeMap<std::string, std::string, LessFunctor<std::string>,
                      TrackingAllocator> map = {
            { "Hello", "World" },
            { "Lorem", "ipsum" },
            { "dolor", "sit" },
            { "amet", "consectetur" }
        };

        auto iter = map.Find("Hello");
        EXPECT_NE(iter, map.GetEnd());
        EXPECT_EQ(*iter, (Pair<std::string, std::string>("Hello", "World")));

        auto notFoundIter = map.Find("hello");
        EXPECT_EQ(notFoundIter, map.GetEnd());
    }

    // operator==(const TreeMap<T, U, Comp, Alloc>&, const TreeMap<T, U, Comp, Alloc>&)
    TEST(TreeMapTest, Equal)
    {
        const TreeMap<std::string, std::string> map = {
            { "Hello", "World" },
            { "Lorem", "ipsum" },
            { "dolor", "sit" },
            { "amet", "consectetur" }
        };

        const TreeMap<std::string, std::string> equal = {
            { "Hello", "World" },
            { "Lorem", "ipsum" },
            { "dolor", "sit" },
            { "amet", "consectetur" }
        };

        const TreeMap<std::string, std::string> notEqual = {
            { "World", "World" },
            { "Lorem", "ipsum" },
            { "dolor", "sit" },
            { "amet", "consectetur" }
        };

        EXPECT_EQ(map, equal);
        EXPECT_NE(map, notEqual);
    }

    // TreeMap<T, U, Comp, Alloc>::Iterator::operator*()
    // TreeMap<T, U, Comp, Alloc>::ConstIterator::operator*()
    TEST(TreeSetTest, IteratorDereference)
    {
        // The end user must not be able to modify the key, but be freely able to
        // modify the value.
        TreeMap<int, int> map = {
            { 32, 541 },
            { 1, 54 },
            { 5, 432 },
            { 7, 12 },
            { 12, 9123 },
            { 1, 1 }
        };

        const TreeMap<int, int> constMap = {
            { 32, 541 },
            { 1, 54 },
            { 5, 432 },
            { 7, 12 },
            { 12, 9123 },
            { 1, 1 }
        };

        auto& [key, value] = *map.GetBegin();
        EXPECT_TRUE((std::is_same_v<decltype(key), const int>));
        EXPECT_TRUE((std::is_same_v<decltype(value), int>));

        auto& [constKey, constValue] = *constMap.GetBegin();
        EXPECT_TRUE((std::is_same_v<decltype(constKey), const int>));
        EXPECT_TRUE((std::is_same_v<decltype(constValue), const int>));
    }

    // TreeMap<T, U, Comp, Alloc>::Iterator::operator->()
    // TreeMap<T, U, Comp, Alloc>::ConstIterator::operator->()
    TEST(TreeSetTest, IteratorArrowOperator)
    {
        // The end user must not be able to modify the key, but be freely able to
        // modify the value.
        TreeMap<int, int> map = {
            { 32, 541 },
            { 1, 54 },
            { 5, 432 },
            { 7, 12 },
            { 12, 9123 },
            { 1, 1 }
        };

        const TreeMap<int, int> constMap = {
            { 32, 541 },
            { 1, 54 },
            { 5, 432 },
            { 7, 12 },
            { 12, 9123 },
            { 1, 1 }
        };

        auto* value = map.GetBegin().operator->();
        EXPECT_TRUE((std::is_same_v<decltype(value), int*>));

        auto* constValue = constMap.GetBegin().operator->();
        EXPECT_TRUE((std::is_same_v<decltype(constValue), const int*>));
    }

    // TreeMap<T, U, Comp, Alloc>::Iterator::operator++()
    // TreeMap<T, U, Comp, Alloc>::ConstIterator::operator++(int)
    TEST(TreeSetTest, IteratorIncrement)
    {
        // The end user must not be able to modify the key, but be freely able to
        // modify the value.
        TreeMap<int, int> map = {
            { 32, 541 },
            { 1, 54 },
            { 5, 432 },
            { 7, 12 },
            { 12, 9123 },
            { 1, 1 }
        };

        auto iter = map.GetBegin();
        EXPECT_EQ(*iter++, (Pair<int, int>(1, 54)));
        EXPECT_EQ(*iter, (Pair<int, int>(5, 432)));

        EXPECT_EQ(*++iter, (Pair<int, int>(7, 12)));
        EXPECT_EQ(*iter, (Pair<int, int>(7, 12)));
    }

    // operator==(
    //     const TreeMap<T, U, Comp, Alloc>::[Const]Iterator&,
    //     const TreeMap<T, U, Comp, Alloc>::[Const]Iterator&)
    TEST(TreeSetTest, IteratorEqual)
    {
        TreeMap<int, int> map = {
            { 8, 3 },
            { 19, 5 },
            { 10, 43 },
            { 68, 4 },
            { 4, 12 }
        };

        auto iter = map.GetBegin();
        auto sameIter = map.GetBegin();

        auto differentIter = map.GetBegin();
        Algorithms::Advance(differentIter, 3);

        EXPECT_EQ(iter, sameIter);
        EXPECT_NE(iter, differentIter);
    }
}
