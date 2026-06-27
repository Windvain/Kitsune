#include <gtest/gtest.h>
#include <algorithm>

#include "TestContainer.h"
#include "TrackingAllocator.h"
#include "StatefulAllocator.h"

#include "Foundation/Concepts/Container.h"
#include "Foundation/Containers/TreeSet.h"

#include "Foundation/Algorithms/Advance.h"

namespace
{
    using namespace Kitsune;
    using Testing::ForwardTestContainer, Testing::StatefulAllocator,
          Testing::TrackingAllocator;

    static_assert(Kitsune::ForwardIterator<TreeSet<int>::Iterator>,
                  "TreeSet<T>'s iterator doesn't satisfy ForwardIterator.");

    static_assert(Kitsune::ForwardIterator<TreeSet<int>::ConstIterator>,
                  "TreeSet<T>'s iterator doesn't satisfy ForwardIterator.");

    static_assert(
        Container<TreeSet<int>>,
        "TreeSet<T> does not satisfy the requirements of the Container concept.");

    template<typename T>
    class FlippableCompare
    {
    public:
        inline explicit FlippableCompare(bool greaterThan = false)
            : m_Flip(greaterThan)
        {
        }

        inline bool operator()(const T& lhs, const T& rhs) const
        {
            return m_Flip ? (lhs > rhs) : (lhs < rhs);
        };

    private:
        bool m_Flip;
    };

    template<typename T>
    class FlippableCompare<std::shared_ptr<T>>
    {
    public:
        inline explicit FlippableCompare(bool greaterThan = false)
            : m_Flip(greaterThan)
        {
        }

        inline bool operator()(const std::shared_ptr<T>& lhs,
                               const std::shared_ptr<T>& rhs)
        {
            return m_Flip ? (*lhs > *rhs) : (*lhs < *rhs);
        };

    private:
        bool m_Flip;
    };

    // TreeSet<T, Comp, Alloc>::TreeSet()
    TEST(TreeSetTest, DefaultConstructor)
    {
        TreeSet<int, FlippableCompare<int>, StatefulAllocator> treeSet;

        EXPECT_EQ(treeSet.Size(), 0);
        EXPECT_EQ(treeSet.GetAllocator().GetId(), 0);

        treeSet.Insert(2);
        treeSet.Insert(3);
        ASSERT_EQ(treeSet.Size(), 2);

        EXPECT_EQ(*treeSet.GetBegin(), 2);
    }

    // TreeSet<T, Comp, Alloc>::TreeSet()
    TEST(TreeSetTest, DefaultConstructorDoesNotAllocate)
    {
        TreeSet<int, FlippableCompare<int>, TrackingAllocator> treeSet;
        EXPECT_EQ(treeSet.GetAllocator().AllocationCount(), 0);
    }

    // TreeSet<T, Comp, Alloc>::TreeSet(const Comp&, const Alloc&)
    TEST(TreeSetTest, CompareAndAllocatorConstructor)
    {
        TreeSet<int, FlippableCompare<int>, StatefulAllocator> treeSet(
            FlippableCompare<int>(true),
            StatefulAllocator(43));

        EXPECT_EQ(treeSet.Size(), 0);
        EXPECT_EQ(treeSet.GetAllocator().GetId(), 43);

        treeSet.Insert(2);
        treeSet.Insert(3);
        ASSERT_EQ(treeSet.Size(), 2);

        EXPECT_EQ(*treeSet.GetBegin(), 3);
    }

    // TreeSet<T, Comp, Alloc>::TreeSet(const Comp&, const Alloc&)
    TEST(TreeSetTest, CompareAndAllocatorConstructorDoesNotAllocate)
    {
        TreeSet<int, FlippableCompare<int>, TrackingAllocator> treeSet(
            FlippableCompare<int>(true));

        EXPECT_EQ(treeSet.GetAllocator().AllocationCount(), 0);
    }

    // TreeSet<T, Comp, Alloc>::TreeSet(const Alloc&)
    TEST(TreeSetTest, AllocatorConstructor)
    {
        TreeSet<int, FlippableCompare<int>, StatefulAllocator> treeSet(
            StatefulAllocator(43));

        EXPECT_EQ(treeSet.Size(), 0);
        EXPECT_EQ(treeSet.GetAllocator().GetId(), 43);

        treeSet.Insert(2);
        treeSet.Insert(3);
        ASSERT_EQ(treeSet.Size(), 2);

        EXPECT_EQ(*treeSet.GetBegin(), 2);
    }

