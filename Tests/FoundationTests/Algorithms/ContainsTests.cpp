#include <gtest/gtest.h>
#include "TestContainer.h"

#include "Foundation/Algorithms/Contains.h"

namespace
{
    using namespace Kitsune;
    using Testing::ForwardTestContainer;

    // Algorithms::Contains(Iter, Iter, const T&)
    TEST(ContainsTest, ContainsElement)
    {
        ForwardTestContainer<int, 5> container = { 2, 12, 565, 23, 675 };
        for (int element : container)
        {
            EXPECT_TRUE(
                Algorithms::Contains(
                    container.GetBegin(),
                    container.GetEnd(),
                    element));
        }

        EXPECT_FALSE(
            Algorithms::Contains(
                container.GetBegin(),
                container.GetEnd(),
                4));
    }

    // Algorithms::Contains(Iter, Iter, Iter2, Iter2)
    TEST(ContainsTest, ContainsRange)
    {
        ForwardTestContainer<int, 5> container = { 2, 12, 565, 23, 675 };

        ForwardTestContainer<int, 3> contained = { 12, 565, 23 };
        ForwardTestContainer<int, 4> notContained = { 12, 565, 23, 674 };

        EXPECT_TRUE(
            Algorithms::Contains(
                container.GetBegin(),
                container.GetEnd(),
                contained.GetBegin(),
                contained.GetEnd()));

        EXPECT_FALSE(
            Algorithms::Contains(
                container.GetBegin(),
                container.GetEnd(),
                notContained.GetBegin(),
                notContained.GetEnd()));
    }

    // Algorithms::Contains(Iter, Iter, Pred)
    TEST(ContainsTest, ContainsIf)
    {
        ForwardTestContainer<int, 5> container = { 2, 12, 565, 23, 675 };

        EXPECT_TRUE(
            Algorithms::ContainsIf(
                container.GetBegin(),
                container.GetEnd(),
                [](int x) -> bool
                {
                    return x == 2;
                }));

        EXPECT_FALSE(
            Algorithms::ContainsIf(
                container.GetBegin(),
                container.GetEnd(),
                [](int x) -> bool
                {
                    return x == 19;
                }));
    }
}
