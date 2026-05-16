#include "Foundation/Algorithms/Destroy.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "TestContainer.h"

using namespace Kitsune;

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
        MOCK_METHOD(void, OnDestroy, ());
    };
}

using namespace Kitsune;
using namespace Kitsune::Testing;

TEST(DestroyTests, Destroy)
{
    ForwardNonOwningTestContainer<MyDestroyObject, 5> container(
        (MyDestroyObject*)std::malloc(sizeof(MyDestroyObject) * 5));

    for (int i = 0; i < 5; ++i)
    {
        std::construct_at(container.m_Array + i);
        EXPECT_CALL(container[i], OnDestroy());
    }

    Algorithms::Destroy(container.GetBegin(), container.GetEnd());
    std::free(container.m_Array);
}

TEST(DestroyTests, DestroyN)
{
    ForwardNonOwningTestContainer<MyDestroyObject, 5> container(
        (MyDestroyObject*)std::malloc(sizeof(MyDestroyObject) * 5));

    for (int i = 0; i < 5; ++i)
    {
        std::construct_at(container.m_Array + i);
        EXPECT_CALL(container[i], OnDestroy());
    }

    Algorithms::DestroyN(container.GetBegin(), 5);
    std::free(container.m_Array);
}