    // TreeSet<T, Comp, Alloc>::TreeSet(const Alloc&)
    TEST(TreeSetTest, AllocatorConstructorDoesNotAllocate)
    {
        TreeSet<int, FlippableCompare<int>, TrackingAllocator> treeSet(
            TrackingAllocator{ /* ... */ });

        EXPECT_EQ(treeSet.GetAllocator().AllocationCount(), 0);
    }

    // TreeSet<T, Comp, Alloc>::TreeSet(Iter, Iter, const Comp&, const Alloc&)
    TEST(TreeSetTest, RangeWithCompareConstructor)
    {
        ForwardTestContainer<int, 5> container = { 23, 1, 5, 9, 5 };
        TreeSet<int, FlippableCompare<int>, StatefulAllocator> treeSet(
            container.GetBegin(), container.GetEnd(),
            FlippableCompare<int>(true),
            StatefulAllocator(2341));

        EXPECT_EQ(treeSet.Size(), 4);
        EXPECT_EQ(treeSet.GetAllocator().GetId(), 2341);

        std::vector<int> expected = { 23, 9, 5, 1 };
        Index index = 0;

        for (auto iter = treeSet.GetBegin(); iter != treeSet.GetEnd(); ++iter, ++index)
            EXPECT_EQ(*iter, expected[index]);
    }

    // TreeSet<T, Comp, Alloc>::TreeSet(Iter, Iter, const Alloc&)
    TEST(TreeSetTest, RangeWithAllocatorOnlyConstructor)
    {
        ForwardTestContainer<int, 5> container = { 23, 1, 5, 9, 5 };
        TreeSet<int, FlippableCompare<int>, StatefulAllocator> treeSet(
            container.GetBegin(), container.GetEnd(),
            StatefulAllocator(2341));

        EXPECT_EQ(treeSet.Size(), 4);
        EXPECT_EQ(treeSet.GetAllocator().GetId(), 2341);

        std::vector<int> expected = { 1, 5, 9, 23 };
        Index index = 0;

        for (auto iter = treeSet.GetBegin(); iter != treeSet.GetEnd(); ++iter, ++index)
            EXPECT_EQ(*iter, expected[index]);
    }

    // TreeSet<T, Comp, Alloc>::TreeSet(
    //     std::initializer_list<T>, const Comp&, const Alloc&)
    TEST(TreeSetTest, InitializerListWithCompareConstructor)
    {
        TreeSet<int, FlippableCompare<int>, StatefulAllocator> treeSet(
            { 23, 1, 5, 9, 5 },
            FlippableCompare<int>(true),
            StatefulAllocator(2341));

        EXPECT_EQ(treeSet.Size(), 4);
        EXPECT_EQ(treeSet.GetAllocator().GetId(), 2341);

        std::vector<int> expected = { 23, 9, 5, 1 };
        Index index = 0;

        for (auto iter = treeSet.GetBegin(); iter != treeSet.GetEnd(); ++iter, ++index)
            EXPECT_EQ(*iter, expected[index]);
    }

    // TreeSet<T, Comp, Alloc>::TreeSet(std::initializer_list<T>, const Alloc&)
    TEST(TreeSetTest, InitializerListWithAllocatorOnlyConstructor)
    {
        TreeSet<int, FlippableCompare<int>, StatefulAllocator> treeSet(
            { 23, 1, 5, 9, 5 },
            StatefulAllocator(2341));

        EXPECT_EQ(treeSet.Size(), 4);
        EXPECT_EQ(treeSet.GetAllocator().GetId(), 2341);

        std::vector<int> expected = { 1, 5, 9, 23 };
        Index index = 0;

        for (auto iter = treeSet.GetBegin(); iter != treeSet.GetEnd(); ++iter, ++index)
            EXPECT_EQ(*iter, expected[index]);
    }

