#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "TestContainer.h"
#include "IteratorWrappers.h"

#include "Foundation/Algorithms/Destroy.h"

namespace DestroyTesting
{
    class D
    {
    public:
        D() { /* ... */ }
        D(const D&) { /* ... */ }

        ~D() { OnDestroy(); }

    public:
        MOCK_METHOD(void, OnDestroy, ());
    };
}

using namespace Kitsune;
using namespace DestroyTesting;

using TestContainer = Testing::TestContainer<Testing::ForwardIteratorWrapper<D>>;

TEST(DestroyTests, Destroy)
{
    D* arr = (D*)Memory::Allocate(sizeof(D) * 5, alignof(D));
    TestContainer cont(arr, arr + 5);

    for (int i = 0; i < 5; ++i)
    {
        Memory::ConstructAt(arr + i);
        EXPECT_CALL(arr[i], OnDestroy());
    }

    Algorithms::Destroy(cont.Begin, cont.End);
    Memory::Free(arr);
}

TEST(DestroyTests, DestroyN)
{
    D* arr = (D*)Memory::Allocate(sizeof(D) * 5, alignof(D));
    TestContainer cont(arr, arr + 5);

    for (int i = 0; i < 5; ++i)
    {
        Memory::ConstructAt(arr + i);
        EXPECT_CALL(arr[i], OnDestroy());
    }

    auto it = Algorithms::DestroyN(cont.Begin, 5);
    EXPECT_EQ(it, cont.End);

    Memory::Free(arr);
}
