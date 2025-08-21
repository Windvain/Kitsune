#include <gtest/gtest.h>
#include "TestContainer2.h"

#include "Foundation/Algorithms/Move.h"

namespace
{
    class A
    {
    public:
        A() = default;
        A(int x) : Value(x) { /* ... */ }
        A(const A& a) : Value(a.Value) { /* ... */ }

        A(A&& a) : Value(a.Value)
        {
            a.Value = 0;
        }

    public:
        A& operator=(const A& a) { Value = a.Value; return *this; }
        A& operator=(A&& a) { Value = a.Value; a.Value = 0; return *this; }

    public:
        int Value;
    };
}

using namespace Kitsune;
using namespace Testing;

TEST(MoveTests, Move)
{
    ForwardTestContainer<A, 5> container = { 2, 3, 1, 4, 6 };
    ForwardTestContainer<A, 5> destContainer = { 54, 1, 2, 6, 3 };

    std::vector<A> expected = { 2, 3, 1, 4, 6 };

    auto it = Algorithms::Move(container.GetBegin(), container.GetEnd(), destContainer.GetBegin());
    EXPECT_EQ(it, destContainer.GetEnd());

    for (std::size_t i = 0; i < 5; ++i)
        EXPECT_EQ(container[i].Value, 0);

    for (std::size_t i = 0; i < 5; ++i)
        EXPECT_EQ(destContainer[i].Value, expected[i].Value);
}

TEST(MoveTests, MoveN)
{
    ForwardTestContainer<A, 5> container = { 2, 3, 1, 4, 6 };
    ForwardTestContainer<A, 5> destContainer = { 54, 1, 2, 6, 3 };

    std::vector<A> expected = { 2, 3, 1, 4, 6 };

    auto it = Algorithms::MoveN(container.GetBegin(), 5, destContainer.GetBegin());
    EXPECT_EQ(it, destContainer.GetEnd());

    for (std::size_t i = 0; i < 5; ++i)
        EXPECT_EQ(container[i].Value, 0);

    for (std::size_t i = 0; i < 5; ++i)
        EXPECT_EQ(destContainer[i].Value, expected[i].Value);
}

TEST(MoveTests, MoveBackwards)
{
    BidirTestContainer<A, 8> container = { 2, 3, 1, 4, 6, 8, 16, 44 };
    std::vector<A> expected = { 0, 0, 0, 2, 3, 1, 4, 6 };

    auto rangeEnd = container.GetBegin();
    ++rangeEnd; ++rangeEnd; ++rangeEnd; ++rangeEnd; ++rangeEnd;

    auto destRangeBegin = container.GetEnd();
    --destRangeBegin; --destRangeBegin; --destRangeBegin; --destRangeBegin; --destRangeBegin;

    auto it = Algorithms::MoveBackwards(container.GetBegin(), rangeEnd, container.GetEnd());
    EXPECT_EQ(it, destRangeBegin);

    for (std::size_t i = 0; i < 8; ++i)
        EXPECT_EQ(container[i].Value, expected[i].Value);
}