    // TreeSet<T, Comp, Alloc>::TreeSet(const TreeSet&)
    TEST(TreeSetTest, CopyConstructor)
    {
        TreeSet<std::shared_ptr<int>,
                FlippableCompare<std::shared_ptr<int>>,
                StatefulAllocator> treeSet(
            {
                std::make_shared<int>(23),
                std::make_shared<int>(1),
                std::make_shared<int>(5),
                std::make_shared<int>(9),
                std::make_shared<int>(5)
            },
            FlippableCompare<std::shared_ptr<int>>(true),
            StatefulAllocator(2341));

        // NOLINTNEXTLINE(performance-unnecessary-copy-initialization)
        TreeSet<std::shared_ptr<int>,
                FlippableCompare<std::shared_ptr<int>>,
                StatefulAllocator> copy(treeSet);

        EXPECT_EQ(copy.GetAllocator(), treeSet.GetAllocator());
        EXPECT_EQ(copy.GetAllocator().GetId(), 2341);

        EXPECT_EQ(treeSet.Size(), 4);
        EXPECT_EQ(copy.Size(), 4);

        std::vector<int> expected = { 23, 9, 5, 1 };
        auto iter = treeSet.GetBegin();
        auto copyIter = copy.GetBegin();

        for (int index = 0; index < expected.size(); ++index, ++iter, ++copyIter)
        {
            EXPECT_EQ(**iter, expected[index]);
            EXPECT_EQ(**copyIter, expected[index]);

            EXPECT_EQ(*iter, *copyIter);
        }
    }

    // TreeSet<T, Comp, Alloc>::TreeSet(TreeSet&&)
    TEST(TreeSetTest, MoveConstructor)
    {
        TreeSet<std::shared_ptr<int>,
                FlippableCompare<std::shared_ptr<int>>,
                StatefulAllocator> treeSet(
            {
                std::make_shared<int>(23),
                std::make_shared<int>(1),
                std::make_shared<int>(5),
                std::make_shared<int>(9),
                std::make_shared<int>(5)
            },
            FlippableCompare<std::shared_ptr<int>>(true),
            StatefulAllocator(2341));

        TreeSet<std::shared_ptr<int>,
                FlippableCompare<std::shared_ptr<int>>,
                StatefulAllocator> move(std::move(treeSet));

        EXPECT_EQ(treeSet.GetAllocator().GetId(), 0);
        EXPECT_EQ(move.GetAllocator().GetId(), 2341);

        EXPECT_EQ(treeSet.Size(), 0);
        EXPECT_EQ(move.Size(), 4);

        std::vector<int> expected = { 23, 9, 5, 1 };
        auto iter = move.GetBegin();

        for (int index = 0; index < expected.size(); ++index, ++iter)
        {
            EXPECT_EQ(**iter, expected[index]);
            EXPECT_EQ(iter->use_count(), 1);
        }
    }

    // TreeSet<T, Comp, Alloc>::~TreeSet()
    TEST(TreeSetTest, Destructor)
    {
        // TODO: Find a way to test the destructor.
        EXPECT_TRUE(true);
    }

    // TreeSet<T, Comp, Alloc>::operator=(const TreeSet&)
    TEST(TreeSetTest, CopyAssign)
    {
        TreeSet<std::shared_ptr<int>,
                FlippableCompare<std::shared_ptr<int>>,
                StatefulAllocator> treeSet(
            {
                std::make_shared<int>(23),
                std::make_shared<int>(1),
                std::make_shared<int>(5),
                std::make_shared<int>(9),
                std::make_shared<int>(5)
            },
            FlippableCompare<std::shared_ptr<int>>(true),
            StatefulAllocator(2341));

        // NOLINTNEXTLINE(performance-unnecessary-copy-initialization)
        TreeSet<std::shared_ptr<int>,
                FlippableCompare<std::shared_ptr<int>>,
                StatefulAllocator> copy =
        {
            std::make_shared<int>(3242),
            std::make_shared<int>(97),
            std::make_shared<int>(1),
            std::make_shared<int>(87)
        };

        copy = treeSet;

        EXPECT_EQ(copy.GetAllocator(), treeSet.GetAllocator());
        EXPECT_EQ(copy.GetAllocator().GetId(), 2341);

        EXPECT_EQ(treeSet.Size(), 4);
        EXPECT_EQ(copy.Size(), 4);

        std::vector<int> expected = { 23, 9, 5, 1 };
        auto iter = treeSet.GetBegin();
        auto copyIter = copy.GetBegin();

        for (int index = 0; index < expected.size(); ++index, ++iter, ++copyIter)
        {
            EXPECT_EQ(**iter, expected[index]);
            EXPECT_EQ(**copyIter, expected[index]);

            EXPECT_EQ(*iter, *copyIter);
        }
    }

