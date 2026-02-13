#include <gtest/gtest.h>
#include "Foundation/Memory/AddressOf.h"

namespace
{
    class A
    {
    public:
        inline A* operator&() const
        {
            return (A*)(std::uintptr_t)0xDEADC0DE;
        }
    };

    class B
    {
    public:
        int X;
        int Y;
        float Z;
        double W;
    };
}

using namespace Kitsune;

TEST(AddressOfTests, AddressOperatorOverload)
{
    A* object = new A();
    EXPECT_EQ(AddressOf(*object), object);

    delete object;
}

TEST(AddressOfTests, NormalUseCase)
{
    B* object = new B();
    EXPECT_EQ(AddressOf(*object), object);

    delete object;
}
