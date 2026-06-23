#pragma once

#include <concepts>

#include "Foundation/Maths/Vector.h"
#include "Foundation/Common/Macros.h"

#include "Foundation/Templates/Move.h"
#include "Foundation/Diagnostics/OutOfRangeException.h"

// If anonymous structs are not defined, don't use SIMD optimizations.
#if !defined(KITSUNE_COMPILER_SUPPORTS_ANONYMOUS_STRUCTS)
    #undef KITSUNE_ENABLE_SIMD_OPTIMIZATIONS
#endif

#if defined(KITSUNE_ARCH_X86_64) && defined(KITSUNE_ENABLE_SIMD_OPTIMIZATIONS)
    // We expect x86_64 targets to support AVX2, b.c. as of April 2026,
    // 95.29% of all computers support AVX2.
    // https://store.steampowered.com/hwsurvey/Steam-Hardware-Software-Survey-Welcome-to-Steam
    #include <immintrin.h>
#endif

KITSUNE_PUSH_COMPILER_WARNINGS()
KITSUNE_IGNORE_MSVC_WARNING(4201)   // Nonstandard extension used: Nameless struct/union.

KITSUNE_IGNORE_CLANG_WARNING(-Wgnu-anonymous-struct)
KITSUNE_IGNORE_CLANG_WARNING(-Wnested-anon-types)

namespace Kitsune
{
    namespace Details
    {
        template<
            typename T,
            typename Vec,
            bool IsSseOptimized = std::is_trivial_v<T> && (sizeof(T) == 4),
            bool IsAvxOptimized = std::is_trivial_v<T> && (sizeof(T) == 8)>
        struct Vector4Base
        {
            using StorageType = T[4];

            inline static Vec Negate(const Vec& vector)
            {
                return Vec(-vector.X, -vector.Y, -vector.Z, -vector.W);
            }

            inline static bool CompareEqual(const Vec& vector1, const Vec& vector2)
            {
                return ((vector1.X == vector2.X) && (vector1.Y == vector2.Y) &&
                        (vector1.Z == vector2.Z) && (vector1.W == vector2.W));
            }

            inline static Vec Add(const Vec& vector1, const Vec& vector2)
            {
                return Vec(
                    vector1.X + vector2.X,
                    vector1.Y + vector2.Y,
                    vector1.Z + vector2.Z,
                    vector1.W + vector2.W);
            }

            inline static Vec Subtract(const Vec& vector1, const Vec& vector2)
            {
                return Vec(
                    vector1.X - vector2.X,
                    vector1.Y - vector2.Y,
                    vector1.Z - vector2.Z,
                    vector1.W - vector2.W);
            }

            inline static Vec Multiply(const Vec& vector1, const Vec& vector2)
            {
                return Vec(
                    vector1.X * vector2.X,
                    vector1.Y * vector2.Y,
                    vector1.Z * vector2.Z,
                    vector1.W * vector2.W);
            }

            inline static Vec Divide(const Vec& vector1, const Vec& vector2)
            {
                return Vec(
                    vector1.X / vector2.X,
                    vector1.Y / vector2.Y,
                    vector1.Z / vector2.Z,
                    vector1.W / vector2.W);
            }

            inline static Vec Multiply(const Vec& vector1, const T& scalar)
            {
                return Vec(
                    vector1.X * scalar,
                    vector1.Y * scalar,
                    vector1.Z * scalar,
                    vector1.W * scalar);
            }

            inline static Vec Divide(const Vec& vector1, const T& scalar)
            {
                return Vec(
                    vector1.X / scalar,
                    vector1.Y / scalar,
                    vector1.Z / scalar,
                    vector1.W / scalar);
            }
        };

#if defined(KITSUNE_ARCH_X86_64) && defined(KITSUNE_ENABLE_SIMD_OPTIMIZATIONS)
        template<typename Vec>
        struct Vector4Base<float, Vec, true, false>
        {
            using StorageType = __m128;

            inline static Vec Negate(const Vec& vector)
            {
                Vec negated;
                negated.m_Storage = _mm_sub_ps(
                    _mm_setzero_ps(),
                    vector.m_Storage);

                return negated;
            }

