#include "Foundation/Maths/Vector2.h"

#include <random>
#include <chrono>

#include <gtest/gtest.h>
#include "Foundation/Common/Types.h"

#define EXPECT_TEMPLATED_EQ(value1, value2)          \
    if constexpr (std::is_floating_point_v<T>)       \
        EXPECT_FLOAT_EQ(T(value1), T(value2));       \
    else                                             \
        EXPECT_EQ(T(value1), T(value2))              \

using namespace Kitsune;

namespace
{
    template<typename T>
    struct A
    {
        T Number;
    };

    template<typename T>
    struct B
    {
        explicit B(T value)
            : Number(value)
        {
        }

        B(const A<T>& value)
            : Number(value.Number)
        {
        }

        B(A<T>&& value)
            : Number(std::exchange(value.Number, 0))
        {
        }

        B& operator=(const A<T>& value)
        {
            Number = value.Number;
            return *this;
        }

        B& operator=(A<T>&& value)
        {
            Number = std::exchange(value.Number, 0);
            return *this;
        }

        T Number;
    };
}

template<typename T>
class Vector2Tests : public ::testing::Test
{
public:
    using ValueType = T;

protected:
    Vector2Tests()
        : m_RandomEngine(std::chrono::high_resolution_clock()
                            .now()
                            .time_since_epoch()
                            .count())
    {
    }

protected:
    T GetRandomValue()
    {
        if constexpr (std::is_floating_point_v<T>)
        {
            auto dist = std::uniform_real_distribution<T>(0, 120.923);
            return dist(m_RandomEngine);
        }
        else
        {
            auto dist = std::uniform_int_distribution<T>(0);
            return dist(m_RandomEngine);
        }
    }

private:
    std::mt19937 m_RandomEngine;
};

using Vector2TestsImpl = ::testing::Types<
    Uint16, Uint32, Uint64,
    Int16, Int32, Int64,
    float, double>;

TYPED_TEST_SUITE(Vector2Tests, Vector2TestsImpl);

TYPED_TEST(Vector2Tests, DefaultConstructor)
{
    using T = typename TestFixture::ValueType;

    Vector2<T> vector;
    EXPECT_TEMPLATED_EQ(vector.x, T(0));
    EXPECT_TEMPLATED_EQ(vector.y, T(0));
}

TYPED_TEST(Vector2Tests, ScalarConstructor)
{
    using T = typename TestFixture::ValueType;

    T scalar(this->GetRandomValue());
    Vector2<T> vector(scalar);

    EXPECT_TEMPLATED_EQ(vector.x, scalar);
    EXPECT_TEMPLATED_EQ(vector.y, scalar);
}

TYPED_TEST(Vector2Tests, XYConstructor)
{
    using T = typename TestFixture::ValueType;

    T scalarX(this->GetRandomValue());
    T scalarY(this->GetRandomValue());
    Vector2<T> vector(scalarX, scalarY);

    EXPECT_TEMPLATED_EQ(vector.x, scalarX);
    EXPECT_TEMPLATED_EQ(vector.y, scalarY);
}

TYPED_TEST(Vector2Tests, TemplatedCopy)
{
    using T = typename TestFixture::ValueType;

    T scalarX(this->GetRandomValue());
    T scalarY(this->GetRandomValue());

    Vector2<T> vector(scalarX, scalarY);
    Vector2<T> copy = vector;

    EXPECT_TEMPLATED_EQ(copy.x, scalarX);
    EXPECT_TEMPLATED_EQ(copy.y, scalarY);
}

TYPED_TEST(Vector2Tests, TemplatedMove)
{
    using T = typename TestFixture::ValueType;

    Vector2<A<T>> vector(A<T>(3), A<T>(54));
    Vector2<B<T>> copy = std::move(vector);

    EXPECT_TEMPLATED_EQ(copy.x.Number, 3);
    EXPECT_TEMPLATED_EQ(copy.y.Number, 54);

    EXPECT_TEMPLATED_EQ(vector.x.Number, 0);
    EXPECT_TEMPLATED_EQ(vector.y.Number, 0);
}

