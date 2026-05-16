#include <gtest/gtest.h>
#include "Foundation/Containers/Pair.h"

using namespace Kitsune;

namespace
{
    template<typename T>
    class MyObject
    {
    public:
        MyObject(T value)
            : Value(value)
        {
        }

        MyObject(MyObject&& object)
            : Value(std::exchange(object.Value, 0))
        {
        }

        MyObject& operator=(MyObject&& object)
        {
            Value = std::exchange(object.Value, 0);
            return *this;
        }

    public:
        T Value;
    };
}

TEST(PairTests, DefaultConstructor)
{
    Pair<int, float> pair;
    EXPECT_EQ(pair.First, 0);
    EXPECT_EQ(pair.Second, 0);
}

TEST(PairTests, ValueConstrutor)
{
    Pair<int, float> pair(int(32), 3.4f);
    EXPECT_EQ(pair.First, 32);
    EXPECT_FLOAT_EQ(pair.Second, 3.4f);
}

TEST(PairTests, ForwardingConstructor)
{
    long x = 34;
    Pair<int, float> pair(x, long(12));

    EXPECT_EQ(pair.First, x);
    EXPECT_FLOAT_EQ(pair.Second, 12);
}

TEST(PairTests, CopyPairConstructor)
{
    Pair<int, long> pair = { 23, 55 };
    Pair<float, double> copy(pair);

    EXPECT_EQ(copy.First, 23);
    EXPECT_EQ(copy.Second, 55);
}

TEST(PairTests, MovePairConstructor)
{
    Pair<MyObject<int>, long> pair = { 23, 55 };
    Pair<MyObject<int>, double> move(std::move(pair));

    EXPECT_EQ(move.First.Value, 23);
    EXPECT_EQ(move.Second, 55);

    EXPECT_EQ(pair.First.Value, 0);
    EXPECT_EQ(pair.Second, 55);
}

/* Pair(const Pair&), Pair(Pair&&) and its assignment operators are defaulted,
 * so no tests.
 **/

TEST(PairTests, PairCopyAssign)
{
    Pair<int, long> pair = { 23, 55 };
    Pair<float, double> copy = { 2.0f, 3231 };

    copy = pair;

    EXPECT_EQ(copy.First, 23);
    EXPECT_EQ(copy.Second, 55);
}

TEST(PairTests, PairMoveAssign)
{
    Pair<MyObject<int>, long> pair = { 23, 55 };
    Pair<MyObject<int>, double> move = { 392, 93.2f };

    move = std::move(pair);

    EXPECT_EQ(move.First.Value, 23);
    EXPECT_EQ(move.Second, 55);

    EXPECT_EQ(pair.First.Value, 0);
    EXPECT_EQ(pair.Second, 55);
}

TEST(PairTests, Swap)
{
    Pair<int, float> pair1 = { 2, 6.5f };
    Pair<int, float> pair2 = { 5, 23.54f };

    pair1.Swap(pair2);

    EXPECT_EQ(pair1.First, 5);
    EXPECT_FLOAT_EQ(pair1.Second, 23.54f);

    EXPECT_EQ(pair2.First, 2);
    EXPECT_FLOAT_EQ(pair2.Second, 6.5f);
}

TEST(PairTests, Equal)
{
    Pair<int, long> pair = { 3, 4 };
    Pair<long long, short> equalPair = { 3, 4 };
    Pair<short, char> inequalPair = { 4, 5 };

    EXPECT_TRUE(pair == equalPair);
    EXPECT_FALSE(pair == inequalPair);
}
