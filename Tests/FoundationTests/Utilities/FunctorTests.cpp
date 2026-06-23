#include <gtest/gtest.h>
#include "Foundation/Utilities/Functor.h"

namespace
{
    using namespace Kitsune;

    int AddTestFunction(int x, int y)
    {
        return (x + y);
    }

    class TestClass
    {
    public:
        static int AddStatic(int x, int y)
        {
            return x + y - 2;
        }
    };

    TEST(FunctorTests, DefaultAndNullptrConstructor)
    {
        Functor<int(int, int)> functor1;
        Functor<void(void)> functor2(nullptr);

        EXPECT_EQ(functor1.Get(), nullptr);
        EXPECT_EQ(functor2.Get(), nullptr);
    }

    TEST(FunctorTests, CallableConstructor)
    {
        Functor<int(int, int)> functor1 = AddTestFunction;
        Functor<int(int, int)> functor2 = [&](int x, int y) -> int { return x - y; };
        Functor<int(int, int)> functor3 = &TestClass::AddStatic;

        EXPECT_NE(functor1.Get(), nullptr);
        EXPECT_EQ(functor1(2, 2), 4);

        EXPECT_NE(functor2.Get(), nullptr);
        EXPECT_EQ(functor2(1, 2), -1);

        EXPECT_NE(functor3.Get(), nullptr);
        EXPECT_EQ(functor3(2, 3), 3);
    }

    TEST(FunctorTests, CopyConstructor)
    {
        Functor<int(int, int)> functor1 = AddTestFunction;
        Functor<int(int, int)> functor2 = [&](int x, int y) -> int { return x - y; };
        Functor<int(int, int)> functor3 = &TestClass::AddStatic;

        Functor<int(int, int)> copy1 = functor1;
        Functor<int(int, int)> copy2 = functor2;
        Functor<int(int, int)> copy3 = functor3;

        EXPECT_NE(functor1.Get(), nullptr);
        EXPECT_EQ(functor1(2, 2), 4);
        EXPECT_NE(copy1.Get(), nullptr);
        EXPECT_EQ(copy1(2, 2), 4);

        EXPECT_NE(functor2.Get(), nullptr);
        EXPECT_EQ(functor2(1, 2), -1);
        EXPECT_NE(copy2.Get(), nullptr);
        EXPECT_EQ(copy2(1, 2), -1);

        EXPECT_NE(functor3.Get(), nullptr);
        EXPECT_EQ(functor3(2, 3), 3);
        EXPECT_NE(copy3.Get(), nullptr);
        EXPECT_EQ(copy3(2, 3), 3);
    }

    TEST(FunctorTests, MoveConstructor)
    {
        Functor<int(int, int)> functor1 = AddTestFunction;
        Functor<int(int, int)> functor2 = [=](int x, int y) -> int { return x - y; };
        Functor<int(int, int)> functor3 = &TestClass::AddStatic;

        Functor<int(int, int)> move1 = std::move(functor1);
        Functor<int(int, int)> move2 = std::move(functor2);
        Functor<int(int, int)> move3 = std::move(functor3);

        EXPECT_EQ(functor1.Get(), nullptr);
        EXPECT_NE(move1.Get(), nullptr);
        EXPECT_EQ(move1(2, 2), 4);

        EXPECT_EQ(functor2.Get(), nullptr);
        EXPECT_NE(move2.Get(), nullptr);
        EXPECT_EQ(move2(1, 2), -1);

        EXPECT_EQ(functor3.Get(), nullptr);
        EXPECT_NE(move3.Get(), nullptr);
        EXPECT_EQ(move3(2, 3), 3);
    }

    TEST(FunctorTests, CopyAssign)
    {
        Functor<int(int, int)> functor1 = AddTestFunction;
        Functor<int(int, int)> functor2 = [=](int x, int y) -> int { return x - y; };
        Functor<int(int, int)> functor3 = &TestClass::AddStatic;

        Functor<int(int, int)> copy1 = [functor1](int x, int y) -> int { return x - y; };
        Functor<int(int, int)> copy2 = AddTestFunction;
        Functor<int(int, int)> copy3 = &TestClass::AddStatic;

        copy1 = functor1;
        copy2 = functor2;
        copy3 = functor3;

        EXPECT_NE(functor1.Get(), nullptr);
        EXPECT_EQ(functor1(2, 2), 4);
        EXPECT_NE(copy1.Get(), nullptr);
        EXPECT_EQ(copy1(2, 2), 4);

        EXPECT_NE(functor2.Get(), nullptr);
        EXPECT_EQ(functor2(1, 2), -1);
        EXPECT_NE(copy2.Get(), nullptr);
        EXPECT_EQ(copy2(1, 2), -1);

        EXPECT_NE(functor3.Get(), nullptr);
        EXPECT_EQ(functor3(2, 3), 3);
        EXPECT_NE(copy3.Get(), nullptr);
        EXPECT_EQ(copy3(2, 3), 3);
    }

