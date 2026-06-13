#include <gtest/gtest.h>
#include "TestContainer.h"

#include "Foundation/Algorithms/Equal.h"

namespace
{
    using namespace Kitsune;
    using Testing::ForwardTestContainer;

    template<Usize N>
    using ContainerType = ForwardTestContainer<int, N>;

    // Algorithms::Equal(Iter1, Iter1, Iter2)
    TEST(EqualTest, WithoutSizeCheckAndPredicate)
    {
        ContainerType<5> container = { 1, 2, 34, 45, 32 };
        ContainerType<7> equalRange = { 1, 2, 34, 45, 32, 45, 88 };
        ContainerType<7> unequalRange = { 1, 72, 34, 45, 32, 45, 88 };

        EXPECT_TRUE(
            Algorithms::Equal(
                container.GetBegin(),
                container.GetEnd(),
                equalRange.GetBegin()));

        EXPECT_FALSE(
            Algorithms::Equal(
                container.GetBegin(),
                container.GetEnd(),
                unequalRange.GetBegin()));
    }

    // Algorithms::Equal(Iter1, Iter1, Iter2, Pred)
    TEST(EqualTest, PredicateWithoutSizeCheck)
    {
        ContainerType<5> container = { 1, 2, 34, 45, 32 };
        ContainerType<7> equalRange = { 1, 2, 34, 45, 32, 45, 88 };
        ContainerType<7> unequalRange = { 1, 72, 34, 45, 32, 45, 88 };

        const auto predicate = [](int lhs, int rhs) -> bool
        {
            return (lhs == rhs);
        };

        EXPECT_TRUE(
            Algorithms::Equal(
                container.GetBegin(),
                container.GetEnd(),
                equalRange.GetBegin(),
                predicate));

        EXPECT_FALSE(
            Algorithms::Equal(
                container.GetBegin(),
                container.GetEnd(),
                unequalRange.GetBegin(),
                predicate));
    }

    // Algorithms::Equal(Iter1, Iter1, Iter2, Iter2)
    TEST(EqualTest, SizeCheckWithoutPredicate)
    {
        ContainerType<5> container = { 1, 2, 34, 45, 32 };

        ContainerType<5> equalRange = { 1, 2, 34, 45, 32 };
        ContainerType<5> unequalContentsRange = { 1, 72, 34, 45, 32 };
        ContainerType<7> unequalSizeRange = { 1, 2, 34, 45, 32, 45, 88 };
        ContainerType<7> unequalSizeAndContentsRange = {
            1, 72, 34, 45, 32, 45, 88
        };

        EXPECT_FALSE(
            Algorithms::Equal(
                container.GetBegin(), container.GetEnd(),
                unequalSizeRange.GetBegin(), unequalSizeRange.GetEnd()));

        EXPECT_FALSE(
            Algorithms::Equal(
                container.GetBegin(), container.GetEnd(),
                unequalContentsRange.GetBegin(), unequalContentsRange.GetEnd()));

        EXPECT_FALSE(
            Algorithms::Equal(
                container.GetBegin(), container.GetEnd(),
                unequalSizeAndContentsRange.GetBegin(),
                unequalSizeAndContentsRange.GetEnd()));

        EXPECT_TRUE(
            Algorithms::Equal(
                container.GetBegin(), container.GetEnd(),
                equalRange.GetBegin(), equalRange.GetEnd()));
    }

    // Algorithms::Equal(Iter1, Iter1, Iter2, Iter2, Pred)
    TEST(EqualTest, SizeCheckWithPredicate)
    {
        ContainerType<5> container = { 1, 2, 34, 45, 32 };

        ContainerType<5> equalRange = { 1, 2, 34, 45, 32 };
        ContainerType<5> unequalContentsRange = { 1, 72, 34, 45, 32 };
        ContainerType<7> unequalSizeRange = { 1, 2, 34, 45, 32, 45, 88 };
        ContainerType<7> unequalSizeAndContentsRange = {
            1, 72, 34, 45, 32, 45, 88
        };

        const auto predicate = [](int lhs, int rhs) -> bool
        {
            return (lhs == rhs);
        };

        EXPECT_FALSE(
            Algorithms::Equal(
                container.GetBegin(), container.GetEnd(),
                unequalSizeRange.GetBegin(), unequalSizeRange.GetEnd(),
                predicate));

        EXPECT_FALSE(
            Algorithms::Equal(
                container.GetBegin(), container.GetEnd(),
                unequalContentsRange.GetBegin(), unequalContentsRange.GetEnd(),
                predicate));

        EXPECT_FALSE(
            Algorithms::Equal(
                container.GetBegin(), container.GetEnd(),
                unequalSizeAndContentsRange.GetBegin(),
                unequalSizeAndContentsRange.GetEnd(),
                predicate));

        EXPECT_TRUE(
            Algorithms::Equal(
                container.GetBegin(), container.GetEnd(),
                equalRange.GetBegin(), equalRange.GetEnd(),
                predicate));
    }
}