    // TreeSet<T, Comp, Alloc>::operator=(TreeSet&&)
    TEST(TreeSetTest, MoveAssign)
    {
        TreeSet<std::shared_ptr<int>,
                FlippableCompare<std::shared_ptr<int>>,
                StatefulAllocator> treeSet(
            {
                std::make_shared<int>(23),
                std::make_shared<int>(1),
                std::make_shared<int>(5),
                std::make_shared<int>(9),
                std::make_shared<int>(5)
            },
            FlippableCompare<std::shared_ptr<int>>(true),
            StatefulAllocator(2341));

        TreeSet<std::shared_ptr<int>,
                FlippableCompare<std::shared_ptr<int>>,
                StatefulAllocator> move = {
            std::make_shared<int>(343),
            std::make_shared<int>(21),
            std::make_shared<int>(2),
            std::make_shared<int>(0),
        };

        move = std::move(treeSet);

        EXPECT_EQ(treeSet.GetAllocator().GetId(), 0);
        EXPECT_EQ(move.GetAllocator().GetId(), 2341);

        EXPECT_EQ(treeSet.Size(), 0);
        EXPECT_EQ(move.Size(), 4);

        std::vector<int> expected = { 23, 9, 5, 1 };
        auto iter = move.GetBegin();

        for (int index = 0; index < expected.size(); ++index, ++iter)
        {
            EXPECT_EQ(**iter, expected[index]);
            EXPECT_EQ(iter->use_count(), 1);
        }
    }

    // TreeSet<T, Comp, Alloc>::operator=(std::initializer_list)
    TEST(TreeSetTest, InitializerListAssign)
    {
        TreeSet<int, FlippableCompare<int>, StatefulAllocator> treeSet(
            { 2384, 23, 12, 11 },
            StatefulAllocator(231));

        ASSERT_EQ(treeSet.GetAllocator().GetId(), 231);
        treeSet = { 23, 1, 5, 9, 5 };

        EXPECT_EQ(treeSet.GetAllocator().GetId(), 231);
        EXPECT_EQ(treeSet.Size(), 4);

        std::vector<int> expected = { 1, 5, 9, 23 };
        Index index = 0;

        for (auto iter = treeSet.GetBegin(); iter != treeSet.GetEnd(); ++iter, ++index)
            EXPECT_EQ(*iter, expected[index]);
    }

    // TreeSet<T, Comp, Alloc>::GetBegin()
    // TreeSet<T, Comp, Alloc>::GetEnd()
    TEST(TreeSetTest, Iterators)
    {
        TreeSet<int, FlippableCompare<int>> treeSet = { 32, 1, 5, 7, 12, 1 };
        ASSERT_EQ(treeSet.Size(), 5);

        EXPECT_EQ(*treeSet.GetBegin(), 1);

        /* TreeSet<T, Comp, Alloc>::GetEnd()'s iterator value is undefined, and therefore
         * cannot be tested.
         */
    }

    // TreeSet<T, Comp, Alloc>::IsEmpty()
    TEST(TreeSetTest, IsEmpty)
    {
        TreeSet<int> treeSet = { 39, 12, 12 };
        TreeSet<int> empty;

        ASSERT_EQ(treeSet.Size(), 2);
        ASSERT_EQ(empty.Size(), 0);

        EXPECT_FALSE(treeSet.IsEmpty());
        EXPECT_TRUE(empty.IsEmpty());
    }

    /* TreeSet<T, Comp, Alloc>::Size() and TreeSet<T, Comp, Alloc>::GetAllocator() is
     * assumed to work. No tests.
     */

    // TreeSet<T, Comp, Alloc>::Clear()
    TEST(TreeSetTest, Clear)
    {
        TreeSet<int, LessFunctor<int>, TrackingAllocator> treeSet = { 3, 123, 123, 10 };
        ASSERT_GT(treeSet.GetAllocator().AllocationSize(), 0);
        ASSERT_GT(treeSet.Size(), 0);

        treeSet.Clear();

        EXPECT_EQ(treeSet.Size(), 0);
        EXPECT_EQ(treeSet.GetAllocator().AllocationSize(), 0);
    }

