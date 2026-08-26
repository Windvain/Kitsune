#include <random>
#include <chrono>
#include <gtest/gtest.h>

#include "Foundation/Common/Types.h"
#include "Foundation/Maths/Vector2.h"

#if 0
#define EXPECT_TEMPLATED_EQ(value1, value2)          \
    if constexpr (std::is_floating_point_v<T>)       \
        EXPECT_FLOAT_EQ(T(value1), T(value2));       \
    else                                             \
        EXPECT_EQ(T(value1), T(value2))              \

using namespace Kitsune;

namespace
{
    template<typename T>
    struct FromCast
    {
        T Number;
    };

    template<typename T>
    struct ToCast
    {
        explicit ToCast(T value)
            : Number(value)
        {
        }

        ToCast(const FromCast<T>& value)
            : Number(value.Number)
        {
        }

        ToCast(FromCast<T>&& value)
            : Number(std::exchange(value.Number, 0))
        {
        }

        ToCast& operator=(const FromCast<T>& value)
        {
            Number = value.Number;
            return *this;
        }

        ToCast& operator=(FromCast<T>&& value)
        {
            Number = std::exchange(value.Number, 0);
            return *this;
        }

        T Number;
    };

    template<typename T>
    class Vector2Test : public ::testing::Test
    {
    public:
        using ValueType = T;

    protected:
        inline Vector2Test()
            : m_RandomEngine(
                std::chrono::high_resolution_clock::now().time_since_epoch().count())
        {
        }

    protected:
        inline T GetRandomValue()
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

    using Vector2TestImpl = ::testing::Types<
        Uint16, Uint32, Uint64,
        Int16, Int32, Int64,
        float, double>;

    TYPED_TEST_SUITE(Vector2Test, Vector2TestImpl);

    // Vector2<T>::Vector2()
    TYPED_TEST(Vector2Test, DefaultConstructor)
    {
        using T = typename TestFixture::ValueType;

        Vector2<T> vector;
        EXPECT_TEMPLATED_EQ(vector.X, T(0));
        EXPECT_TEMPLATED_EQ(vector.Y, T(0));
    }

    // Vector2<T>::Vector2(const T&)
    TYPED_TEST(Vector2Test, ScalarConstructor)
    {
        using T = typename TestFixture::ValueType;

        T scalar(this->GetRandomValue());
        Vector2<T> vector(scalar);

        EXPECT_TEMPLATED_EQ(vector.X, scalar);
        EXPECT_TEMPLATED_EQ(vector.Y, scalar);
    }

    // Vector2<T>::Vector2(const T&, const T&)
    TYPED_TEST(Vector2Test, XYConstructor)
    {
        using T = typename TestFixture::ValueType;

        T scalarX(this->GetRandomValue());
        T scalarY(this->GetRandomValue());
        Vector2<T> vector(scalarX, scalarY);

        EXPECT_TEMPLATED_EQ(vector.X, scalarX);
        EXPECT_TEMPLATED_EQ(vector.Y, scalarY);
    }

    // Vector2<T>::Vector2(const Vector<U, 2>&)
    TYPED_TEST(Vector2Test, TemplatedCopy)
    {
        using T = typename TestFixture::ValueType;

        T scalarX(this->GetRandomValue());
        T scalarY(this->GetRandomValue());

        Vector2<T> vector(scalarX, scalarY);
        Vector2<T> copy = vector;

        EXPECT_TEMPLATED_EQ(copy.X, scalarX);
        EXPECT_TEMPLATED_EQ(copy.Y, scalarY);
    }

    // Vector2<T>::Vector2(Vector<U, 2>&&)
    TYPED_TEST(Vector2Test, TemplatedMove)
    {
        using T = typename TestFixture::ValueType;

        Vector2<FromCast<T>> vector(FromCast<T>(3), FromCast<T>(54));
        Vector2<ToCast<T>> copy = std::move(vector);

        EXPECT_TEMPLATED_EQ(copy.X.Number, 3);
        EXPECT_TEMPLATED_EQ(copy.Y.Number, 54);

        EXPECT_TEMPLATED_EQ(vector.X.Number, 0);
        EXPECT_TEMPLATED_EQ(vector.Y.Number, 0);
    }

    // Vector2<T>::operator=(const Vector<U, 2>&)
    TYPED_TEST(Vector2Test, TemplatedCopyAssign)
    {
        using T = typename TestFixture::ValueType;

        T scalarX(this->GetRandomValue());
        T scalarY(this->GetRandomValue());

        Vector2<FromCast<T>> vector = { FromCast<T>(scalarX), FromCast<T>(scalarY) };
        Vector2<ToCast<T>> copy = { ToCast<T>(this->GetRandomValue()),
                               ToCast<T>(this->GetRandomValue()) };

        copy = vector;

        EXPECT_TEMPLATED_EQ(copy.X.Number, scalarX);
        EXPECT_TEMPLATED_EQ(copy.Y.Number, scalarY);
    }

    // Vector2<T>::operator=(Vector<U, 2>&&)
    TYPED_TEST(Vector2Test, TemplatedMoveAssign)
    {
        using T = typename TestFixture::ValueType;

        T scalarX(this->GetRandomValue());
        T scalarY(this->GetRandomValue());

        Vector2<FromCast<T>> vector = { FromCast<T>(scalarX), FromCast<T>(scalarY) };
        Vector2<ToCast<T>> moved = { ToCast<T>(this->GetRandomValue()),
                                ToCast<T>(this->GetRandomValue()) };

        moved = std::move(vector);

        EXPECT_TEMPLATED_EQ(moved.X.Number, scalarX);
        EXPECT_TEMPLATED_EQ(moved.Y.Number, scalarY);

        EXPECT_TEMPLATED_EQ(vector.X.Number, 0);
        EXPECT_TEMPLATED_EQ(vector.Y.Number, 0);
    }

    // Vector2<T>::operator[](Index)
    TYPED_TEST(Vector2Test, Subscript)
    {
        using T = typename TestFixture::ValueType;
        Vector2<T> vector = { this->GetRandomValue(),
                              this->GetRandomValue() };

        EXPECT_EQ(&vector[0], &vector.X);
        EXPECT_EQ(&vector[1], &vector.Y);
    }

    /* Vector2<T>::operator+() is a no-op. */

    // Vector2<T>::operator-()
    TYPED_TEST(Vector2Test, Negate)
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

    // Vector2<T>::operator+=(const Vector2&)
    // Vector2<T>::operator-=(const Vector2&)
    // Vector2<T>::operator*=(const Vector2&)
    // Vector2<T>::operator/=(const Vector2&)
    // Vector2<T>::operator*=(const T&)
    // Vector2<T>::operator/=(const T&)
    TYPED_TEST(Vector2Test, AssignmentOperators)
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

    // Vector2<T>::operator+(const Vector2&)
    // Vector2<T>::operator-(const Vector2&)
    // Vector2<T>::operator*(const Vector2&)
    // Vector2<T>::operator/(const Vector2&)
    // Vector2<T>::operator*(const T&)
    // Vector2<T>::operator/(const T&)
    // operator*(const T&, const Vector2<T>&)
    // operator/(const T&, const Vector2<T>&)
    TYPED_TEST(Vector2Test, ArithmeticOperators)
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

    // operator==(const Vector2<T>&, const Vector2<U>&)
    TYPED_TEST(Vector2Test, EqualityOperators)
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
}
#endif