TYPED_TEST(Vector2Tests, TemplatedCopyAssign)
{
    using T = typename TestFixture::ValueType;

    T scalarX(this->GetRandomValue());
    T scalarY(this->GetRandomValue());

    Vector2<A<T>> vector = { A<T>(scalarX), A<T>(scalarY) };
    Vector2<B<T>> copy = { B<T>(this->GetRandomValue()),
                           B<T>(this->GetRandomValue()) };

    copy = vector;

    EXPECT_TEMPLATED_EQ(copy.x.Number, scalarX);
    EXPECT_TEMPLATED_EQ(copy.y.Number, scalarY);
}

TYPED_TEST(Vector2Tests, TemplatedMoveAssign)
{
    using T = typename TestFixture::ValueType;

    T scalarX(this->GetRandomValue());
    T scalarY(this->GetRandomValue());

    Vector2<A<T>> vector = { A<T>(scalarX), A<T>(scalarY) };
    Vector2<B<T>> moved = { B<T>(this->GetRandomValue()),
                            B<T>(this->GetRandomValue()) };

    moved = std::move(vector);

    EXPECT_TEMPLATED_EQ(moved.x.Number, scalarX);
    EXPECT_TEMPLATED_EQ(moved.y.Number, scalarY);

    EXPECT_TEMPLATED_EQ(vector.x.Number, 0);
    EXPECT_TEMPLATED_EQ(vector.y.Number, 0);
}

TYPED_TEST(Vector2Tests, Subscript)
{
    using T = typename TestFixture::ValueType;
    Vector2<T> vector = { this->GetRandomValue(),
                          this->GetRandomValue() };

    EXPECT_EQ(&vector[0], &vector.x);
    EXPECT_EQ(&vector[1], &vector.y);
}

TYPED_TEST(Vector2Tests, Negate)
{
    using T = typename TestFixture::ValueType;
    if constexpr (std::is_signed_v<T>)
    {
        T scalarX(this->GetRandomValue());
        T scalarY(this->GetRandomValue());

        Vector2<T> vector = { scalarX, scalarY };
        Vector2<T> neg = -vector;

        EXPECT_TEMPLATED_EQ(neg.x, -scalarX);
        EXPECT_TEMPLATED_EQ(neg.y, -scalarY);
    }
}

TYPED_TEST(Vector2Tests, AssignmentOperators)
{
    using T = typename TestFixture::ValueType;

    T scalarX(this->GetRandomValue());
    T scalarY(this->GetRandomValue());

    Vector2<T> vector = { scalarX, scalarY };
    Vector2<T> vector2 = { this->GetRandomValue(),
                           this->GetRandomValue() };

    Vector2<T> vector3 = { this->GetRandomValue(),
                           this->GetRandomValue() };

    vector /= 4;
    EXPECT_TEMPLATED_EQ(vector.x, scalarX /= 4);
    EXPECT_TEMPLATED_EQ(vector.y, scalarY /= 4);

    vector *= 7;
    EXPECT_TEMPLATED_EQ(vector.x, scalarX *= 7);
    EXPECT_TEMPLATED_EQ(vector.y, scalarY *= 7);

    vector -= vector2;
    EXPECT_TEMPLATED_EQ(vector.x, scalarX -= vector2.x);
    EXPECT_TEMPLATED_EQ(vector.y, scalarY -= vector2.y);

    vector += vector2;
    EXPECT_TEMPLATED_EQ(vector.x, scalarX += vector2.x);
    EXPECT_TEMPLATED_EQ(vector.y, scalarY += vector2.y);

    vector *= vector3;
    EXPECT_TEMPLATED_EQ(vector.x, scalarX *= vector3.x);
    EXPECT_TEMPLATED_EQ(vector.y, scalarY *= vector3.y);

    vector /= vector3;
    EXPECT_TEMPLATED_EQ(vector.x, scalarX /= vector3.x);
    EXPECT_TEMPLATED_EQ(vector.y, scalarY /= vector3.y);
}

