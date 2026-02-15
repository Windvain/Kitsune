#include "Foundation/Templates/Swap.h"
#include "Foundation/Algorithms/Swap.h"

#include <gtest/gtest.h>
#include "TestContainer.h"

using namespace Kitsune;
using namespace Kitsune::Testing;

namespace
{
    class A
    {
    public:
        explicit A(int value)
            : Value(value)
        {
        }

        void Swap(A& object)
        {
            Value = 2;
            object.Value = 2;
        }

    public:
        int Value;
    };

    class B
    {
    public:
        explicit B(int value)
            : Value(value)
        {
        }

        B(B&& object)
        {
            Value = object.Value;
        }

        B& operator=(B&& object)
        {
            Value = object.Value;
            return *this;
        }

    public:
        int Value;
    };
}

TEST(SwapTests, SwapWithMemberFunction)
{
    A object1(283);
    A object2(123);

    Swap(object1, object2);

    EXPECT_EQ(object1.Value, 2);
    EXPECT_EQ(object2.Value, 2);
}

TEST(SwapTests, SwapWithMoves)
{
    B object1(283);
    B object2(123);

    Swap(object1, object2);

    EXPECT_EQ(object1.Value, 123);
    EXPECT_EQ(object2.Value, 283);
}

TEST(SwapTests, SwapAlgorithm)
{
    ForwardTestContainer<B, 4> container = { B(23), B(32), B(12), B(234) };
    ForwardTestContainer<B, 4> output = { B(234), B(76), B(91), B(444) };

    Algorithms::Swap(container.GetBegin(), container.GetEnd(), output.GetBegin());

    EXPECT_EQ(container[0].Value, 234);
    EXPECT_EQ(container[1].Value, 76);
    EXPECT_EQ(container[2].Value, 91);
    EXPECT_EQ(container[3].Value, 444);

    EXPECT_EQ(output[0].Value, 23);
    EXPECT_EQ(output[1].Value, 32);
    EXPECT_EQ(output[2].Value, 12);
    EXPECT_EQ(output[3].Value, 234);
}
