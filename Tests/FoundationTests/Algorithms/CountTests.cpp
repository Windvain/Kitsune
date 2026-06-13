#include <gtest/gtest.h>
#include "TestContainer.h"

#include "Foundation/Algorithms/Count.h"

namespace
{
    using namespace Kitsune;
    using Testing::ForwardTestContainer;

    // Algorithms::Count(Iter, Iter, const T&)
    TEST(CountTest, Count)
    {
        ForwardTestContainer<int, 8> container = { 3, 4, 11, 6, 23, 11, 12, 11 };
        EXPECT_EQ(
            Algorithms::Count(container.GetBegin(), container.GetEnd(), 11),
            3);
    }

    // Algorithms::CountIf(Iter, Iter, Pred)
    TEST(CountTests, CountIf)
    {
        ForwardTestContainer<int, 8> container = { 3, 4, 11, 6, 23, 11, 12, 11 };
        auto count = Algorithms::CountIf(container.GetBegin(), container.GetEnd(),
            [](int element) -> bool
            {
                return ((element % 2) != 0);
            });

        EXPECT_EQ(count, 5);
    }
}
