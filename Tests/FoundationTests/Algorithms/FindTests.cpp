#include <gtest/gtest.h>
#include "TestContainer.h"

#include "Foundation/Algorithms/Find.h"
#include "Foundation/Algorithms/Advance.h"

namespace
{
    using namespace Kitsune;
    using Testing::ForwardTestContainer;

    // Algorithms::Find(Iter, Iter, const T&)
    TEST(FindTest, FindByEquality)
    {
        ForwardTestContainer<int, 5> container = { 1, 2, 54, 2, 1 };
        auto iterator = container.GetBegin();

        Algorithms::Advance(iterator, 1);

        EXPECT_EQ(
            Algorithms::Find(container.GetBegin(), container.GetEnd(), 2),
            iterator);

        EXPECT_EQ(
            Algorithms::Find(container.GetBegin(), container.GetEnd(), -1),
            container.GetEnd());
    }

    // Algorithms::Find(Iter, Iter, RngIter, RngIter)
    TEST(FindTest, FindRangeByEquality)
    {
        ForwardTestContainer<int, 5> container = { 1, 2, 54, 2, 1 };
        ForwardTestContainer<int, 2> foundRange = { 54, 2 };
        ForwardTestContainer<int, 3> absentRange = { 2, 3, 4 };

        auto iterator = container.GetBegin();
        Algorithms::Advance(iterator, 2);

        EXPECT_EQ(
            Algorithms::Find(
                container.GetBegin(), container.GetEnd(),
                foundRange.GetBegin(), foundRange.GetEnd()),
            iterator);

        EXPECT_EQ(
            Algorithms::Find(
                container.GetBegin(), container.GetEnd(),
                absentRange.GetBegin(), absentRange.GetEnd()),
            container.GetEnd());
    }

    // Algorithms::FindIf(Iter, Iter, Pred)
    TEST(FindTest, FindIf)
    {
        ForwardTestContainer<int, 5> container = { 1, 2, 54, 2, 1 };
        const auto predicate = [](int element) -> bool
        {
            return (element == 54);
        };

        const auto failingPred = [](int element) -> bool
        {
            return (element > 57);
        };

        auto iterator = container.GetBegin();
        Algorithms::Advance(iterator, 2);

        EXPECT_EQ(
            Algorithms::FindIf(container.GetBegin(), container.GetEnd(), predicate),
            iterator);

        EXPECT_EQ(
            Algorithms::FindIf(container.GetBegin(), container.GetEnd(), failingPred),
            container.GetEnd());
    }

    // Algorithms::FindLast(Iter, Iter, const T&)
    TEST(FindTest, FindLast)
    {
        ForwardTestContainer<int, 5> container = { 1, 2, 54, 2, 1 };
        auto iterator = container.GetBegin();

        Algorithms::Advance(iterator, 3);

        EXPECT_EQ(
            Algorithms::FindLast(container.GetBegin(), container.GetEnd(), 2),
            iterator);

        EXPECT_EQ(
            Algorithms::FindLast(container.GetBegin(), container.GetEnd(), -2),
            container.GetEnd());
    }

    // Algorithms::FindLast(Iter, Iter, Pred)
    TEST(FindTest, FindLastIf)
    {
        ForwardTestContainer<int, 5> container = { 1, 2, 54, 2, 1 };
        auto predicate = [](int element) -> bool
        {
            return (element % 2) == 0;
        };

        auto failingPred = [](int element) -> bool
        {
            return (element == -1);
        };

        auto iterator = container.GetBegin();
        Algorithms::Advance(iterator, 3);

        EXPECT_EQ(
            Algorithms::FindLastIf(container.GetBegin(), container.GetEnd(), predicate),
            iterator);

        EXPECT_EQ(
            Algorithms::FindLastIf(
                container.GetBegin(), container.GetEnd(),
                failingPred),
            container.GetEnd());
    }
}
