#include <random>
#include <chrono>
#include <gtest/gtest.h>

#include "Foundation/Common/Types.h"
#include "Foundation/Maths/Vector3.h"

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
    class Vector3Test : public ::testing::Test
    {
    public:
        using ValueType = T;

    protected:
        inline Vector3Test()
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

    using Vector3TestImpl = ::testing::Types<
        Uint16, Uint32, Uint64,
        Int16, Int32, Int64,
        float, double>;

    TYPED_TEST_SUITE(Vector3Test, Vector3TestImpl);

    // Vector3<T>::Vector3()
    TYPED_TEST(Vector3Test, DefaultConstructor)
    {
        using T = typename TestFixture::ValueType;

        Vector3<T> vector;
        EXPECT_TEMPLATED_EQ(vector.X, T(0));
        EXPECT_TEMPLATED_EQ(vector.Y, T(0));
        EXPECT_TEMPLATED_EQ(vector.Z, T(0));
    }

    // Vector3<T>::Vector3(const T&)
    TYPED_TEST(Vector3Test, ScalarConstructor)
    {
        using T = typename TestFixture::ValueType;

        T scalar(this->GetRandomValue());
        Vector3<T> vector(scalar);

        EXPECT_TEMPLATED_EQ(vector.X, scalar);
        EXPECT_TEMPLATED_EQ(vector.Y, scalar);
        EXPECT_TEMPLATED_EQ(vector.Z, scalar);
    }

    // Vector3<T>::Vector3(const T&, const T&, const T&)
    TYPED_TEST(Vector3Test, XYZConstructor)
    {
        using T = typename TestFixture::ValueType;

        T scalarX(this->GetRandomValue());
        T scalarY(this->GetRandomValue());
        T scalarZ(this->GetRandomValue());
        Vector3<T> vector(scalarX, scalarY, scalarZ);

        EXPECT_TEMPLATED_EQ(vector.X, scalarX);
        EXPECT_TEMPLATED_EQ(vector.Y, scalarY);
        EXPECT_TEMPLATED_EQ(vector.Z, scalarZ);
    }

    // Vector3<T>::Vector3(const Vector3<U>&)
    TYPED_TEST(Vector3Test, TemplatedCopy)
    {
        using T = typename TestFixture::ValueType;

        T scalarX(this->GetRandomValue());
        T scalarY(this->GetRandomValue());
        T scalarZ(this->GetRandomValue());

        Vector3<T> vector(scalarX, scalarY, scalarZ);
        Vector3<T> copy = vector;

        EXPECT_TEMPLATED_EQ(copy.X, scalarX);
        EXPECT_TEMPLATED_EQ(copy.Y, scalarY);
        EXPECT_TEMPLATED_EQ(copy.Z, scalarZ);
    }

    // Vector3<T>::Vector3(Vector3<U>&&)
    TYPED_TEST(Vector3Test, TemplatedMove)
    {
        using T = typename TestFixture::ValueType;

        Vector3<FromCast<T>> vector(FromCast<T>(3), FromCast<T>(54), FromCast<T>(512));
        Vector3<ToCast<T>> copy = std::move(vector);

        EXPECT_TEMPLATED_EQ(copy.X.Number, 3);
        EXPECT_TEMPLATED_EQ(copy.Y.Number, 54);
        EXPECT_TEMPLATED_EQ(copy.Z.Number, 512);

        EXPECT_TEMPLATED_EQ(vector.X.Number, 0);
        EXPECT_TEMPLATED_EQ(vector.Y.Number, 0);
        EXPECT_TEMPLATED_EQ(vector.Z.Number, 0);
    }

    // Vector3<T>::operator=(const Vector3<U>&)
    TYPED_TEST(Vector3Test, TemplatedCopyAssign)
    {
        using T = typename TestFixture::ValueType;

        T scalarX(this->GetRandomValue());
        T scalarY(this->GetRandomValue());
        T scalarZ(this->GetRandomValue());

        Vector3<FromCast<T>> vector = {
            FromCast<T>(scalarX),
            FromCast<T>(scalarY),
            FromCast<T>(scalarZ)
        };

        Vector3<ToCast<T>> copy = {
            ToCast<T>(this->GetRandomValue()),
            ToCast<T>(this->GetRandomValue()),
            ToCast<T>(this->GetRandomValue()),
        };

        copy = vector;

        EXPECT_TEMPLATED_EQ(copy.X.Number, scalarX);
        EXPECT_TEMPLATED_EQ(copy.Y.Number, scalarY);
        EXPECT_TEMPLATED_EQ(copy.Z.Number, scalarZ);
    }

    // Vector3<T>::operator=(Vector3<U>&&)
    TYPED_TEST(Vector3Test, TemplatedMoveAssign)
    {
        using T = typename TestFixture::ValueType;

        T scalarX(this->GetRandomValue());
        T scalarY(this->GetRandomValue());
        T scalarZ(this->GetRandomValue());

        Vector3<FromCast<T>> vector = {
            FromCast<T>(scalarX),
            FromCast<T>(scalarY),
            FromCast<T>(scalarZ)
        };

        Vector3<ToCast<T>> moved = {
            ToCast<T>(this->GetRandomValue()),
            ToCast<T>(this->GetRandomValue()),
            ToCast<T>(this->GetRandomValue()),
        };

        moved = std::move(vector);

        EXPECT_TEMPLATED_EQ(moved.X.Number, scalarX);
        EXPECT_TEMPLATED_EQ(moved.Y.Number, scalarY);
        EXPECT_TEMPLATED_EQ(moved.Z.Number, scalarZ);

        EXPECT_TEMPLATED_EQ(vector.X.Number, 0);
        EXPECT_TEMPLATED_EQ(vector.Y.Number, 0);
        EXPECT_TEMPLATED_EQ(vector.Z.Number, 0);
    }

    // Vector3<T>::operator[](Index)
    TYPED_TEST(Vector3Test, Subscript)
    {
        using T = typename TestFixture::ValueType;
        Vector3<T> vector = {
            this->GetRandomValue(),
            this->GetRandomValue(),
            this->GetRandomValue(),
        };

        EXPECT_EQ(&vector[0], &vector.X);
        EXPECT_EQ(&vector[1], &vector.Y);
        EXPECT_EQ(&vector[2], &vector.Z);
    }

    /* Vector3<T>::operator+() is a no-op. */

    // Vector3<T>::operator-()
    TYPED_TEST(Vector3Test, Negate)
    {
        using T = typename TestFixture::ValueType;
        if constexpr (std::is_signed_v<T>)
        {
            T scalarX(this->GetRandomValue());
            T scalarY(this->GetRandomValue());
            T scalarZ(this->GetRandomValue());

            Vector3<T> vector = { scalarX, scalarY, scalarZ };
            Vector3<T> neg = -vector;

            EXPECT_TEMPLATED_EQ(neg.X, -scalarX);
            EXPECT_TEMPLATED_EQ(neg.Y, -scalarY);
            EXPECT_TEMPLATED_EQ(neg.Z, -scalarZ);
        }
    }

    // Vector3<T>::operator+=(const Vector3&)
    // Vector3<T>::operator-=(const Vector3&)
    // Vector3<T>::operator*=(const Vector3&)
    // Vector3<T>::operator/=(const Vector3&)
    // Vector3<T>::operator*=(const T&)
    // Vector3<T>::operator/=(const T&)
    TYPED_TEST(Vector3Test, AssignmentOperators)
    {
        using T = typename TestFixture::ValueType;

        T scalarX(this->GetRandomValue());
        T scalarY(this->GetRandomValue());
        T scalarZ(this->GetRandomValue());

        Vector3<T> vector = { scalarX, scalarY, scalarZ };
        Vector3<T> vector2 = {
            this->GetRandomValue(),
            this->GetRandomValue(),
            this->GetRandomValue()
        };

        Vector3<T> vector3 = {
            this->GetRandomValue(),
            this->GetRandomValue(),
            this->GetRandomValue(),
        };

        vector /= 4;
        EXPECT_TEMPLATED_EQ(vector.X, scalarX /= 4);
        EXPECT_TEMPLATED_EQ(vector.Y, scalarY /= 4);
        EXPECT_TEMPLATED_EQ(vector.Z, scalarZ /= 4);

        vector *= 7;
        EXPECT_TEMPLATED_EQ(vector.X, scalarX *= 7);
        EXPECT_TEMPLATED_EQ(vector.Y, scalarY *= 7);
        EXPECT_TEMPLATED_EQ(vector.Z, scalarZ *= 7);

        vector -= vector2;
        EXPECT_TEMPLATED_EQ(vector.X, scalarX -= vector2.X);
        EXPECT_TEMPLATED_EQ(vector.Y, scalarY -= vector2.Y);
        EXPECT_TEMPLATED_EQ(vector.Z, scalarZ -= vector2.Z);

        vector += vector2;
        EXPECT_TEMPLATED_EQ(vector.X, scalarX += vector2.X);
        EXPECT_TEMPLATED_EQ(vector.Y, scalarY += vector2.Y);
        EXPECT_TEMPLATED_EQ(vector.Z, scalarZ += vector2.Z);

        vector *= vector3;
        EXPECT_TEMPLATED_EQ(vector.X, scalarX *= vector3.X);
        EXPECT_TEMPLATED_EQ(vector.Y, scalarY *= vector3.Y);
        EXPECT_TEMPLATED_EQ(vector.Z, scalarZ *= vector3.Z);

        vector /= vector3;
        EXPECT_TEMPLATED_EQ(vector.X, scalarX /= vector3.X);
        EXPECT_TEMPLATED_EQ(vector.Y, scalarY /= vector3.Y);
        EXPECT_TEMPLATED_EQ(vector.Z, scalarZ /= vector3.Z);
    }

    // Vector3<T>::operator+(const Vector3&)
    // Vector3<T>::operator-(const Vector3&)
    // Vector3<T>::operator*(const Vector3&)
    // Vector3<T>::operator/(const Vector3&)
    // Vector3<T>::operator*(const T&)
    // Vector3<T>::operator/(const T&)
    // operator*(const T&, const Vector3<T>&)
    // operator/(const T&, const Vector3<T>&)
    TYPED_TEST(Vector3Test, ArithmeticOperators)
    {
        using T = typename TestFixture::ValueType;

        T scalarX = this->GetRandomValue();
        T scalarY = this->GetRandomValue();
        T scalarZ = this->GetRandomValue();

        T scalar = this->GetRandomValue();

        Vector3<T> vector = { scalarX, scalarY, scalarZ };
        Vector3<T> vector2 = {
            this->GetRandomValue(),
            this->GetRandomValue(),
            this->GetRandomValue()
        };

        Vector3<T> scalarDivValue = vector / scalar;
        EXPECT_TEMPLATED_EQ(scalarDivValue.X, vector.X / scalar);
        EXPECT_TEMPLATED_EQ(scalarDivValue.Y, vector.Y / scalar);
        EXPECT_TEMPLATED_EQ(scalarDivValue.Z, vector.Z / scalar);

        Vector3<T> scalarMulValue = vector * scalar;
        EXPECT_TEMPLATED_EQ(scalarMulValue.X, vector.X * scalar);
        EXPECT_TEMPLATED_EQ(scalarMulValue.Y, vector.Y * scalar);
        EXPECT_TEMPLATED_EQ(scalarMulValue.Z, vector.Z * scalar);

        Vector3<T> subValue = vector - vector2;
        EXPECT_TEMPLATED_EQ(subValue.X, vector.X - vector2.X);
        EXPECT_TEMPLATED_EQ(subValue.Y, vector.Y - vector2.Y);
        EXPECT_TEMPLATED_EQ(subValue.Z, vector.Z - vector2.Z);

        Vector3<T> addValue = vector + vector2;
        EXPECT_TEMPLATED_EQ(addValue.X, vector.X + vector2.X);
        EXPECT_TEMPLATED_EQ(addValue.Y, vector.Y + vector2.Y);
        EXPECT_TEMPLATED_EQ(addValue.Z, vector.Z + vector2.Z);

        Vector3<T> mulValue = vector * vector2;
        EXPECT_TEMPLATED_EQ(mulValue.X, vector.X * vector2.X);
        EXPECT_TEMPLATED_EQ(mulValue.Y, vector.Y * vector2.Y);
        EXPECT_TEMPLATED_EQ(mulValue.Z, vector.Z * vector2.Z);

        Vector3<T> divValue = vector / vector2;
        EXPECT_TEMPLATED_EQ(divValue.X, vector.X / vector2.X);
        EXPECT_TEMPLATED_EQ(divValue.Y, vector.Y / vector2.Y);
        EXPECT_TEMPLATED_EQ(divValue.Z, vector.Z / vector2.Z);

        Vector3<T> flippedDivValue = scalar / vector;
        EXPECT_TEMPLATED_EQ(flippedDivValue.X, scalar / vector.X);
        EXPECT_TEMPLATED_EQ(flippedDivValue.Y, scalar / vector.Y);
        EXPECT_TEMPLATED_EQ(flippedDivValue.Z, scalar / vector.Z);

        Vector3<T> flippedMulValue = scalar * vector;
        EXPECT_TEMPLATED_EQ(flippedMulValue.X, vector.X * scalar);
        EXPECT_TEMPLATED_EQ(flippedMulValue.Y, vector.Y * scalar);
        EXPECT_TEMPLATED_EQ(flippedMulValue.Z, vector.Z * scalar);
    }

    // operator==(const Vector3<T>&, const Vector3<U>&)
    TYPED_TEST(Vector3Test, EqualityOperators)
    {
        using T = typename TestFixture::ValueType;

        T scalarX = this->GetRandomValue();
        T scalarY = this->GetRandomValue();
        T scalarZ = this->GetRandomValue();

        Vector3<T> vector = { scalarX, scalarY, scalarZ };
        Vector3<T> equalVector = { scalarX, scalarY, scalarZ };
        Vector3<T> unequalVector = {
            this->GetRandomValue(),
            this->GetRandomValue(),
            this->GetRandomValue(),
        };

        while ((unequalVector.X == scalarX) && (unequalVector.Y == scalarY) &&
               (unequalVector.Z == scalarZ))
        {
            unequalVector.X = this->GetRandomValue();
            unequalVector.Y = this->GetRandomValue();
            unequalVector.Z = this->GetRandomValue();
        }

        EXPECT_TRUE(vector == equalVector);
        EXPECT_FALSE(vector == unequalVector);

        EXPECT_FALSE(vector != equalVector);
        EXPECT_TRUE(vector != unequalVector);
    }
}
