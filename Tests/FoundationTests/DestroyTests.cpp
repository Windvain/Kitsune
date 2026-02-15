#include "Foundation/Algorithms/Destroy.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "TestContainer.h"

namespace
{
    class MyDestroyObject
    {
    public:
        MyDestroyObject()
        {
        }

        ~MyDestroyObject()
        {
            OnDestroy();
        }

    public:
        static MyDestroyObject* AllocateObject()
        {
            return (MyDestroyObject*)(std::malloc(sizeof(MyDestroyObject)));
        }

    public:
        MOCK_METHOD(void, OnDestroy, ());
    };
}

using namespace Kitsune;
using namespace Kitsune::Testing;

TEST(DestroyTests, Destroy)
{
    ForwardTestContainer<MyDestroyObject*, 5> container = { /* ... */ };

    for (int i = 0; i < 5; ++i)
    {
        container[i] = MyDestroyObject::AllocateObject();
        std::construct_at(container[i]);

        EXPECT_CALL(*container[i], OnDestroy());
    }

    Algorithms::Destroy(container.GetBegin(), container.GetEnd());

    for (std::size_t i = 0; i < 5; ++i)
        std::free(container[i]);
}

TEST(DestroyTests, DestroyN)
{
    ForwardTestContainer<MyDestroyObject*, 5> container = { /* ... */ };

    for (int i = 0; i < 5; ++i)
    {
        container[i] = MyDestroyObject::AllocateObject();
        std::construct_at(container[i]);

        EXPECT_CALL(*container[i], OnDestroy());
    }

    Algorithms::DestroyN(container.GetBegin(), 5);

    for (std::size_t i = 0; i < 5; ++i)
        std::free(container[i]);
}
