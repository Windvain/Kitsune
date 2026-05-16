#include <gtest/gtest.h>
#include "TestContainer.h"

#include "Foundation/Algorithms/Contains.h"

using namespace Kitsune;
using namespace Kitsune::Testing;

TEST(ContainsTests, ContainsElement)
{
    ForwardTestContainer<int, 5> container = { 2, 12, 565, 23, 675 };
    for (int element : container)
    {
        EXPECT_TRUE(
            Algorithms::Contains(
                container.GetBegin(),
                container.GetEnd(),
                element)
        );
    }

    EXPECT_FALSE(Algorithms::Contains(container.GetBegin(), container.GetEnd(), 4));
}

TEST(ContainsTests, ContainsRange)
{
    ForwardTestContainer<int, 5> container = { 2, 12, 565, 23, 675 };
    ForwardTestContainer<int, 3> range = { 12, 565, 23 };

    ForwardTestContainer<int, 4> range2 = { 12, 565, 23, 674 };

    EXPECT_TRUE(Algorithms::Contains(container.GetBegin(), container.GetEnd(),
                                     range.GetBegin(), range.GetEnd()));

    EXPECT_FALSE(Algorithms::Contains(container.GetBegin(), container.GetEnd(),
                                      range2.GetBegin(), range2.GetEnd()));
}

TEST(ContainsTests, ContainsIf)
{
    ForwardTestContainer<int, 5> container = { 2, 12, 565, 23, 675 };
    EXPECT_TRUE(Algorithms::ContainsIf(
        container.GetBegin(), container.GetEnd(),
        [](int x) -> bool { return x == 2; }));

    EXPECT_FALSE(Algorithms::ContainsIf(
        container.GetBegin(), container.GetEnd(),
        [](int x) -> bool { return x == 19; }));
}