    // TreeSet<T, Comp, Alloc>::Emplace(Args&&...)
    TEST(TreeSetTest, Emplace)
    {
        using Ptr = std::shared_ptr<int>;
        TreeSet<Ptr, FlippableCompare<Ptr>, TrackingAllocator> treeSet = {
            std::make_shared<int>(23),
            std::make_shared<int>(0),
            std::make_shared<int>(3234),
            std::make_shared<int>(61),
        };

        int* pointer = new int(4);
        auto [iterator, success] = treeSet.Emplace(pointer);

        EXPECT_EQ(iterator->get(), pointer);
        EXPECT_TRUE(success);

        int* pointer2 = new int(0);
        auto [iter2, success2] = treeSet.Emplace(pointer2);

        EXPECT_EQ(**iter2, 0);
        EXPECT_FALSE(success2);

        /* Emplace() creates the object, delete will have already been called
         * on pointer2.
         */

        std::vector<int> expected = { 0, 4, 23, 61, 3234 };
        int index = 0;

        for (auto iter = treeSet.GetBegin(); iter != treeSet.GetEnd(); ++iter, ++index)
            EXPECT_EQ(**iter, expected[index]);
    }

    // TreeSet<T, Comp, Alloc>::Insert(const T&)
    TEST(TreeSetTest, InsertCopy)
    {
        using Ptr = std::shared_ptr<int>;
        TreeSet<Ptr, FlippableCompare<Ptr>, TrackingAllocator> treeSet = {
            std::make_shared<int>(23),
            std::make_shared<int>(0),
            std::make_shared<int>(3234),
            std::make_shared<int>(61),
        };

        ASSERT_EQ(treeSet.Size(), 4);

        Usize bytes = treeSet.GetAllocator().AllocationSize();
        Ptr pointer = std::make_shared<int>(5);

        auto [iter, success] = treeSet.Insert(pointer);
        auto [iter2, fail] = treeSet.Insert(pointer);

        EXPECT_EQ(*iter, pointer);
        EXPECT_TRUE(success);

        EXPECT_EQ(iter2, iter);
        EXPECT_FALSE(fail);

        EXPECT_GT(treeSet.GetAllocator().AllocationSize(), bytes);
        EXPECT_EQ(treeSet.Size(), 5);

        std::vector<int> expected = { 0, 5, 23, 61, 3234 };
        int index = 0;

        for (auto iter = treeSet.GetBegin(); iter != treeSet.GetEnd(); ++iter, ++index)
        {
            EXPECT_EQ(iter->use_count(), 1 + (**iter == 5));
            EXPECT_EQ(**iter, expected[index]);
        }
    }

    // TreeSet<T, Comp, Alloc>::Insert(T&&)
    TEST(TreeSetTest, InsertMove)
    {
        using Ptr = std::shared_ptr<int>;
        TreeSet<Ptr, FlippableCompare<Ptr>, TrackingAllocator> treeSet = {
            std::make_shared<int>(23),
            std::make_shared<int>(0),
            std::make_shared<int>(3234),
            std::make_shared<int>(61),
        };

        ASSERT_EQ(treeSet.Size(), 4);

        Usize bytes = treeSet.GetAllocator().AllocationSize();

        auto [iter, success] = treeSet.Insert(std::make_shared<int>(5));
        auto [iter2, fail] = treeSet.Insert(std::make_shared<int>(5));

        EXPECT_EQ(**iter, 5);
        EXPECT_TRUE(success);

        EXPECT_EQ(iter2, iter);
        EXPECT_FALSE(fail);

        EXPECT_GT(treeSet.GetAllocator().AllocationSize(), bytes);
        EXPECT_EQ(treeSet.Size(), 5);

        std::vector<int> expected = { 0, 5, 23, 61, 3234 };
        int index = 0;

        for (auto iter = treeSet.GetBegin(); iter != treeSet.GetEnd(); ++iter, ++index)
        {
            EXPECT_EQ(iter->use_count(), 1);
            EXPECT_EQ(**iter, expected[index]);
        }
    }

