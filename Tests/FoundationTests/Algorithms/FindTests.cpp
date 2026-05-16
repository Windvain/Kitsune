#include "Foundation/Algorithms/Find.h"

#include <gtest/gtest.h>
#include "TestContainer.h"

using namespace Kitsune;
using namespace Kitsune::Testing;

TEST(FindTests, FindByEquality)
{
    ForwardTestContainer<int, 5> container({ 1, 2, 54, 2, 1 });

    auto it = container.GetBegin();
    ++it;

    EXPECT_EQ(Algorithms::Find(container.GetBegin(), container.GetEnd(), 2), it);
    EXPECT_EQ(Algorithms::Find(container.GetBegin(), container.GetEnd(), -1),
                               container.GetEnd());
}

TEST(FindTests, FindRangeByEquality)
{
    ForwardTestContainer<int, 5> container({ 1, 2, 54, 2, 1 });
    ForwardTestContainer<int, 2> rangeFound({ 54, 2 });
    ForwardTestContainer<int, 3> rangeNotFound({ 2, 3, 4 });

    auto it = container.GetBegin();
    ++it; ++it;

    EXPECT_EQ(Algorithms::Find(container.GetBegin(), container.GetEnd(),
                               rangeFound.GetBegin(), rangeFound.GetEnd()), it);

    EXPECT_EQ(Algorithms::Find(container.GetBegin(), container.GetEnd(),
                               rangeNotFound.GetBegin(), rangeNotFound.GetEnd()),
                               container.GetEnd());
}

TEST(FindTests, FindIf)
{
    ForwardTestContainer<int, 5> container({ 1, 2, 54, 2, 1 });
    auto predicate = [](int element) -> bool
    {
        return (element == 54);
    };

    auto failingPredicate = [](int element) -> bool
    {
        return (element > 57);
    };

    auto it = container.GetBegin();
    ++it; ++it;

    EXPECT_EQ(Algorithms::FindIf(container.GetBegin(), container.GetEnd(),
                                 predicate), it);

    EXPECT_EQ(Algorithms::FindIf(container.GetBegin(), container.GetEnd(),
                                 failingPredicate), container.GetEnd());
}

TEST(FindTests, FindLast)
{
    ForwardTestContainer<int, 5> container({ 1, 2, 54, 2, 1 });

    auto it = container.GetBegin();
    ++it; ++it; ++it;

    EXPECT_EQ(Algorithms::FindLast(container.GetBegin(), container.GetEnd(), 2),
              it);

    EXPECT_EQ(Algorithms::FindLast(container.GetBegin(), container.GetEnd(), -2),
              container.GetEnd());
}

TEST(FindTests, FindLastIf)
{
    ForwardTestContainer<int, 5> container({ 1, 2, 54, 2, 1 });
    auto predicate = [](int element) -> bool
    {
        return (element % 2) == 0;
    };

    auto failingPredicate = [](int element) -> bool
    {
        return (element == -1);
    };

    auto it = container.GetBegin();
    ++it; ++it; ++it;

    EXPECT_EQ(Algorithms::FindLastIf(container.GetBegin(), container.GetEnd(),
                                     predicate), it);

    EXPECT_EQ(Algorithms::FindLastIf(container.GetBegin(), container.GetEnd(),
                                     failingPredicate), container.GetEnd());
}
