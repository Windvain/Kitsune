#pragma once

#include "Foundation/Maths/Vector.h"

#include "Foundation/Templates/Move.h"
#include "Foundation/Diagnostics/OutOfRangeException.h"

namespace Kitsune
{
    // A 3-dimensional mathematical vector class.
    template<typename T>
    class Vector<T, 3>
    {
    public:
        inline Vector()
            : X(T()), Y(T()), Z(T())
        {
        }

        inline Vector(const T& scalar)
            : X(scalar), Y(scalar), Z(scalar)
        {
        }

        inline Vector(const T& paramX, const T& paramY, const T& paramZ)
            : X(paramX), Y(paramY), Z(paramZ)
        {
        }

        template<typename U>
        inline Vector(const Vector<U, 3>& vector)
            : X(vector.X), Y(vector.Y), Z(vector.Z)
        {
        }

        template<typename U>
        inline Vector(Vector<U, 3>&& vector)
            : X(Move(vector.X)), Y(Move(vector.Y)), Z(Move(vector.Z))
        {
        }

        inline Vector(const Vector&) = default;
        inline Vector(Vector&&) = default;

        inline ~Vector() = default;

    public:
        inline Vector& operator=(const Vector&) = default;
        inline Vector& operator=(Vector&&) = default;

        template<typename U>
        inline Vector& operator=(const Vector<U, 3>& vector)
        {
            X = vector.X;
            Y = vector.Y;
            Z = vector.Z;

            return *this;
        }

        template<typename U>
        inline Vector& operator=(Vector<U, 3>&& vector)
        {
            X = Move(vector.X);
            Y = Move(vector.Y);
            Z = Move(vector.Z);

            return *this;
        }

    public:
        inline T& operator[](Index index)
        {
            switch (index)
            {
            case 0: return X;
            case 1: return Y;
            case 2: return Z;
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
            case 2: return Z;
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
            return Vector(-X, -Y, -Z);
        }

    public:
        inline Vector& operator+=(const Vector& vector)
        {
            X += vector.X;
            Y += vector.Y;
            Z += vector.Z;

            return *this;
        }

        inline Vector& operator-=(const Vector& vector)
        {
            X -= vector.X;
            Y -= vector.Y;
            Z -= vector.Z;

            return *this;
        }

        inline Vector& operator*=(const Vector& vector)
        {
            X *= vector.X;
            Y *= vector.Y;
            Z *= vector.Z;

            return *this;
        }

        inline Vector& operator/=(const Vector& vector)
        {
            X /= vector.X;
            Y /= vector.Y;
            Z /= vector.Z;

            return *this;
        }

        inline Vector& operator*=(const T& scalar)
        {
            X *= scalar;
            Y *= scalar;
            Z *= scalar;

            return *this;
        }

        inline Vector& operator/=(const T& scalar)
        {
            X /= scalar;
            Y /= scalar;
            Z /= scalar;

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
        union { T X, R; };
        union { T Y, G; };
        union { T Z, B; };
    };

    template<typename T>
    inline Vector<T, 3> operator*(const T& scalar, const Vector<T, 3>& vector)
    {
        return (vector * scalar);
    }

    template<typename T>
    inline Vector<T, 3> operator/(const T& scalar, const Vector<T, 3>& vector)
    {
        return (Vector<T, 3>(scalar) /= vector);
    }

    template<typename T, typename U>
    inline bool operator==(const Vector<T, 3>& vector1, const Vector<U, 3>& vector2)
    {
        return ((vector1.X == vector2.X) && (vector1.Y == vector2.Y) &&
                (vector1.Z == vector2.Z));
    }

    template<typename T> using Vector3 = Vector<T, 3>;
    template<typename T> using Point3 = Vector<T, 3>;
    template<typename T> using Color3 = Vector<T, 3>;
}
