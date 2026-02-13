#include <gtest/gtest.h>

#include "TestContainer.h"
#include "TestIterators.h"

#include "Foundation/Algorithms/Copy.h"

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
            : Value(object.Value + 1)
        {
        }

    public:
        A& operator=(const A& object)
        {
            Value = object.Value + 1;
            return *this;
        }

    public:
        int Value;
    };
}

using namespace Kitsune;
using namespace Kitsune::Testing;

TEST(CopyTests, Copy)
{
    ForwardTestContainer<A, 5> container = { A(2), A(3), A(1), A(4), A(6) };
    ForwardTestContainer<A, 5> destContainer = { A(54), A(1), A(2), A(6), A(3) };

    auto it = Algorithms::Copy(container.GetBegin(), container.GetEnd(),
                               destContainer.GetBegin());

    EXPECT_EQ(it, destContainer.GetEnd());

    EXPECT_EQ(container[0].Value, 2);
    EXPECT_EQ(container[1].Value, 3);
    EXPECT_EQ(container[2].Value, 1);
    EXPECT_EQ(container[3].Value, 4);
    EXPECT_EQ(container[4].Value, 6);

    EXPECT_EQ(destContainer[0].Value, 3);
    EXPECT_EQ(destContainer[1].Value, 4);
    EXPECT_EQ(destContainer[2].Value, 2);
    EXPECT_EQ(destContainer[3].Value, 5);
    EXPECT_EQ(destContainer[4].Value, 7);
}

TEST(CopyTests, CopyN)
{
    ForwardTestContainer<A, 5> container = { A(2), A(3), A(1), A(4), A(6) };
    ForwardTestContainer<A, 5> destContainer = { A(54), A(1), A(2), A(6), A(3) };

    auto it = Algorithms::CopyN(container.GetBegin(), 5, destContainer.GetBegin());
    EXPECT_EQ(it, destContainer.GetEnd());

    EXPECT_EQ(container[0].Value, 2);
    EXPECT_EQ(container[1].Value, 3);
    EXPECT_EQ(container[2].Value, 1);
    EXPECT_EQ(container[3].Value, 4);
    EXPECT_EQ(container[4].Value, 6);

    EXPECT_EQ(destContainer[0].Value, 3);
    EXPECT_EQ(destContainer[1].Value, 4);
    EXPECT_EQ(destContainer[2].Value, 2);
    EXPECT_EQ(destContainer[3].Value, 5);
    EXPECT_EQ(destContainer[4].Value, 7);
}

TEST(CopyTests, CopyBackwards)
{
    BidirectionalTestContainer<A, 5> container = { A(2), A(3), A(1), A(4), A(6) };
    BidirectionalTestContainer<A, 5> destContainer = { A(54), A(1), A(2), A(6), A(3) };

    auto it = Algorithms::CopyBackwards(container.GetBegin(), container.GetEnd(),
                                        destContainer.GetEnd());

    EXPECT_EQ(it, destContainer.GetBegin());

    EXPECT_EQ(container[0].Value, 2);
    EXPECT_EQ(container[1].Value, 3);
    EXPECT_EQ(container[2].Value, 1);
    EXPECT_EQ(container[3].Value, 4);
    EXPECT_EQ(container[4].Value, 6);

    EXPECT_EQ(destContainer[0].Value, 3);
    EXPECT_EQ(destContainer[1].Value, 4);
    EXPECT_EQ(destContainer[2].Value, 2);
    EXPECT_EQ(destContainer[3].Value, 5);
    EXPECT_EQ(destContainer[4].Value, 7);
}