            inline static bool CompareEqual(const Vec& vector1, const Vec& vector2)
            {
                return (_mm_movemask_ps(
                    _mm_cmpeq_ps(vector1.m_Storage, vector2.m_Storage)) == 0xF);
            }

            inline static Vec Add(const Vec& vector1, const Vec& vector2)
            {
                Vec sum;
                sum.m_Storage = _mm_add_ps(vector1.m_Storage, vector2.m_Storage);

                return sum;
            }

            inline static Vec Subtract(const Vec& vector1, const Vec& vector2)
            {
                Vec difference;
                difference.m_Storage = _mm_sub_ps(
                    vector1.m_Storage,
                    vector2.m_Storage);

                return difference;
            }

            inline static Vec Multiply(const Vec& vector1, const Vec& vector2)
            {
                Vec product;
                product.m_Storage = _mm_mul_ps(
                    vector1.m_Storage,
                    vector2.m_Storage);

                return product;
            }

            inline static Vec Divide(const Vec& vector1, const Vec& vector2)
            {
                Vec quotient;
                quotient.m_Storage = _mm_div_ps(
                    vector1.m_Storage,
                    vector2.m_Storage);

                return quotient;
            }

            inline static Vec Multiply(const Vec& vector, float scalar)
            {
                Vec product;
                product.m_Storage = _mm_mul_ps(
                    vector.m_Storage,
                    _mm_set1_ps(scalar));

                return product;
            }

            inline static Vec Divide(const Vec& vector, float scalar)
            {
                return Multiply(vector, 1 / scalar);
            }
        };

        template<std::integral T, typename Vec>
        struct Vector4Base<T, Vec, true, false>
        {
            using StorageType = __m128i;

            inline static Vec Negate(const Vec& vector)
            {
                Vec negated;
                negated.m_Storage = _mm_sub_epi32(
                    _mm_setzero_si128(),
                    vector.m_Storage);

                return negated;
            }

            inline static bool CompareEqual(const Vec& vector1, const Vec& vector2)
            {
                return (_mm_movemask_epi8(
                    _mm_cmpeq_epi32(vector1.m_Storage, vector2.m_Storage)) == 0xFFFF);
            }

            inline static Vec Add(const Vec& vector1, const Vec& vector2)
            {
                Vec sum;
                sum.m_Storage = _mm_add_epi32(vector1.m_Storage, vector2.m_Storage);

                return sum;
            }

            inline static Vec Subtract(const Vec& vector1, const Vec& vector2)
            {
                Vec difference;
                difference.m_Storage = _mm_sub_epi32(
                    vector1.m_Storage,
                    vector2.m_Storage);

                return difference;
            }

            inline static Vec Multiply(const Vec& vector1, const Vec& vector2)
            {
                Vec product;
                product.m_Storage = _mm_mullo_epi32(
                    vector1.m_Storage,
                    vector2.m_Storage);

                return product;
            }

            inline static Vec Divide(const Vec& vector1, const Vec& vector2)
            {
                // _mm_div_ep[i,u]32 isn't defined in <immintrin.h>. Casting it
                // to a float then dividing via _mm_div_ps() returns
                // inaccurate results.
                // Just use the scalar path instead.
                return Vector4Base<T, Vec, false, false>::Divide(vector1, vector2);
            }

            inline static Vec Multiply(const Vec& vector, T scalar)
            {
                Vec product;
                product.m_Storage = _mm_mullo_epi32(
                    vector.m_Storage,
                    _mm_set1_epi32(scalar));

                return product;
            }

            inline static Vec Divide(const Vec& vector, T scalar)
            {
                // Look at Divide(const Vec&, const Vec&).
                return Vector4Base<T, Vec, false, false>::Divide(vector, scalar);
            }
        };

        template<typename Vec>
        struct Vector4Base<double, Vec, false, true>
        {
            using StorageType = __m256d;

            inline static Vec Negate(const Vec& vector)
            {
                Vec negated;
                negated.m_Storage = _mm256_sub_pd(
                    _mm256_setzero_pd(),
                    vector.m_Storage);

                return negated;
            }

