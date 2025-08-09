#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "TestContainer2.h"
#include "Foundation/Algorithms/Destroy.h"

namespace
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

    D* CreateUninitializedD()
    {
        return static_cast<D*>(std::malloc(sizeof(D)));
    }
}

using namespace Kitsune;
using namespace Testing;

TEST(DestroyTests, Destroy)
{
    ForwardTestContainer<D*, 5> container = {
        CreateUninitializedD(),
        CreateUninitializedD(),
        CreateUninitializedD(),
        CreateUninitializedD(),
        CreateUninitializedD()
    };

    for (int i = 0; i < 5; ++i)
    {
        std::construct_at(container[i]);
        EXPECT_CALL(*container[i], OnDestroy());
    }

    Algorithms::Destroy(container.GetBegin(), container.GetEnd());

    for (std::size_t i = 0; i < 5; ++i)
        std::free(container[i]);
}

TEST(DestroyTests, DestroyN)
{
    ForwardTestContainer<D*, 5> container = {
        CreateUninitializedD(),
        CreateUninitializedD(),
        CreateUninitializedD(),
        CreateUninitializedD(),
        CreateUninitializedD()
    };

    for (int i = 0; i < 5; ++i)
    {
        std::construct_at(container[i]);
        EXPECT_CALL(*container[i], OnDestroy());
    }

    Algorithms::DestroyN(container.GetBegin(), 5);

    for (std::size_t i = 0; i < 5; ++i)
        std::free(container[i]);
}