    // TreeSet<T, Comp, Alloc>::Insert(Iter, Iter)
    TEST(TreeSetTest, InsertRange)
    {
        using Ptr = std::shared_ptr<int>;
        TreeSet<Ptr, FlippableCompare<Ptr>, TrackingAllocator> treeSet = {
            std::make_shared<int>(23),
            std::make_shared<int>(0),
            std::make_shared<int>(3234),
            std::make_shared<int>(61),
        };

        ASSERT_EQ(treeSet.Size(), 4);

        Usize bytes = treeSet.GetAllocator().AllocationSize();
        ForwardTestContainer<Ptr, 6> container = {
            std::make_shared<int>(5),
            std::make_shared<int>(23),
            std::make_shared<int>(123),
            std::make_shared<int>(13),
            std::make_shared<int>(5),
            std::make_shared<int>(73),
        };

        treeSet.Insert(container.GetBegin(), container.GetEnd());

        EXPECT_GT(treeSet.GetAllocator().AllocationSize(), bytes);
        EXPECT_EQ(treeSet.Size(), 8);

        std::vector<int> expected = { 0, 5, 13, 23, 61, 73, 123, 3234 };
        int index = 0;

        std::vector<int> twoOwners = { 5, 123, 13, 73 };
        for (auto iter = treeSet.GetBegin(); iter != treeSet.GetEnd(); ++iter, ++index)
        {
            if (std::find(twoOwners.begin(), twoOwners.end(), **iter) != twoOwners.end())
                EXPECT_EQ(iter->use_count(), 2);
            else
                EXPECT_EQ(iter->use_count(), 1);

            EXPECT_EQ(**iter, expected[index]);
        }
    }

    // TreeSet<T, Comp, Alloc>::Insert(std::initializer_list)
    TEST(TreeSetTest, InsertInitializerList)
    {
        using Ptr = std::shared_ptr<int>;
        TreeSet<Ptr, FlippableCompare<Ptr>, TrackingAllocator> treeSet = {
            std::make_shared<int>(23),
            std::make_shared<int>(0),
            std::make_shared<int>(3234),
            std::make_shared<int>(61),
        };

        ASSERT_EQ(treeSet.Size(), 4);

        Usize bytes = treeSet.GetAllocator().AllocationSize();
        treeSet.Insert({
            std::make_shared<int>(5),
            std::make_shared<int>(23),
            std::make_shared<int>(123),
            std::make_shared<int>(13),
            std::make_shared<int>(5),
            std::make_shared<int>(73),
        });

        EXPECT_GT(treeSet.GetAllocator().AllocationSize(), bytes);
        EXPECT_EQ(treeSet.Size(), 8);

        std::vector<int> expected = { 0, 5, 13, 23, 61, 73, 123, 3234 };
        int index = 0;

        for (auto iter = treeSet.GetBegin(); iter != treeSet.GetEnd(); ++iter, ++index)
        {
            EXPECT_EQ(iter->use_count(), 1);
            EXPECT_EQ(**iter, expected[index]);
        }
    }

    // TreeSet<T, Comp, Alloc>::Swap(TreeSet&)
    TEST(TreeSetTest, Swap)
    {
        TreeSet<int, FlippableCompare<int>, StatefulAllocator> set1(
            { 23, 1287, 38, 595, 122, 38, 444 },
            FlippableCompare<int>(true), StatefulAllocator(12));

        TreeSet<int, FlippableCompare<int>, StatefulAllocator> set2(
            { 23, 93, 1287, 123, 11 },
            FlippableCompare<int>(false), StatefulAllocator(4444));

        set1.Swap(set2);

        EXPECT_EQ(set1.Size(), 5);
        EXPECT_EQ(set2.Size(), 6);

        EXPECT_EQ(set1.GetAllocator().GetId(), 4444);
        EXPECT_EQ(set2.GetAllocator().GetId(), 12);

        int expected1[5] = { 11, 23, 93, 123, 1287 };
        Index index1 = 0;

        for (auto iter = set1.GetBegin(); iter != set1.GetEnd(); ++iter, ++index1)
            EXPECT_EQ(*iter, expected1[index1]);

        int expected2[6] = { 1287, 595, 444, 122, 38, 23 };
        Index index2 = 0;

        for (auto iter = set2.GetBegin(); iter != set2.GetEnd(); ++iter, ++index2)
            EXPECT_EQ(*iter, expected2[index2]);
    }

