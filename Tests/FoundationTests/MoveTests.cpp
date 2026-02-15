#include <gtest/gtest.h>

#include "TestContainer.h"
#include "TestIterators.h"

#include "Foundation/Algorithms/Move.h"

namespace
{
    class A
    {
    public:
        A() = default;
        explicit A(int x)
            : Value(x)
        {
        }

        A(const A& object)
            : Value(object.Value)
        {
        }

        A(A&& object)
            : Value(object.Value)
        {
            object.Value = 0;
        }

    public:
        A& operator=(const A& object)
        {
            Value = object.Value;
            return *this;
        }

        A& operator=(A&& object)
        {
            Value = object.Value;
            object.Value = 0;

            return *this;
        }

    public:
        int Value;
    };
}

using namespace Kitsune;
using namespace Kitsune::Testing;

TEST(MoveTests, Move)
{
    ForwardTestContainer<A, 5> container = { A(2), A(3), A(1), A(4), A(6) };
    ForwardTestContainer<A, 5> destContainer = { A(54), A(1), A(2), A(6), A(3) };

    auto it = Algorithms::Move(container.GetBegin(), container.GetEnd(),
                               destContainer.GetBegin());

    EXPECT_EQ(it, destContainer.GetEnd());

    EXPECT_EQ(container[0].Value, 0);
    EXPECT_EQ(container[1].Value, 0);
    EXPECT_EQ(container[2].Value, 0);
    EXPECT_EQ(container[3].Value, 0);
    EXPECT_EQ(container[4].Value, 0);

    EXPECT_EQ(destContainer[0].Value, 2);
    EXPECT_EQ(destContainer[1].Value, 3);
    EXPECT_EQ(destContainer[2].Value, 1);
    EXPECT_EQ(destContainer[3].Value, 4);
    EXPECT_EQ(destContainer[4].Value, 6);
}

TEST(MoveTests, MoveN)
{
    ForwardTestContainer<A, 5> container = { A(2), A(3), A(1), A(4), A(6) };
    ForwardTestContainer<A, 5> destContainer = { A(54), A(1), A(2), A(6), A(3) };

    auto it = Algorithms::MoveN(container.GetBegin(), 5,
                               destContainer.GetBegin());

    EXPECT_EQ(it, destContainer.GetEnd());

    EXPECT_EQ(container[0].Value, 0);
    EXPECT_EQ(container[1].Value, 0);
    EXPECT_EQ(container[2].Value, 0);
    EXPECT_EQ(container[3].Value, 0);
    EXPECT_EQ(container[4].Value, 0);

    EXPECT_EQ(destContainer[0].Value, 2);
    EXPECT_EQ(destContainer[1].Value, 3);
    EXPECT_EQ(destContainer[2].Value, 1);
    EXPECT_EQ(destContainer[3].Value, 4);
    EXPECT_EQ(destContainer[4].Value, 6);
}

TEST(MoveTests, MoveBackwards)
{
    BidirectionalTestContainer<A, 5> container = { A(2), A(3), A(1), A(4), A(6) };
    BidirectionalTestContainer<A, 5> destContainer = { A(54), A(1), A(2), A(6), A(3) };

    auto it = Algorithms::MoveBackwards(container.GetBegin(), container.GetEnd(),
                                        destContainer.GetEnd());

    EXPECT_EQ(it, destContainer.GetBegin());

    EXPECT_EQ(container[0].Value, 0);
    EXPECT_EQ(container[1].Value, 0);
    EXPECT_EQ(container[2].Value, 0);
    EXPECT_EQ(container[3].Value, 0);
    EXPECT_EQ(container[4].Value, 0);

    EXPECT_EQ(destContainer[0].Value, 2);
    EXPECT_EQ(destContainer[1].Value, 3);
    EXPECT_EQ(destContainer[2].Value, 1);
    EXPECT_EQ(destContainer[3].Value, 4);
    EXPECT_EQ(destContainer[4].Value, 6);
}
