#include <gtest/gtest.h>
#include "Foundation/Memory/Memory.h"

namespace MemoryTesting
{
    struct alignas(16) B
    {
    public:
        float D1, D2, D3, D4;
    };
}

using namespace Kitsune;
using namespace MemoryTesting;

TEST(MemoryTests, TryAllocateDefaultAlign)
{
    Usize size = 100;
    Int8* ptr = (Int8*)Memory::TryAllocate(size);

    ptr[size - 1] = 117;        // Make sure that it can be written to..
    EXPECT_EQ((Uintptr)ptr % Memory::GetDefaultAlignment(), 0);

    Memory::Free(ptr);
}

TEST(MemoryTests, TryAllocateSpecifiedAlign)
{
    Usize size = 100;
    Usize align = 128;

    Int8* ptr = (Int8*)Memory::TryAllocate(size, align);

    ptr[size - 1] = 117;        // Make sure that it can be written to..
    EXPECT_EQ((Uintptr)ptr % align, 0);

    Memory::Free(ptr);
}

TEST(MemoryTests, NewAlignment)
{
    float d1 = 2.4f;
    float d2 = 3.2f;

    B* ptr = Memory::New<B>(d1, d2);
    EXPECT_FLOAT_EQ(ptr->D1, d1);
    EXPECT_FLOAT_EQ(ptr->D2, d2);

    EXPECT_EQ((Uintptr)ptr % alignof(B), 0);
}