            inline static bool CompareEqual(const Vec& vector1, const Vec& vector2)
            {
                return (_mm256_movemask_pd(
                    _mm256_cmp_pd(vector1.m_Storage, vector2.m_Storage, 0)) == 0xF);
            }

            inline static Vec Add(const Vec& vector1, const Vec& vector2)
            {
                Vec sum;
                sum.m_Storage = _mm256_add_pd(vector1.m_Storage, vector2.m_Storage);

                return sum;
            }

            inline static Vec Subtract(const Vec& vector1, const Vec& vector2)
            {
                Vec difference;
                difference.m_Storage = _mm256_sub_pd(
                    vector1.m_Storage,
                    vector2.m_Storage);

                return difference;
            }

            inline static Vec Multiply(const Vec& vector1, const Vec& vector2)
            {
                Vec product;
                product.m_Storage = _mm256_mul_pd(
                    vector1.m_Storage,
                    vector2.m_Storage);

                return product;
            }

            inline static Vec Divide(const Vec& vector1, const Vec& vector2)
            {
                Vec quotient;
                quotient.m_Storage = _mm256_div_pd(
                    vector1.m_Storage,
                    vector2.m_Storage);

                return quotient;
            }

            inline static Vec Multiply(const Vec& vector, double scalar)
            {
                Vec product;
                product.m_Storage = _mm256_mul_pd(
                    vector.m_Storage,
                    _mm256_set1_pd(scalar));

                return product;
            }

            inline static Vec Divide(const Vec& vector, double scalar)
            {
                return Multiply(vector, 1 / scalar);
            }
        };

        template<std::integral T, typename Vec>
        struct Vector4Base<T, Vec, false, true>
        {
            using StorageType = __m256i;

            inline static Vec Negate(const Vec& vector)
            {
                Vec negated;
                negated.m_Storage = _mm256_sub_epi64(
                    _mm256_setzero_si256(),
                    vector.m_Storage);

                return negated;
            }

            inline static bool CompareEqual(const Vec& vector1, const Vec& vector2)
            {
                return (_mm256_movemask_epi8(
                    _mm256_cmpeq_epi64(
                        vector1.m_Storage,
                        vector2.m_Storage)) == 0xFFFFFFFF);
            }

            inline static Vec Add(const Vec& vector1, const Vec& vector2)
            {
                Vec sum;
                sum.m_Storage = _mm256_add_epi64(vector1.m_Storage, vector2.m_Storage);

                return sum;
            }

            inline static Vec Subtract(const Vec& vector1, const Vec& vector2)
            {
                Vec difference;
                difference.m_Storage = _mm256_sub_epi64(
                    vector1.m_Storage,
                    vector2.m_Storage);

                return difference;
            }

            inline static Vec Multiply(const Vec& vector1, const Vec& vector2)
            {
                // _mm256_mullo_epi64 was only added with AVX512VL.
                return Vector4Base<T, Vec, false, false>::Multiply(vector1, vector2);
            }

            inline static Vec Divide(const Vec& vector1, const Vec& vector2)
            {
                return Vector4Base<T, Vec, false, false>::Divide(vector1, vector2);
            }

            inline static Vec Multiply(const Vec& vector, T scalar)
            {
                // _mm256_mullo_epi64 was only added with AVX512VL.
                return Vector4Base<T, Vec, false, false>::Multiply(vector, Vec(scalar));
            }

            inline static Vec Divide(const Vec& vector, T scalar)
            {
                return Vector4Base<T, Vec, false, false>::Divide(vector, scalar);
            }
        };
#endif
    }

    // A 4-dimensional mathematical vector class.
    template<typename T>
    class Vector<T, 4>
    {
    private:
        using BaseType_ = Details::Vector4Base<T, Vector<T, 4>>;

    public:
        inline Vector()
            : X(T()), Y(T()), Z(T()), W(T())
        {
        }

        inline Vector(const T& scalar)
            : X(scalar), Y(scalar), Z(scalar), W(scalar)
        {
        }

        inline Vector(const T& paramX, const T& paramY, const T& paramZ,
                      const T& paramW)
            : X(paramX), Y(paramY), Z(paramZ), W(paramW)
        {
        }