    TEST(FunctorTests, MoveAssign)
    {
        Functor<int(int, int)> functor1 = AddTestFunction;
        Functor<int(int, int)> functor2 = [=](int x, int y) -> int { return x - y; };
        Functor<int(int, int)> functor3 = &TestClass::AddStatic;

        Functor<int(int, int)> move1 = [=](int x, int y) -> int { return x - y; };
        Functor<int(int, int)> move2 = AddTestFunction;
        Functor<int(int, int)> move3 = &TestClass::AddStatic;

        move1 = std::move(functor1);
        move2 = std::move(functor2);
        move3 = std::move(functor3);

        EXPECT_EQ(functor1.Get(), nullptr);
        EXPECT_NE(move1.Get(), nullptr);
        EXPECT_EQ(move1(2, 2), 4);

        EXPECT_EQ(functor2.Get(), nullptr);
        EXPECT_NE(move2.Get(), nullptr);
        EXPECT_EQ(move2(1, 2), -1);

        EXPECT_EQ(functor3.Get(), nullptr);
        EXPECT_NE(move3.Get(), nullptr);
        EXPECT_EQ(move3(2, 3), 3);
    }

    TEST(FunctorTests, NullptrAssign)
    {
        Functor<int(int, int)> functor1 = AddTestFunction;
        Functor<int(int, int)> functor2 = [=](int x, int y) -> int { return x - y; };
        Functor<int(int, int)> functor3 = &TestClass::AddStatic;

        functor1 = nullptr;
        functor2 = nullptr;
        functor3 = nullptr;

        EXPECT_EQ(functor1.Get(), nullptr);
        EXPECT_EQ(functor2.Get(), nullptr);
        EXPECT_EQ(functor3.Get(), nullptr);
    }

    TEST(FunctorTests, CallableAssign)
    {
        Functor<int(int, int)> functor1 = AddTestFunction;
        Functor<int(int, int)> functor2 = [=](int x, int y) -> int { return x - y; };
        Functor<int(int, int)> functor3 = &TestClass::AddStatic;

        functor1 = AddTestFunction;
        functor2 = &TestClass::AddStatic;
        functor3 = [](int x, int y) -> int { return x - y; };

        EXPECT_NE(functor1.Get(), nullptr);
        EXPECT_NE(functor2.Get(), nullptr);
        EXPECT_NE(functor3.Get(), nullptr);

        EXPECT_EQ(functor1(2, 5), 7);
        EXPECT_EQ(functor2(2, 5), 5);
        EXPECT_EQ(functor3(2, 5), -3);
    }

    TEST(FunctorTests, OperatorBool)
    {
        Functor<int(float, long)> empty;
        Functor<int(int, int)> functor = AddTestFunction;
        Functor<int(int, int)> functor2 = [=](int, int) -> int { return 2; };

        EXPECT_FALSE(empty);

        EXPECT_TRUE(functor);
        EXPECT_TRUE(functor2);
    }

    TEST(FunctorTests, IsEmpty)
    {
        Functor<int(float, long)> empty;
        Functor<int(int, int)> functor = AddTestFunction;
        Functor<int(int, int)> functor2 = [=](int, int) -> int { return 2; };

        EXPECT_TRUE(empty.IsEmpty());

        EXPECT_FALSE(functor.IsEmpty());
        EXPECT_FALSE(functor2.IsEmpty());
    }

    TEST(FunctorTests, SwapFunction)
    {
        Functor<int(int, int)> functor = AddTestFunction;
        Functor<int(int, int)> functor2 = [=](int, int) -> int { return 2; };

        functor.Swap(functor2);

        EXPECT_NE(functor.Get(), nullptr);
        EXPECT_EQ(functor(1, 96), 2);

        EXPECT_NE(functor2.Get(), nullptr);
        EXPECT_EQ(functor2(2, 2), 4);
    }
}
