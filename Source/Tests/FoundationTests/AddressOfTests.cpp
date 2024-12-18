#include <gtest/gtest.h>

#include "Foundation/Common/Types.h"
#include "Foundation/Memory/AddressOf.h"

namespace AddressOfTesting
{
    class A
    {
    public:
        A* operator&() { return reinterpret_cast<A*>((Kitsune::Uintptr)0xDEADC0DE); }
    };
}

using namespace Kitsune;
using namespace AddressOfTesting;

TEST(AddressOfTests, Overloaded)
{
    A object;
    EXPECT_EQ(AddressOf(object), (A*)&(char&)(object));
}

TEST(AddressOfTests, NotOverloaded)
{
    int x;
    EXPECT_EQ(AddressOf(x), &x);
}
