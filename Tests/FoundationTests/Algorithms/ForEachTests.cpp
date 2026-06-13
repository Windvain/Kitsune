#include <gtest/gtest.h>
#include "TestContainer.h"

#include "Foundation/Algorithms/ForEach.h"

namespace
{
    using namespace Kitsune;
    using Testing::ForwardTestContainer;

    // Algorithms::ForEach(Iter, Iter, Func)
    TEST(ForEachTest, ForEach)
    {
        ForwardTestContainer<int, 7> container = { 1, 65, 3, 87, 878, 1, 33 };
        int iterations = 0;

        const auto function = [&](int element) -> void
        {
            EXPECT_EQ(element, container[iterations]);
            ++iterations;
        };

        Algorithms::ForEach(container.GetBegin(), container.GetEnd(), function);
        EXPECT_EQ(iterations, 7);
    }

    // Algorithms::ForEachN(Iter, Size, Func)
    TEST(ForEachTest, ForEachN)
    {
        ForwardTestContainer<int, 7> container = { 1, 65, 3, 87, 878, 1, 33 };
        int iterations = 0;

        const auto function = [&](int element) -> void
        {
            EXPECT_EQ(element, container[iterations]);
            ++iterations;
        };

        Algorithms::ForEachN(container.GetBegin(), 7, function);
        EXPECT_EQ(iterations, 7);
    }

    // Algorithms::ForEachIf(Iter, Size, Pred, Func)
    TEST(ForEachTest, ForEachIf)
    {
        ForwardTestContainer<int, 7> container = { 1, 65, 3, 87, 878, 1, 33 };
        std::vector expected = { 878 };

        int iterations = 0;
        const auto function = [&](int element) -> void
        {
            EXPECT_EQ(element, expected[iterations]);
            ++iterations;
        };

        const auto predicate = [](int element) -> bool
        {
            return ((element % 2) == 0);
        };

        Algorithms::ForEachIf(
            container.GetBegin(),
            container.GetEnd(),
            predicate,
            function);

        EXPECT_EQ(iterations, 1);
    }
}
