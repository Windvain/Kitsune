#include "Foundation/Algorithms/Fill.h"

#include <gtest/gtest.h>
#include "TestContainer.h"

using namespace Kitsune;
using namespace Kitsune::Testing;

TEST(FillTests, Fill)
{
    ForwardTestContainer<int, 6> container = { 32, 6, 22, 6, 12, 222 };
    Algorithms::Fill(container.GetBegin(), container.GetEnd(), 655);

    for (std::size_t i = 0; i < 6; ++i)
        EXPECT_EQ(container[i], 655);
}

TEST(FillTests, FillN)
{
    ForwardTestContainer<int, 6> container = { 32, 6, 22, 6, 12, 222 };
    Algorithms::FillN(container.GetBegin(), 6, 655);

    for (std::size_t i = 0; i < 6; ++i)
        EXPECT_EQ(container[i], 655);
}
