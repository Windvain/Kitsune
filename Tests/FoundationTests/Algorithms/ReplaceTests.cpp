#include <gtest/gtest.h>
#include "TestContainer.h"

#include "Foundation/Algorithms/Replace.h"

namespace
{
    using namespace Kitsune;
    using Testing::ForwardTestContainer;

    // Algorithms::Replace(Iter, Iter, const T&, const T&)
    TEST(ReplaceTests, Replace)
    {
        ForwardTestContainer<int, 5> container = { 4, 5, 6, 4, 2 };
        std::vector<int> expected = { 17, 5, 6, 17, 2 };

        Algorithms::Replace(container.GetBegin(), container.GetEnd(), 4, 17);

        for (std::size_t i = 0; i < 5; ++i)
            EXPECT_EQ(container[i], expected[i]);
    }

    // Algorithms::ReplaceN(Iter, Size, const T&, const T&)
    TEST(ReplaceTests, ReplaceN)
    {
        ForwardTestContainer<int, 5> container = { 4, 5, 6, 4, 2 };
        std::vector<int> expected = { 17, 5, 6, 4, 2 };

        Algorithms::ReplaceN(container.GetBegin(), 3, 4, 17);

        for (std::size_t i = 0; i < 5; ++i)
            EXPECT_EQ(container[i], expected[i]);
    }

    // Algorithms::ReplaceIf(Iter, Size, Pred, const T&)
    TEST(ReplaceTests, ReplaceIf)
    {
        ForwardTestContainer<int, 5> container = { 4, 5, 6, 4, 2 };
        std::vector<int> expected = { 17, 5, 17, 17, 17 };

        auto pred = [](int x) -> bool { return (x % 2) == 0; };
        Algorithms::ReplaceIf(container.GetBegin(), container.GetEnd(), pred, 17);

        for (std::size_t i = 0; i < 5; ++i)
            EXPECT_EQ(container[i], expected[i]);
    }
}
