#include <gtest/gtest.h>
#include "Foundation/Containers/Pair.h"

namespace
{
    using namespace Kitsune;

    // Pair<T, U>::Pair()
    TEST(PairTest, DefaultConstructor)
    {
        Pair<int, float> pair;
        EXPECT_EQ(pair.First, 0);
        EXPECT_EQ(pair.Second, 0);
    }

    // Pair<T, U>::Pair(const T&, const U&)
    TEST(PairTest, ValueConstructor)
    {
        Pair<int, float> pair(int(32), 3.4f);
        EXPECT_EQ(pair.First, 32);
        EXPECT_FLOAT_EQ(pair.Second, 3.4f);
    }

    // Pair<T, U>::Pair(T2&&, U2&&)
    TEST(PairTest, ForwardingConstructor)
    {
        long x = 34;
        Pair<int, float> pair(x, long(12));

        EXPECT_EQ(pair.First, x);
        EXPECT_FLOAT_EQ(pair.Second, 12);
    }

    // Pair<T, U>::Pair(const Pair<T2, U2>&)
    TEST(PairTest, CopyPairConstructor)
    {
        Pair<int, long> pair = { 23, 55 };
        Pair<float, double> copy(pair);

        EXPECT_EQ(copy.First, 23);
        EXPECT_EQ(copy.Second, 55);
    }

    // Pair<T, U>::Pair(Pair<T2, U2>&&)
    TEST(PairTest, MovePairConstructor)
    {
        Pair<std::shared_ptr<int>, long> pair = { std::make_shared<int>(23), 55 };
        Pair<std::shared_ptr<int>, double> move(std::move(pair));

        EXPECT_EQ(*move.First, 23);
        EXPECT_EQ(move.Second, 55);

        EXPECT_EQ(pair.First.get(), nullptr);
        EXPECT_EQ(pair.Second, 55);
    }

    /* Pair(const Pair&), Pair(Pair&&) and its assignment operators are defaulted,
     * so no tests.
     */

    // Pair<T, U>::operator=(const Pair<T2, U2>&)
    TEST(PairTest, PairCopyAssign)
    {
        Pair<int, long> pair = { 23, 55 };
        Pair<float, double> copy = { 2.0f, 3231 };

        copy = pair;

        EXPECT_EQ(copy.First, 23);
        EXPECT_EQ(copy.Second, 55);
    }

    // Pair<T, U>::operator=(Pair<T2, U2>&&)
    TEST(PairTest, PairMoveAssign)
    {
        Pair<std::shared_ptr<int>, long> pair = { std::make_shared<int>(23), 55 };
        Pair<std::shared_ptr<int>, double> move = { std::make_shared<int>(392), 93.2f };

        move = std::move(pair);

        EXPECT_EQ(*move.First, 23);
        EXPECT_EQ(move.Second, 55);

        EXPECT_EQ(pair.First.get(), nullptr);
        EXPECT_EQ(pair.Second, 55);
    }

    // Pair<T, U>::Swap(const Pair&)
    TEST(PairTest, Swap)
    {
        Pair<int, float> pair1 = { 2, 6.5f };
        Pair<int, float> pair2 = { 5, 23.54f };

        pair1.Swap(pair2);

        EXPECT_EQ(pair1.First, 5);
        EXPECT_FLOAT_EQ(pair1.Second, 23.54f);

        EXPECT_EQ(pair2.First, 2);
        EXPECT_FLOAT_EQ(pair2.Second, 6.5f);
    }

    // Pair<T, U>::operator==(const Pair<T2, U2>&)
    TEST(PairTest, Equal)
    {
        Pair<int, long> pair = { 3, 4 };
        Pair<long long, short> equalPair = { 3, 4 };
        Pair<short, char> unequalPair = { 4, 5 };

        EXPECT_TRUE(pair == equalPair);
        EXPECT_FALSE(pair == unequalPair);
    }
}
