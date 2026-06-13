#include <gtest/gtest.h>
#include "TestContainer.h"

#include "Foundation/Templates/Swap.h"
#include "Foundation/Algorithms/Swap.h"

namespace
{
    using namespace Kitsune;
    using namespace Kitsune::Testing;

    class SwapMemberObject
    {
    public:
        inline explicit SwapMemberObject(int value)
            : Value(value)
        {
        }

        // Swaps the two values and increment them by one.
        inline void Swap(SwapMemberObject& object)
        {
            ++Value;
            ++object.Value;

            std::swap(Value, object.Value);
        }

    public:
        int Value = 0;
    };

    // Swap(const T&, const T&) -> x.Swap(const T&)
    TEST(SwapTests, SwapWithMemberFunction)
    {
        int value1 = 283;
        int value2 = 123;

        SwapMemberObject object1(value1);
        SwapMemberObject object2(value2);

        Swap(object1, object2);

        EXPECT_EQ(object1.Value, value2 + 1);
        EXPECT_EQ(object2.Value, value1 + 1);
    }

    // Swap(const T&, const T&) -> Move(x)
    TEST(SwapTests, SwapWithoutMemberFunction)
    {
        int value1 = 283;
        int value2 = 123;

        Swap(value1, value2);

        EXPECT_EQ(value1, 123);
        EXPECT_EQ(value2, 283);
    }

    // Algorithms::Swap(Iter, Iter, OutIter)
    TEST(SwapTests, SwapAlgorithm)
    {
        ForwardTestContainer<SwapMemberObject, 4> container = {
            SwapMemberObject(23),
            SwapMemberObject(32),
            SwapMemberObject(12),
            SwapMemberObject(234)
        };

        ForwardTestContainer<SwapMemberObject, 4> output = {
            SwapMemberObject(234),
            SwapMemberObject(76),
            SwapMemberObject(91),
            SwapMemberObject(444)
        };

        Algorithms::Swap(container.GetBegin(), container.GetEnd(), output.GetBegin());

        EXPECT_EQ(container[0].Value, 235);
        EXPECT_EQ(container[1].Value, 77);
        EXPECT_EQ(container[2].Value, 92);
        EXPECT_EQ(container[3].Value, 445);

        EXPECT_EQ(output[0].Value, 24);
        EXPECT_EQ(output[1].Value, 33);
        EXPECT_EQ(output[2].Value, 13);
        EXPECT_EQ(output[3].Value, 235);
    }
}
