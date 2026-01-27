#include <gtest/gtest.h>
#include "TestContainer2.h"

#include "Foundation/Algorithms/Equal.h"

namespace
{
    class A
    {
    public:
        A() : Value(0) { /* ... */ }
        A(int x) : Value(x) { /* ... */ }

    public:
        int Value;
    };
}

using namespace Testing;
using namespace Kitsune;

TEST(EqualTests, WithoutSizeCheckAndPredicate)
{
    ForwardTestContainer<int, 5> container({ 1, 2, 34, 45, 32 });

    ForwardTestContainer<int, 7> equalRange({ 1, 2, 34, 45, 32, 7, 9 });
    ForwardTestContainer<int, 7> inequalRange({ 1, 2, 5, 45, 32, 7, 9 });

    EXPECT_TRUE(Algorithms::Equal(container.GetBegin(), container.GetEnd(), equalRange.GetBegin()));
    EXPECT_FALSE(Algorithms::Equal(container.GetBegin(), container.GetEnd(), inequalRange.GetBegin()));
}

TEST(EqualTests, PredicateWithoutSizeCheck)
{
    ForwardTestContainer<A, 5> container({ 1, 2, 34, 45, 32 });

    ForwardTestContainer<A, 7> equalRange({ 1, 2, 34, 45, 32, 7, 9 });
    ForwardTestContainer<A, 7> inequalRange({ 1, 2, 5, 45, 32, 7, 9 });

    auto predicate = [](const A& lhs, const A& rhs) -> bool
    {
        return (lhs.Value == rhs.Value);
    };

    EXPECT_TRUE(Algorithms::Equal(container.GetBegin(), container.GetEnd(), equalRange.GetBegin(), predicate));
    EXPECT_FALSE(Algorithms::Equal(container.GetBegin(), container.GetEnd(), inequalRange.GetBegin(), predicate));
}

TEST(EqualTests, SizeCheckWithoutPredicate)
{
    ForwardTestContainer<int, 5> container({ 1, 2, 34, 45, 32 });
    ForwardTestContainer<int, 5> rangeEqual({ 1, 2, 34, 45, 32 });
    ForwardTestContainer<int, 6> rangeSizeInequal({ 1, 2, 34, 45, 32, 53 });
    ForwardTestContainer<int, 5> rangeContentsInequal({ 423, 31, 342, 453, 1 });

    EXPECT_FALSE(Algorithms::Equal(container.GetBegin(), container.GetEnd(),
                                   rangeSizeInequal.GetBegin(), rangeSizeInequal.GetEnd()));

    EXPECT_FALSE(Algorithms::Equal(container.GetBegin(), container.GetEnd(),
                                   rangeContentsInequal.GetBegin(), rangeContentsInequal.GetEnd()));

    EXPECT_TRUE(Algorithms::Equal(container.GetBegin(), container.GetEnd(),
                                  rangeEqual.GetBegin(), rangeEqual.GetEnd()));
}

TEST(EqualTests, SizeCheckWithPredicate)
{
    ForwardTestContainer<A, 5> container({ 1, 2, 34, 45, 32 });
    ForwardTestContainer<A, 5> rangeEqual({ 1, 2, 34, 45, 32 });
    ForwardTestContainer<A, 6> rangeSizeInequal({ 1, 2, 34, 45, 32, 53 });
    ForwardTestContainer<A, 5> rangeContentsInequal({ 423, 31, 342, 453, 1 });

    auto predicate = [](const A& lhs, const A& rhs) -> bool
    {
        return (lhs.Value == rhs.Value);
    };

    EXPECT_FALSE(Algorithms::Equal(container.GetBegin(), container.GetEnd(),
                                   rangeSizeInequal.GetBegin(), rangeSizeInequal.GetEnd(), predicate));

    EXPECT_FALSE(Algorithms::Equal(container.GetBegin(), container.GetEnd(),
                                   rangeContentsInequal.GetBegin(), rangeContentsInequal.GetEnd(), predicate));

    EXPECT_TRUE(Algorithms::Equal(container.GetBegin(), container.GetEnd(),
                                  rangeEqual.GetBegin(), rangeEqual.GetEnd(), predicate));
}
