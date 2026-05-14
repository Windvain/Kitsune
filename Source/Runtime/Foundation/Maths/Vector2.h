#pragma once

#include "Foundation/Maths/Vector.h"

#include "Foundation/Templates/Move.h"
#include "Foundation/Diagnostics/OutOfRangeException.h"

namespace Kitsune
{
    // A 2-dimensional mathematical vector class.
    template<typename T>
    class Vector<T, 2>
    {
    public:
        inline Vector()
            : X(T()), Y(T())
        {
        }

        inline Vector(const T& scalar)
            : X(scalar), Y(scalar)
        {
        }

        inline Vector(const T& paramX, const T& paramY)
            : X(paramX), Y(paramY)
        {
        }

        template<typename U>
        inline Vector(const Vector<U, 2>& vector)
            : X(vector.X), Y(vector.Y)
        {
        }

        template<typename U>
        inline Vector(Vector<U, 2>&& vector)
            : X(Move(vector.X)), Y(Move(vector.Y))
        {
        }

        inline Vector(const Vector&) = default;
        inline Vector(Vector&&) = default;

        inline ~Vector() = default;

    public:
        inline Vector& operator=(const Vector&) = default;
        inline Vector& operator=(Vector&&) = default;

        template<typename U>
        inline Vector& operator=(const Vector<U, 2>& vector)
        {
            X = vector.X;
            Y = vector.Y;

            return *this;
        }

        template<typename U>
        inline Vector& operator=(Vector<U, 2>&& vector)
        {
            X = Move(vector.X);
            Y = Move(vector.Y);

            return *this;
        }

    public:
        inline T& operator[](Index index)
        {
            switch (index)
            {
            case 0: return X;
            case 1: return Y;
            default:
                throw OutOfRangeException();
            }
        }
        inline const T& operator[](Index index) const
        {
            switch (index)
            {
            case 0: return X;
            case 1: return Y;
            default:
                throw OutOfRangeException();
            }
        }

        inline Vector operator+() const
        {
            return *this;
        }

        inline Vector operator-() const
        {
            return Vector(-X, -Y);
        }

    public:
        inline Vector& operator+=(const Vector& vector)
        {
            X += vector.X;
            Y += vector.Y;

            return *this;
        }

        inline Vector& operator-=(const Vector& vector)
        {
            X -= vector.X;
            Y -= vector.Y;

            return *this;
        }

        inline Vector& operator*=(const Vector& vector)
        {
            X *= vector.X;
            Y *= vector.Y;

            return *this;
        }

        inline Vector& operator/=(const Vector& vector)
        {
            X /= vector.X;
            Y /= vector.Y;

            return *this;
        }

        inline Vector& operator*=(const T& scalar)
        {
            X *= scalar;
            Y *= scalar;

            return *this;
        }

        inline Vector& operator/=(const T& scalar)
        {
            X /= scalar;
            Y /= scalar;

            return *this;
        }

        inline Vector operator+(const Vector& vector) const
        {
            Vector copy = *this;
            return (copy += vector);
        }

        inline Vector operator-(const Vector& vector) const
        {
            Vector copy = *this;
            return (copy -= vector);
        }

        inline Vector operator*(const Vector& vector) const
        {
            Vector copy = *this;
            return (copy *= vector);
        }

        inline Vector operator/(const Vector& vector) const
        {
            Vector copy = *this;
            return (copy /= vector);
        }

        inline Vector operator*(const T& scalar) const
        {
            Vector copy = *this;
            return (copy *= scalar);
        }

        inline Vector operator/(const T& scalar) const
        {
            Vector copy = *this;
            return (copy /= scalar);
        }

    public:
        union { T X, U; };
        union { T Y, V; };
    };

    template<typename T>
    inline Vector<T, 2> operator*(const T& scalar, const Vector<T, 2>& vector)
    {
        return (vector * scalar);
    }

    template<typename T>
    inline Vector<T, 2> operator/(const T& scalar, const Vector<T, 2>& vector)
    {
        return (Vector<T, 2>(scalar) /= vector);
    }

    template<typename T, typename U>
    inline bool operator==(const Vector<T, 2>& vector1, const Vector<U, 2>& vector2)
    {
        return ((vector1.X == vector2.X) && (vector1.Y == vector2.Y));
    }

    template<typename T> using Vector2 = Vector<T, 2>;
    template<typename T> using Point2 = Vector<T, 2>;
}
