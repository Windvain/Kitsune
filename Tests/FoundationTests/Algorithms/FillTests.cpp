#include <gtest/gtest.h>
#include "TestContainer.h"

#include "Foundation/Algorithms/Fill.h"

namespace
{
    using namespace Kitsune;
    using Testing::ForwardTestContainer;

    // Algorithms::Fill(Iter, Iter, const T&)
    TEST(FillTest, Fill)
    {
        ForwardTestContainer<int, 6> container = { 32, 6, 22, 6, 12, 222 };
        ForwardTestContainer<int, 6> expected = { 655, 655, 655, 655, 655, 655 };

        Algorithms::Fill(container.GetBegin(), container.GetEnd(), 655);

        for (int index = 0; index < 6; ++index)
            EXPECT_EQ(container[index], expected[index]);
    }

    // Algorithms::FillN(Iter, Size, const T&)
    TEST(FillTest, FillN)
    {
        ForwardTestContainer<int, 6> container = { 32, 6, 22, 6, 12, 222 };
        ForwardTestContainer<int, 6> expected = { 655, 655, 655, 655, 655, 655 };

        Algorithms::FillN(container.GetBegin(), container.Size(), 655);

        for (int index = 0; index < 6; ++index)
            EXPECT_EQ(container[index], expected[index]);
    }
}
