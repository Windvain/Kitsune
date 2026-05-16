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
    EXPECT_TEMPLATED_EQ(vector.X, T(0));
    EXPECT_TEMPLATED_EQ(vector.Y, T(0));
}

TYPED_TEST(Vector2Tests, ScalarConstructor)
{
    using T = typename TestFixture::ValueType;

    T scalar(this->GetRandomValue());
    Vector2<T> vector(scalar);

    EXPECT_TEMPLATED_EQ(vector.X, scalar);
    EXPECT_TEMPLATED_EQ(vector.Y, scalar);
}

TYPED_TEST(Vector2Tests, XYConstructor)
{
    using T = typename TestFixture::ValueType;

    T scalarX(this->GetRandomValue());
    T scalarY(this->GetRandomValue());
    Vector2<T> vector(scalarX, scalarY);

    EXPECT_TEMPLATED_EQ(vector.X, scalarX);
    EXPECT_TEMPLATED_EQ(vector.Y, scalarY);
}

TYPED_TEST(Vector2Tests, TemplatedCopy)
{
    using T = typename TestFixture::ValueType;

    T scalarX(this->GetRandomValue());
    T scalarY(this->GetRandomValue());

    Vector2<T> vector(scalarX, scalarY);
    Vector2<T> copy = vector;

    EXPECT_TEMPLATED_EQ(copy.X, scalarX);
    EXPECT_TEMPLATED_EQ(copy.Y, scalarY);
}

TYPED_TEST(Vector2Tests, TemplatedMove)
{
    using T = typename TestFixture::ValueType;

    Vector2<A<T>> vector(A<T>(3), A<T>(54));
    Vector2<B<T>> copy = std::move(vector);

    EXPECT_TEMPLATED_EQ(copy.X.Number, 3);
    EXPECT_TEMPLATED_EQ(copy.Y.Number, 54);

    EXPECT_TEMPLATED_EQ(vector.X.Number, 0);
    EXPECT_TEMPLATED_EQ(vector.Y.Number, 0);
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

    EXPECT_TEMPLATED_EQ(copy.X.Number, scalarX);
    EXPECT_TEMPLATED_EQ(copy.Y.Number, scalarY);
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

    EXPECT_TEMPLATED_EQ(moved.X.Number, scalarX);
    EXPECT_TEMPLATED_EQ(moved.Y.Number, scalarY);

    EXPECT_TEMPLATED_EQ(vector.X.Number, 0);
    EXPECT_TEMPLATED_EQ(vector.Y.Number, 0);
}

TYPED_TEST(Vector2Tests, Subscript)
{
    using T = typename TestFixture::ValueType;
    Vector2<T> vector = { this->GetRandomValue(),
                          this->GetRandomValue() };

    EXPECT_EQ(&vector[0], &vector.X);
    EXPECT_EQ(&vector[1], &vector.Y);
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

        EXPECT_TEMPLATED_EQ(neg.X, -scalarX);
        EXPECT_TEMPLATED_EQ(neg.Y, -scalarY);
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
    EXPECT_TEMPLATED_EQ(vector.X, scalarX /= 4);
    EXPECT_TEMPLATED_EQ(vector.Y, scalarY /= 4);

    vector *= 7;
    EXPECT_TEMPLATED_EQ(vector.X, scalarX *= 7);
    EXPECT_TEMPLATED_EQ(vector.Y, scalarY *= 7);

    vector -= vector2;
    EXPECT_TEMPLATED_EQ(vector.X, scalarX -= vector2.X);
    EXPECT_TEMPLATED_EQ(vector.Y, scalarY -= vector2.Y);

    vector += vector2;
    EXPECT_TEMPLATED_EQ(vector.X, scalarX += vector2.X);
    EXPECT_TEMPLATED_EQ(vector.Y, scalarY += vector2.Y);

    vector *= vector3;
    EXPECT_TEMPLATED_EQ(vector.X, scalarX *= vector3.X);
    EXPECT_TEMPLATED_EQ(vector.Y, scalarY *= vector3.Y);

    vector /= vector3;
    EXPECT_TEMPLATED_EQ(vector.X, scalarX /= vector3.X);
    EXPECT_TEMPLATED_EQ(vector.Y, scalarY /= vector3.Y);
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
    EXPECT_TEMPLATED_EQ(scalarDivValue.X, vector.X / scalar);
    EXPECT_TEMPLATED_EQ(scalarDivValue.Y, vector.Y / scalar);

    Vector2<T> scalarMulValue = vector * scalar;
    EXPECT_TEMPLATED_EQ(scalarMulValue.X, vector.X * scalar);
    EXPECT_TEMPLATED_EQ(scalarMulValue.Y, vector.Y * scalar);

    Vector2<T> subValue = vector - vector2;
    EXPECT_TEMPLATED_EQ(subValue.X, vector.X - vector2.X);
    EXPECT_TEMPLATED_EQ(subValue.Y, vector.Y - vector2.Y);

    Vector2<T> addValue = vector + vector2;
    EXPECT_TEMPLATED_EQ(addValue.X, vector.X + vector2.X);
    EXPECT_TEMPLATED_EQ(addValue.Y, vector.Y + vector2.Y);

    Vector2<T> mulValue = vector * vector2;
    EXPECT_TEMPLATED_EQ(mulValue.X, vector.X * vector2.X);
    EXPECT_TEMPLATED_EQ(mulValue.Y, vector.Y * vector2.Y);

    Vector2<T> divValue = vector / vector2;
    EXPECT_TEMPLATED_EQ(divValue.X, vector.X / vector2.X);
    EXPECT_TEMPLATED_EQ(divValue.Y, vector.Y / vector2.Y);

    Vector2<T> flippedDivValue = scalar / vector;
    EXPECT_TEMPLATED_EQ(flippedDivValue.X, scalar / vector.X);
    EXPECT_TEMPLATED_EQ(flippedDivValue.Y, scalar / vector.Y);

    Vector2<T> flippedMulValue = scalar * vector;
    EXPECT_TEMPLATED_EQ(flippedMulValue.X, vector.X * scalar);
    EXPECT_TEMPLATED_EQ(flippedMulValue.Y, vector.Y * scalar);
}

TYPED_TEST(Vector2Tests, EqualityOperators)
{
    using T = typename TestFixture::ValueType;

    T scalarX = this->GetRandomValue();
    T scalarY = this->GetRandomValue();

    Vector2<T> vector = { scalarX, scalarY };
    Vector2<T> equalVector = { scalarX, scalarY };
    Vector2<T> unequalVector = { this->GetRandomValue(), this->GetRandomValue() };

    while ((unequalVector.X == scalarX) && (unequalVector.Y == scalarY))
    {
        unequalVector.X = this->GetRandomValue();
        unequalVector.Y = this->GetRandomValue();
    }

    EXPECT_TRUE(vector == equalVector);
    EXPECT_FALSE(vector == unequalVector);

    EXPECT_FALSE(vector != equalVector);
    EXPECT_TRUE(vector != unequalVector);
}
