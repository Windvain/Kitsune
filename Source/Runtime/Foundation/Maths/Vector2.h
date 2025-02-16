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
    template<typename T>
    class VectorBase<T, 2>
    {
    public:
        inline VectorBase() : x(T()), y(T()) { /* ... */ }

        inline explicit VectorBase(const T& val)            : x(val),    y(val)    { /* ... */ }
        inline VectorBase(const T& paramX, const T& paramY) : x(paramX), y(paramY) { /* ... */ }

        template<typename U>
        inline VectorBase(const VectorBase<U, 2>& vec) : x(vec.x), y(vec.y) { /* ... */ }

        template<typename U>
        inline VectorBase(VectorBase<U, 2>&& vec) : x(Move(vec.x)), y(Move(vec.y)) { /* ... */ }

        inline VectorBase(const VectorBase&) = default;
        inline VectorBase(VectorBase&&) = default;

        inline ~VectorBase() = default;

    public:
        inline VectorBase& operator=(const VectorBase&) = default;
        inline VectorBase& operator=(VectorBase&&) = default;

        template<typename U>
        inline VectorBase& operator=(const VectorBase<U, 2>& vec)
        {
            x = vec.x;
            y = vec.y;
            return *this;
        }

        template<typename U>
        inline VectorBase& operator=(VectorBase<U, 2>&& vec)
        {
            x = Move(vec.x);
            y = Move(vec.y);
            return *this;
        }

        inline T& operator[](Index index)             { return Data[index]; }
        inline const T& operator[](Index index) const { return Data[index]; }

        inline VectorBase operator-() { return VectorBase(-x, -y); }

        inline VectorBase& operator+=(const VectorBase& vec)  { x += vec.x; y += vec.y; return *this; }
        inline VectorBase& operator-=(const VectorBase& vec)  { x -= vec.x; y -= vec.y; return *this; }
        inline VectorBase& operator*=(const VectorBase& vec)  { x *= vec.x; y *= vec.y; return *this; }
        inline VectorBase& operator/=(const VectorBase& vec)  { x /= vec.x; y /= vec.y; return *this; }

        inline VectorBase& operator*=(const T& scalar)  { x *= scalar; y *= scalar; return *this; }
        inline VectorBase& operator/=(const T& scalar)  { x /= scalar; y /= scalar; return *this; }

        inline VectorBase operator+(const VectorBase& vec) const { auto copy = *this; return (copy += vec); }
        inline VectorBase operator-(const VectorBase& vec) const { auto copy = *this; return (copy -= vec); }
        inline VectorBase operator*(const VectorBase& vec) const { auto copy = *this; return (copy *= vec); }
        inline VectorBase operator/(const VectorBase& vec) const { auto copy = *this; return (copy /= vec); }

        inline VectorBase operator*(const T& scalar) const { auto copy = *this; return (copy *= scalar); }
        inline VectorBase operator/(const T& scalar) const { auto copy = *this; return (copy /= scalar); }

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
            struct { T x; T y; };
            T Data[2];
        };
    };

    template<typename T>
    VectorBase<T, 2> operator*(const T& scalar, const VectorBase<T, 2>& vec)
    {
        return (vec * scalar);
    }

    template<typename T>
    VectorBase<T, 2> operator/(const T& scalar, const VectorBase<T, 2>& vec)
    {
        return (VectorBase<T, 2>(scalar) /= vec);
    }

    template<typename T, typename U>
    bool operator==(const VectorBase<T, 2>& vec1, const VectorBase<U, 2>& vec2)
    {
        return ((vec1.x == vec2.x) && (vec1.y == vec2.y));
    }

    template<typename T>
    using Vector2 = VectorBase<T, 2>;
}

KITSUNE_POP_COMPILER_WARNINGS()
