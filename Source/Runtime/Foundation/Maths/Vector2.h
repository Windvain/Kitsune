#pragma once

#include "Foundation/Common/Macros.h"
#include "Foundation/Templates/Move.h"

#include "Foundation/Maths/VectorBase.h"

KITSUNE_PUSH_COMPILER_WARNINGS()
KITSUNE_IGNORE_MSVC_WARNING(4201)       // Nonstandard extension used: Nameless struct/union.

KITSUNE_IGNORE_CLANG_WARNING(-Wgnu-anonymous-struct)
KITSUNE_IGNORE_CLANG_WARNING(-Wnested-anon-types)

namespace Kitsune
{
    // A 2-dimensional mathematical vector class.
    template<typename T>
    class VectorBase<T, 2>
    {
    public:
        inline VectorBase()
            : X(T()), Y(T())
        {
        }

        inline explicit VectorBase(const T& value)
            : X(value), Y(value)
        {
        }

        inline VectorBase(const T& paramX, const T& paramY)
            : X(paramX), Y(paramY)
        {
        }

        template<typename U>
        inline VectorBase(const VectorBase<U, 2>& vector)
            : X(vector.X), Y(vector.Y)
        {
        }

        template<typename U>
        inline VectorBase(VectorBase<U, 2>&& vector)
            : X(Move(vector.X)), Y(Move(vector.Y))
        {
        }

        inline VectorBase(const VectorBase&) = default;
        inline VectorBase(VectorBase&&) = default;

        inline ~VectorBase() = default;

    public:
        inline VectorBase& operator=(const VectorBase&) = default;
        inline VectorBase& operator=(VectorBase&&) = default;

        template<typename U>
        inline VectorBase& operator=(const VectorBase<U, 2>& vector)
        {
            X = vector.X;
            Y = vector.Y;

            return *this;
        }

        template<typename U>
        inline VectorBase& operator=(VectorBase<U, 2>&& vector)
        {
            X = Move(vector.X);
            Y = Move(vector.Y);

            return *this;
        }

    public:
        inline T& operator[](Index index) { return Data[index]; }
        inline const T& operator[](Index index) const { return Data[index]; }

        inline VectorBase operator-()
        {
            return VectorBase(-X, -Y);
        }

        inline VectorBase& operator+=(const VectorBase& vector)
        {
            X += vector.X;
            Y += vector.Y;

            return *this;
        }

        inline VectorBase& operator-=(const VectorBase& vector)
        {
            X -= vector.X;
            Y -= vector.Y;

            return *this;
        }

        inline VectorBase& operator*=(const VectorBase& vector)
        {
            X *= vector.X;
            Y *= vector.Y;

            return *this;
        }

        inline VectorBase& operator/=(const VectorBase& vector)
        {
            X /= vector.X;
            Y /= vector.Y;

            return *this;
        }

        inline VectorBase& operator*=(const T& scalar)
        {
            X *= scalar;
            Y *= scalar;

            return *this;
        }

        inline VectorBase& operator/=(const T& scalar)
        {
            X /= scalar;
            Y /= scalar;

            return *this;
        }

        inline VectorBase operator+(const VectorBase& vector) const
        {
            VectorBase copy = *this;
            return (copy += vector);
        }

        inline VectorBase operator-(const VectorBase& vector) const
        {
            VectorBase copy = *this;
            return (copy -= vector);
        }

        inline VectorBase operator*(const VectorBase& vector) const
        {
            VectorBase copy = *this;
            return (copy *= vector);
        }

        inline VectorBase operator/(const VectorBase& vector) const
        {
            VectorBase copy = *this;
            return (copy /= vector);
        }

        inline VectorBase operator*(const T& scalar) const
        {
            VectorBase copy = *this;
            return (copy *= scalar);
        }

        inline VectorBase operator/(const T& scalar) const
        {
            VectorBase copy = *this;
            return (copy /= scalar);
        }

    public:
        // Should not be called by engine/client code.
        // Made public so that the compiler can generate code for range-based for loops.
        inline T* begin() { return Data; }
        inline const T* begin() const { return Data; }

        inline T* end() { return (Data + 2); }
        inline const T* end() const { return (Data + 2); }

    public:
        union
        {
            struct { T X; T Y; };
            T Data[2];
        };
    };

    template<typename T>
    inline VectorBase<T, 2> operator*(const T& scalar, const VectorBase<T, 2>& vector)
    {
        return (vector * scalar);
    }

    template<typename T>
    inline VectorBase<T, 2> operator/(const T& scalar, const VectorBase<T, 2>& vector)
    {
        return (VectorBase<T, 2>(scalar) /= vector);
    }

    template<typename T, typename U>
    inline bool operator==(const VectorBase<T, 2>& vector1, const VectorBase<U, 2>& vector2)
    {
        return ((vector1.X == vector2.X) && (vector1.Y == vector2.Y));
    }

    template<typename T>
    using Vector2 = VectorBase<T, 2>;
}

KITSUNE_POP_COMPILER_WARNINGS()