    // TreeSet<T, Comp, Alloc>::Contains(const T&)
    TEST(TreeSetTest, Contains)
    {
        TreeSet<int, FlippableCompare<int>> set(
            { 23, 1287, 38, 595, 122, 38, 444 },
            FlippableCompare<int>(true));

        EXPECT_TRUE(set.Contains(23));
        EXPECT_FALSE(set.Contains(445));
    }

    // TreeSet<T, Comp, Alloc>::Find(const T&)
    TEST(TreeSetTest, Find)
    {
        TreeSet<int> set = { 23, 1287, 38, 595, 122, 38, 444 };
        const TreeSet<int> constSet = { 23, 1287, 38, 595, 122, 38, 444 };

        EXPECT_EQ(*set.Find(23), 23);
        EXPECT_EQ(set.Find(1), set.GetEnd());

        EXPECT_EQ(*constSet.Find(23), 23);
        EXPECT_EQ(constSet.Find(1), constSet.GetEnd());
    }

    // operator==(const TreeSet<T, Comp, Alloc>&, const TreeSet<T, Comp, Alloc>&)
    TEST(TreeSetTest, Equal)
    {
        TreeSet<int> set = { 123, 92, 91, 11, -34, -3, 0 };
        TreeSet<int> equalSet = { 92, 91, -34, 123, -3, 11, 0 };
        TreeSet<int> unequalSet = { 123, 92, 91, 11, -34, -3, 1 };

        EXPECT_TRUE(set == equalSet);
        EXPECT_FALSE(set == unequalSet);
    }

    // TreeSet<T, Comp, Alloc>::Iterator::operator*()
    // TreeSet<T, Comp, Alloc>::ConstIterator::operator*()
    TEST(TreeSetTest, IteratorDereference)
    {
        // The end user must not be able to modify any element of the tree set.
        TreeSet<int> set = { 8, 19, 10, 68, 4 };
        const TreeSet<int> constSet = { 8, 19, 10, 68, 4 };

        ASSERT_EQ(*set.GetBegin(), 4);
        ASSERT_EQ(*constSet.GetBegin(), 4);

        EXPECT_TRUE((std::is_same_v<decltype(*set.GetBegin()), const int&>));
        EXPECT_TRUE((std::is_same_v<decltype(*constSet.GetBegin()), const int&>));
    }

    // TreeSet<T, Comp, Alloc>::Iterator::operator->()
    // TreeSet<T, Comp, Alloc>::ConstIterator::operator->()
    TEST(TreeSetTest, IteratorArrowOperator)
    {
        // The end user must not be able to modify any element of the tree set.
        TreeSet<int> set = { 8, 19, 10, 68, 4 };
        const TreeSet<int> constSet = { 8, 19, 10, 68, 4 };

        EXPECT_TRUE((std::is_same_v<
            decltype(set.GetBegin().operator->()),
            const int*>));

        EXPECT_TRUE((std::is_same_v<
            decltype(constSet.GetBegin().operator->()),
            const int*>));
    }

    // TreeSet<T, Comp, Alloc>::Iterator::operator++()
    // TreeSet<T, Comp, Alloc>::Iterator::operator++(int)
    TEST(TreeSetTest, IteratorIncrement)
    {
        TreeSet<int> set = { 8, 19, 10, 68, 4 };
        auto iter = set.GetBegin();

        ASSERT_EQ(*iter, 4);

        EXPECT_EQ(*(iter++), 4);
        EXPECT_EQ(*iter, 8);

        EXPECT_EQ(*(++iter), 10);
        EXPECT_EQ(*iter, 10);
    }

    // operator==(
    //     const TreeSet<T, Comp, Alloc>::[Const]Iterator&,
    //     const TreeSet<T, Comp, Alloc>::[Const]Iterator&)
    TEST(TreeSetTest, IteratorEqual)
    {
        TreeSet<int> set = { 8, 19, 10, 68, 4 };

        auto iter = set.GetBegin();
        auto sameIter = set.GetBegin();

        auto differentIter = set.GetBegin();
        Algorithms::Advance(differentIter, 3);

        EXPECT_EQ(iter, sameIter);
        EXPECT_NE(iter, differentIter);
    }
}
