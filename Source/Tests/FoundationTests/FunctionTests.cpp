#include <gtest/gtest.h>
#include "Foundation/Utilities/Function.h"

namespace
{
    class Functor
    {
    public:
        long operator()(int x) const
        {
            return static_cast<long>(x) + 3;
        }
    };

    long TestFunction(int)
    {
        return 3;
    }
}

using namespace Kitsune;

TEST(FunctionTests, DefaultCtor)
{
    Function<int(float)> func;
    EXPECT_EQ(func.Target(), nullptr);
}

TEST(FunctionTests, FunctorCtor)
{
    Function<long(int)> func = Functor();
    EXPECT_NE(func.Target(), nullptr);
    EXPECT_EQ(func(3), 6l);
}

TEST(FunctionTests, CopyCtor)
{
    Function<long(int)> func = Functor();
    Function<long(int)> f2 = func;

    EXPECT_NE(f2.Target(), func.Target());
    EXPECT_EQ(f2(2), 5);
    EXPECT_EQ(func(3), 6);
}

TEST(FunctionTests, MoveCtor)
{
    Function<long(int)> func = Functor();
    Function<long(int)> moved = std::move(func);

    EXPECT_EQ(func.Target(), nullptr);
    EXPECT_NE(moved.Target(), nullptr);
    EXPECT_EQ(moved(3), 6l);
}

TEST(FunctionTests, Destructor)
{
    EXPECT_TRUE(true);
}

TEST(FunctionTests, CopyAssign)
{
    Function<long(int)> func = Functor();
    Function<long(int)> f2 = Functor();

    f2 = func;

    EXPECT_NE(f2.Target(), func.Target());
    EXPECT_EQ(f2(2), 5);
    EXPECT_EQ(func(3), 6);
}

TEST(FunctionTests, MoveAssign)
{
    Function<long(int)> func = Functor();
    Function<long(int)> moved = Functor();

    moved = std::move(func);

    EXPECT_EQ(func.Target(), nullptr);
    EXPECT_NE(moved.Target(), nullptr);
    EXPECT_EQ(moved(3), 6l);
}

TEST(FunctionTests, FunctionAssign)
{
    Function<long(int)> func = Functor();
    func = [](int) -> long { return 3; };

    EXPECT_NE(func.Target(), nullptr);
    EXPECT_EQ(func(123), 3l);
}

TEST(FunctionTests, Boolean)
{
    Function<long(int)> func;
    Function<long(int)> f2 = [&](int) -> long { return 2; };

    EXPECT_FALSE((bool)func);
    EXPECT_TRUE((bool)f2);
}

TEST(FunctionTests, Call)
{
    Function<long(int)> func = [](int x) -> long { return x + 2; };
    Function<long(int)> empty;

    EXPECT_EQ(func(2), 4l);
    EXPECT_THROW(empty(3), BadCallException);
}

TEST(FunctionTests, Target)
{
    Function<long(int)> func = &TestFunction;
    Function<long(int)> empty;

    EXPECT_EQ(empty.Target(), nullptr);
    EXPECT_NE(func.Target(), nullptr);
}

TEST(FunctionTests, SwapMemberFunction)
{
    Function<long(int)> func = &TestFunction;
    Function<long(int)> func2 = [](int) -> long { return 2; };

    void* ptr = func2.Target();
    void* ptr2 = func.Target();

    func.Swap(func2);

    EXPECT_EQ(func.Target(), ptr);
    EXPECT_EQ(func2.Target(), ptr2);
}

TEST(FunctionTests, SwapAlgorithm)
{
    Function<long(int)> func = &TestFunction;
    Function<long(int)> func2 = [](int) -> long { return 2; };

    void* ptr = func2.Target();
    void* ptr2 = func.Target();

    Algorithms::Swap(func, func2);

    EXPECT_EQ(func.Target(), ptr);
    EXPECT_EQ(func2.Target(), ptr2);
}