TYPED_TEST(Vector2Tests, ArithmeticOperators)
{
    using T = typename TestFixture::ValueType;

    T scalarX = this->GetRandomValue();
    T scalarY = this->GetRandomValue();

    T scalar = this->GetRandomValue();

    Vector2<T> vector = { scalarX, scalarY };
    Vector2<T> vector2 = { this->GetRandomValue(),
                           this->GetRandomValue() };

    Vector2<T> scalarDivValue = vector / scalar;
    EXPECT_TEMPLATED_EQ(scalarDivValue.x, vector.x / scalar);
    EXPECT_TEMPLATED_EQ(scalarDivValue.y, vector.y / scalar);

    Vector2<T> scalarMulValue = vector * scalar;
    EXPECT_TEMPLATED_EQ(scalarMulValue.x, vector.x * scalar);
    EXPECT_TEMPLATED_EQ(scalarMulValue.y, vector.y * scalar);

    Vector2<T> subValue = vector - vector2;
    EXPECT_TEMPLATED_EQ(subValue.x, vector.x - vector2.x);
    EXPECT_TEMPLATED_EQ(subValue.y, vector.y - vector2.y);

    Vector2<T> addValue = vector + vector2;
    EXPECT_TEMPLATED_EQ(addValue.x, vector.x + vector2.x);
    EXPECT_TEMPLATED_EQ(addValue.y, vector.y + vector2.y);

    Vector2<T> mulValue = vector * vector2;
    EXPECT_TEMPLATED_EQ(mulValue.x, vector.x * vector2.x);
    EXPECT_TEMPLATED_EQ(mulValue.y, vector.y * vector2.y);

    Vector2<T> divValue = vector / vector2;
    EXPECT_TEMPLATED_EQ(divValue.x, vector.x / vector2.x);
    EXPECT_TEMPLATED_EQ(divValue.y, vector.y / vector2.y);

    Vector2<T> flippedDivValue = scalar / vector;
    EXPECT_TEMPLATED_EQ(flippedDivValue.x, scalar / vector.x);
    EXPECT_TEMPLATED_EQ(flippedDivValue.y, scalar / vector.y);

    Vector2<T> flippedMulValue = scalar * vector;
    EXPECT_TEMPLATED_EQ(flippedMulValue.x, vector.x * scalar);
    EXPECT_TEMPLATED_EQ(flippedMulValue.y, vector.y * scalar);
}

TYPED_TEST(Vector2Tests, RangedForLoop)
{
    using T = typename TestFixture::ValueType;

    Vector2<T> vector;
    Index index = 0;

    for (T dimension : vector)
    {
        EXPECT_TEMPLATED_EQ(dimension, vector.Data[index]);
        ++index;
    }
}

TYPED_TEST(Vector2Tests, EqualityOperators)
{
    using T = typename TestFixture::ValueType;

    T scalarX = this->GetRandomValue();
    T scalarY = this->GetRandomValue();

    Vector2<T> vector = { scalarX, scalarY };
    Vector2<T> equalVector = { scalarX, scalarY };
    Vector2<T> unequalVector = { this->GetRandomValue(), this->GetRandomValue() };

    while ((unequalVector.x == scalarX) && (unequalVector.y == scalarY))
    {
        unequalVector.x = this->GetRandomValue();
        unequalVector.y = this->GetRandomValue();
    }

    EXPECT_TRUE(vector == equalVector);
    EXPECT_FALSE(vector == unequalVector);

    EXPECT_FALSE(vector != equalVector);
    EXPECT_TRUE(vector != unequalVector);
}
