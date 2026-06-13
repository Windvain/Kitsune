#include <gtest/gtest.h>
#include "TestContainer.h"

#include "Foundation/Algorithms/Distance.h"

namespace
{
    using namespace Kitsune;
    using Testing::ForwardTestContainer, Testing::RandomAccessTestContainer;

    // Algorithms::Distance(Iter, Iter)
    TEST(DistanceTest, NotRandomAccess)
    {
        ForwardTestContainer<int, 6> container = { 2, 4, 1, 65, 7, 1 };
        EXPECT_EQ(Algorithms::Distance(container.GetBegin(), container.GetEnd()), 6);
    }

    // Algorithms::Distance(RandomAccessIter, RandomAccessIter)
    TEST(DistanceTest, RandomAccess)
    {
        RandomAccessTestContainer<int, 4> container = { 4, 93, 61, 3 };
        EXPECT_EQ(Algorithms::Distance(container.GetBegin(), container.GetEnd()), 4);
    }
}
