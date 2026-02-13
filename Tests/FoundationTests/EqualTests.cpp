#include "Foundation/Algorithms/Equal.h"

#include <gtest/gtest.h>
#include "TestContainer.h"

namespace
{
    class A
    {
    public:
        A()
            : Value(0)
        {
        }

        A(int x)
            : Value(x)
        {
        }

    public:
        int Value;
    };
}

using namespace Kitsune;
using namespace Kitsune::Testing;

TEST(EqualTests, WithoutSizeCheckAndPredicate)
{
    ForwardTestContainer<int, 5> container({ 1, 2, 34, 45, 32 });

    ForwardTestContainer<int, 7> equalRange({ 1, 2, 34, 45, 32, 7, 9 });
    ForwardTestContainer<int, 7> unequalRange({ 1, 2, 5, 45, 32, 7, 9 });

    EXPECT_TRUE(Algorithms::Equal(container.GetBegin(), container.GetEnd(),
                                  equalRange.GetBegin()));

    EXPECT_FALSE(Algorithms::Equal(container.GetBegin(), container.GetEnd(),
                                   unequalRange.GetBegin()));
}

TEST(EqualTests, PredicateWithoutSizeCheck)
{
    ForwardTestContainer<A, 5> container({ 1, 2, 34, 45, 32 });

    ForwardTestContainer<A, 7> equalRange({ 1, 2, 34, 45, 32, 7, 9 });
    ForwardTestContainer<A, 7> unequalRange({ 1, 2, 5, 45, 32, 7, 9 });

    auto predicate = [](const A& lhs, const A& rhs) -> bool
    {
        return (lhs.Value == rhs.Value);
    };

    EXPECT_TRUE(Algorithms::Equal(container.GetBegin(), container.GetEnd(),
                                  equalRange.GetBegin(), predicate));

    EXPECT_FALSE(Algorithms::Equal(container.GetBegin(), container.GetEnd(),
                                   unequalRange.GetBegin(), predicate));
}

TEST(EqualTests, SizeCheckWithoutPredicate)
{
    ForwardTestContainer<int, 5> container({ 1, 2, 34, 45, 32 });
    ForwardTestContainer<int, 5> rangeEqual({ 1, 2, 34, 45, 32 });
    ForwardTestContainer<int, 6> rangeSizeUnequal({ 1, 2, 34, 45, 32, 53 });
    ForwardTestContainer<int, 5> rangeContentsUnequal({ 423, 31, 342, 453, 1 });

    EXPECT_FALSE(Algorithms::Equal(container.GetBegin(), container.GetEnd(),
                                   rangeSizeUnequal.GetBegin(),
                                   rangeSizeUnequal.GetEnd()));

    EXPECT_FALSE(Algorithms::Equal(container.GetBegin(), container.GetEnd(),
                                   rangeContentsUnequal.GetBegin(),
                                   rangeContentsUnequal.GetEnd()));

    EXPECT_TRUE(Algorithms::Equal(container.GetBegin(), container.GetEnd(),
                                  rangeEqual.GetBegin(), rangeEqual.GetEnd()));
}

TEST(EqualTests, SizeCheckWithPredicate)
{
    ForwardTestContainer<A, 5> container({ 1, 2, 34, 45, 32 });
    ForwardTestContainer<A, 5> rangeEqual({ 1, 2, 34, 45, 32 });
    ForwardTestContainer<A, 6> rangeSizeUnequal({ 1, 2, 34, 45, 32, 53 });
    ForwardTestContainer<A, 5> rangeContentsUnequal({ 423, 31, 342, 453, 1 });

    auto predicate = [](const A& lhs, const A& rhs) -> bool
    {
        return (lhs.Value == rhs.Value);
    };

    EXPECT_FALSE(Algorithms::Equal(container.GetBegin(), container.GetEnd(),
                                   rangeSizeUnequal.GetBegin(),
                                   rangeSizeUnequal.GetEnd(), predicate));

    EXPECT_FALSE(Algorithms::Equal(container.GetBegin(), container.GetEnd(),
                                   rangeContentsUnequal.GetBegin(),
                                   rangeContentsUnequal.GetEnd(), predicate));

    EXPECT_TRUE(Algorithms::Equal(container.GetBegin(), container.GetEnd(),
                                  rangeEqual.GetBegin(),
                                  rangeEqual.GetEnd(), predicate));
}
