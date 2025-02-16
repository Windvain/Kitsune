#include <gtest/gtest.h>
#include "Foundation/Maths/Vector2.h"

using namespace Kitsune;

namespace Vector2Testing
{
    struct G
    {
        int y;
    };

    struct I
    {
        I(int x) : x(x) { /* ... */ }
        I(G&& integer)
            : x(std::exchange(integer.y, 0))
        {
        }

        int x;
    };
}

using namespace Vector2Testing;

TEST(Vector2Tests, DefaultCtor)
{
    Vector2<Int64> vec;
    EXPECT_EQ(vec.x, 0);
    EXPECT_EQ(vec.y, 0);
}

TEST(Vector2Tests, ScalarCtor)
{
    auto vec = Vector2<Uint64>(432);
    EXPECT_EQ(vec.x, 432);
    EXPECT_EQ(vec.y, 432);
}

TEST(Vector2Tests, TemplatedCopy)
{
    const auto vec = Vector2<int>(3, 54);
    Vector2<Int64> copy = vec;

    EXPECT_EQ(copy.x, 3);
    EXPECT_EQ(copy.y, 54);
}

TEST(Vector2Tests, TemplatedMove)
{
    auto vec = Vector2<G>(G(3), G(54));
    Vector2<I> copy = std::move(vec);

    EXPECT_EQ(copy.x.x, 3);
    EXPECT_EQ(copy.y.x, 54);

    EXPECT_EQ(vec.x.y, 0);
    EXPECT_EQ(vec.y.y, 0);
}

TEST(Vector2Tests, TemplatedCopyAssign)
{
    const auto vec = Vector2<int>(3, 54);
    Vector2<Int64> copy = { 7, 4 };

    copy = vec;

    EXPECT_EQ(copy.x, 3);
    EXPECT_EQ(copy.y, 54);
}

TEST(Vector2Tests, TemplatedMoveAssign)
{
    auto vec = Vector2<G>(G(3), G(54));
    Vector2<I> copy = { I(5), I(123) };

    copy = std::move(vec);

    EXPECT_EQ(copy.x.x, 3);
    EXPECT_EQ(copy.y.x, 54);

    EXPECT_EQ(vec.x.y, 0);
    EXPECT_EQ(vec.y.y, 0);
}

TEST(Vector2Tests, Subscript)
{
    auto vec = Vector2<int>(3, 2);
    EXPECT_EQ(vec[0], 3);
    EXPECT_EQ(vec[1], 2);
}

TEST(Vector2Tests, Negate)
{
    Vector2<int> vec = { 43, 54 };
    Vector2<int> neg = -vec;

    EXPECT_EQ(neg.x, -43);
    EXPECT_EQ(neg.y, -54);
}

TEST(Vector2Tests, OperatorAssigns)
{
    Vector2<int> vec = { 28, 80 };

    vec /= 4;
    EXPECT_EQ(vec.x, 7);
    EXPECT_EQ(vec.y, 20);

    vec *= (unsigned long)7;
    EXPECT_EQ(vec.x, 49);
    EXPECT_EQ(vec.y, 140);

    vec -= Vector2<unsigned>(48, 70);
    EXPECT_EQ(vec.x, 1);
    EXPECT_EQ(vec.y, 70);

    vec += Vector2<long>(3, 10);
    EXPECT_EQ(vec.x, 4);
    EXPECT_EQ(vec.y, 80);

    vec *= Vector2<short>(8, 2);
    EXPECT_EQ(vec.x, 32);
    EXPECT_EQ(vec.y, 160);

    vec /= Vector2<unsigned long long>(16, 10);
    EXPECT_EQ(vec.x, 2);
    EXPECT_EQ(vec.y, 16);
}

TEST(Vector2Tests, ArithmeticOperators)
{
    Vector2<int> vec = { 28, 80 };
    Vector2<int> divScalar = (vec / 4);

    EXPECT_EQ(divScalar.x, 7);
    EXPECT_EQ(divScalar.y, 20);

    Vector2<int> mulScalar = (divScalar * 7);
    EXPECT_EQ(mulScalar.x, 49);
    EXPECT_EQ(mulScalar.y, 140);

    Vector2<int> sub = (mulScalar - Vector2<unsigned>(48, 70));
    EXPECT_EQ(sub.x, 1);
    EXPECT_EQ(sub.y, 70);

    Vector2<int> add = (sub + Vector2<long>(3, 10));
    EXPECT_EQ(add.x, 4);
    EXPECT_EQ(add.y, 80);

    Vector2<int> mulVec = (add * Vector2<short>(8, 2));
    EXPECT_EQ(mulVec.x, 32);
    EXPECT_EQ(mulVec.y, 160);

    Vector2<int> divVec = (mulVec / Vector2<unsigned long long>(16, 10));
    EXPECT_EQ(divVec.x, 2);
    EXPECT_EQ(divVec.y, 16);
}

TEST(Vector2Tests, RangedForLoop)
{
    Vector2<int> vec;
    int index = 0;

    for (int dim : vec)
    {
        EXPECT_EQ(dim, vec.Data[index]);
        ++index;
    }
}

TEST(Vector2Tests, FlippedMul)
{
    Vector2<int> vec = 2 * Vector2<int>(7, 2);
    EXPECT_EQ(vec.x, 14);
    EXPECT_EQ(vec.y, 4);
}

TEST(Vector2Tests, FlippedDiv)
{
    Vector2<int> vec = 24 / Vector2<int>(2, 8);
    EXPECT_EQ(vec.x, 12);
    EXPECT_EQ(vec.y, 3);
}

TEST(Vector2Tests, Equal)
{
    Vector2<int> vec = { 1409, 5354 };
    Vector2<int> eq = { 1409, 5354 };
    Vector2<int> neq = { 21409, 5354 };

    EXPECT_TRUE(vec == eq);
    EXPECT_FALSE(vec == neq);

    EXPECT_FALSE(vec != eq);
    EXPECT_TRUE(vec != neq);
}
