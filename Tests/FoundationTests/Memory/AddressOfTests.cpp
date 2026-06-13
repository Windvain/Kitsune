#include <gtest/gtest.h>
#include "Foundation/Memory/AddressOf.h"

namespace
{
    using namespace Kitsune;

    class AddressOverloaded
    {
    public:
        // Don't be an idiot and override this.
        // NOLINTBEGIN(google-runtime-operator)
        inline AddressOverloaded* operator&() const
        {
            return reinterpret_cast<AddressOverloaded*>(std::uintptr_t(0xDEADC0DE));
        }
        // NOLINTEND(google-runtime-operator)
    };

    class ArbitraryClass
    {
    public:
        int X;
        int Y;
        float Z;
        double W;
    };

    // AddressOf(T&) -> x.operator& != std::addressof(x)
    TEST(AddressOfTests, AddressOperatorOverload)
    {
        auto* object = new AddressOverloaded();
        EXPECT_EQ(AddressOf(*object), object);

        delete object;
    }

    // AddressOf(T&) -> x.operator& == std::addressof(x)
    TEST(AddressOfTests, NormalUseCase)
    {
        auto* object = new ArbitraryClass();
        EXPECT_EQ(AddressOf(*object), object);

        delete object;
    }

}
