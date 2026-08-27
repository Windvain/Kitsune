#include <gtest/gtest.h>
#include "Foundation/Threading/Atomic.h"

namespace
{
    using namespace Kitsune;

    class MyObject
    {
    public:
        inline MyObject() = default;
        inline MyObject(int id)
            : m_Id(id)
        {
        }

    public:
        inline bool operator==(const MyObject& object) const
        {
            return (m_Id == object.m_Id);
        }

    private:
        int m_Id = 0;
    };

    // Will not test atomicity; just testing the basic functionality.
    template<typename T>
    class AtomicTest : public ::testing::Test
    {
    protected:
        using ValueType = T;
        using AtomicType = Atomic<T>;
    };

    using AtomicTestTypes = ::testing::Types<
        bool, char, char8_t, char16_t, char32_t,
        signed char, signed short, signed int, signed long, signed long long,
        unsigned char, unsigned short, unsigned int, unsigned long, unsigned long long,
        MyObject>;

    TYPED_TEST_SUITE(AtomicTest, AtomicTestTypes);

    TYPED_TEST(AtomicTest, DefaultConstructor)
    {
        using T = typename TestFixture::ValueType;
        Atomic<T> object;

        EXPECT_EQ(object.Load(), T());
    }

    TYPED_TEST(AtomicTest, ValueConstructor)
    {
        using T = typename TestFixture::ValueType;

        if constexpr (std::is_integral_v<T> || std::is_same_v<T, MyObject>)
        {
            Atomic<T> object(T(34));
            EXPECT_EQ(object.Load(), T(34));
        }
    }

    TYPED_TEST(AtomicTest, ValueAssign)
    {
        using T = typename TestFixture::ValueType;

        if constexpr (std::is_integral_v<T> || std::is_same_v<T, MyObject>)
        {
            Atomic<T> object(T(34));
            object = T(8);

            EXPECT_EQ(object.Load(), T(8));
        }
    }

    /* Atomic<T>::Load() and Atomic<T>::Store() are assumed to work. */

    TYPED_TEST(AtomicTest, Exchange)
    {
        using T = typename TestFixture::ValueType;

        if constexpr (std::is_integral_v<T> || std::is_same_v<T, MyObject>)
        {
            Atomic<T> object(T(434));
            ASSERT_EQ(object.Load(), T(434));

            EXPECT_EQ(object.Exchange(T(111)), T(434));
            EXPECT_EQ(object.Load(), T(111));
        }
    }

    TYPED_TEST(AtomicTest, CompareExchange)
    {
        using T = typename TestFixture::ValueType;

        if constexpr (std::is_same_v<T, bool>)
        {
            Atomic<T> object(T(true));
            T expected(false);

            EXPECT_FALSE(object.CompareExchange(expected, false));
            EXPECT_EQ(expected, true);

            EXPECT_TRUE(object.CompareExchange(expected, false));
            EXPECT_EQ(expected, true);
            EXPECT_EQ(object.Load(), false);
        }
        else if (std::is_integral_v<T> || std::is_same_v<T, MyObject>)
        {
            Atomic<T> object(T(211));
            T expected(120);

            EXPECT_FALSE(object.CompareExchange(expected, T(5)));
            EXPECT_EQ(expected, T(211));

            EXPECT_TRUE(object.CompareExchange(expected, T(10)));
            EXPECT_EQ(expected, T(211));
            EXPECT_EQ(object.Load(), T(10));
        }
    }

    TYPED_TEST(AtomicTest, ArithmeticOperators)
    {
        using T = typename TestFixture::ValueType;
        if constexpr (std::is_integral_v<T> && !std::is_same_v<T, bool>)
        {
            Atomic<T> object(T(123));
            EXPECT_EQ((object += 3), 126);
            EXPECT_EQ(object.Load(), 126);

            EXPECT_EQ((object -= 10), 116);
            EXPECT_EQ(object.Load(), 116);
        }
    }

    TYPED_TEST(AtomicTest, BinaryOperators)
    {
        using T = typename TestFixture::ValueType;
        if constexpr (std::is_integral_v<T> && !std::is_same_v<T, bool>)
        {
            Atomic<T> object(T(123));
            T expected = T(123) & 3;
            EXPECT_EQ((object &= 3), expected);
            EXPECT_EQ(object.Load(), expected);

            T expected2 = expected | 10;
            EXPECT_EQ((object |= 10), expected2);
            EXPECT_EQ(object.Load(), expected2);

            T expected3 = expected2 ^ 31;
            EXPECT_EQ((object ^= 31), expected3);
            EXPECT_EQ(object.Load(), expected3);
        }
    }

    /* Atomic<T>::operator T() is an alias for Load(). Skipping. */

    TYPED_TEST(AtomicTest, IncrementDecrementOperators)
    {
        using T = typename TestFixture::ValueType;
        if constexpr (std::is_integral_v<T> && !std::is_same_v<T, bool>)
        {
            Atomic<T> object(T(123));
            EXPECT_EQ(++object, T(124));
            EXPECT_EQ(object.Load(), T(124));

            EXPECT_EQ(object++, T(124));
            EXPECT_EQ(object.Load(), T(125));

            EXPECT_EQ(--object, T(124));
            EXPECT_EQ(object.Load(), T(124));

            EXPECT_EQ(object--, T(124));
            EXPECT_EQ(object.Load(), T(123));
        }
    }

    TYPED_TEST(AtomicTest, ArithmeticFetchFunctions)
    {
        using T = typename TestFixture::ValueType;
        if constexpr (std::is_integral_v<T> && !std::is_same_v<T, bool>)
        {
            Atomic<T> object(T(23));
            EXPECT_EQ(object.FetchAdd(4), 23);
            EXPECT_EQ(object.Load(), 27);

            EXPECT_EQ(object.FetchSub(7), 27);
            EXPECT_EQ(object.Load(), 20);
        }
    }

    TYPED_TEST(AtomicTest, BinaryFetchFunctions)
    {
        using T = typename TestFixture::ValueType;
        if constexpr (std::is_integral_v<T> && !std::is_same_v<T, bool>)
        {
            Atomic<T> object(T(23));
            T expected = T(23) & 4;

            EXPECT_EQ(object.FetchAnd(4), 23);
            EXPECT_EQ(object.Load(), expected);

            T expected2 = expected ^ 3;
            EXPECT_EQ(object.FetchXor(3), expected);
            EXPECT_EQ(object.Load(), expected2);

            T expected3 = expected2 | 43;
            EXPECT_EQ(object.FetchOr(43), expected2);
            EXPECT_EQ(object.Load(), expected3);
        }
    }
}