        template<typename U>
        inline Vector(const Vector<U, 4>& vector)
            : X(vector.X), Y(vector.Y), Z(vector.Z), W(vector.W)
        {
        }

        template<typename U>
        inline Vector(Vector<U, 4>&& vector)
            : X(Move(vector.X)), Y(Move(vector.Y)), Z(Move(vector.Z)),
              W(Move(vector.W))
        {
        }

        inline Vector(const Vector&) = default;
        inline Vector(Vector&&) = default;

        inline ~Vector() = default;

    public:
        inline Vector& operator=(const Vector&) = default;
        inline Vector& operator=(Vector&&) = default;

        template<typename U>
        inline Vector& operator=(const Vector<U, 4>& vector)
        {
            X = vector.X;
            Y = vector.Y;
            Z = vector.Z;
            W = vector.W;

            return *this;
        }

        template<typename U>
        inline Vector& operator=(Vector<U, 4>&& vector)
        {
            X = Move(vector.X);
            Y = Move(vector.Y);
            Z = Move(vector.Z);
            W = Move(vector.W);

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
            case 3: return W;
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
            case 3: return W;
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
            return BaseType_::Negate(*this);
        }

    public:
        inline Vector& operator+=(const Vector& vector)
        {
            return (*this = *this + vector);
        }

        inline Vector& operator-=(const Vector& vector)
        {
            return (*this = *this - vector);
        }

        inline Vector& operator*=(const Vector& vector)
        {
            return (*this = *this * vector);
        }

        inline Vector& operator/=(const Vector& vector)
        {
            return (*this = *this / vector);
        }

        inline Vector& operator*=(const T& scalar)
        {
            return (*this = *this * scalar);
        }

        inline Vector& operator/=(const T& scalar)
        {
            return (*this = *this / scalar);
        }

    public:
        inline Vector operator+(const Vector& vector) const
        {
            return BaseType_::Add(*this, vector);
        }

        inline Vector operator-(const Vector& vector) const
        {
            return BaseType_::Subtract(*this, vector);
        }

        inline Vector operator*(const Vector& vector) const
        {
            return BaseType_::Multiply(*this, vector);
        }

        inline Vector operator/(const Vector& vector) const
        {
            return BaseType_::Divide(*this, vector);
        }

        inline Vector operator*(const T& scalar) const
        {
            return BaseType_::Multiply(*this, scalar);
        }

        inline Vector operator/(const T& scalar) const
        {
            return BaseType_::Divide(*this, scalar);
        }

    public:
        union
        {
            struct { T X, Y, Z, W; };
            struct { T R, G, B, A; };

            typename BaseType_::StorageType m_Storage;
        };
    };

    template<typename T>
    inline Vector<T, 4> operator*(const T& scalar, const Vector<T, 4>& vector)
    {
        return (vector * scalar);
    }

    template<typename T>
    inline Vector<T, 4> operator/(const T& scalar, const Vector<T, 4>& vector)
    {
        return (Vector<T, 4>(scalar) /= vector);
    }

    template<typename T>
    inline bool operator==(const Vector<T, 4>& vector1, const Vector<T, 4>& vector2)
    {
        return Details::Vector4Base<T, Vector<T, 4>>::CompareEqual(
            vector1, vector2);
    }

    namespace Maths
    {
        template<typename T>
        inline T Dot(const Vector<T, 4>& vector1, const Vector<T, 4>& vector2)
        {
            Vector<T, 4> vector = vector1 * vector2;
            return vector.X + vector.Y + vector.Z + vector.W;
        }

#if defined(KITSUNE_ARCH_X86_64) && defined(KITSUNE_ENABLE_SIMD_OPTIMIZATIONS)
        inline float Dot(const Vector<float, 4>& vector1,
                         const Vector<float, 4>& vector2)
        {
            __m128 data = _mm_dp_ps(vector1.m_Storage, vector2.m_Storage, 0xFF);
            return _mm_cvtss_f32(data);
        }
#endif
    }

    template<typename T> using Vector4 = Vector<T, 4>;
    template<typename T> using Point4 = Vector<T, 4>;
    template<typename T> using Color4 = Vector<T, 4>;
}

KITSUNE_POP_COMPILER_WARNINGS()
