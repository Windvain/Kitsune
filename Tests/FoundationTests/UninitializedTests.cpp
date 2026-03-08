#include "Foundation/Algorithms/Uninitialized.h"

#include <cstring>
#include <gtest/gtest.h>

#include "TestStrings.h"
#include "TestContainer.h"

using namespace Kitsune;
using namespace Kitsune::Testing;

namespace
{
    class A
    {
    public:
        A() = default;
        explicit A(int x)
            : Value(x)
        {
        }

        A(const A&) = default;
        A(A&& object)
        {
            Value = std::exchange(object.Value, 0);
        }

    public:
        int Value = 0;
    };
}

class UninitializedTests : public testing::Test
{
protected:
    template<typename T, Usize S>
    ForwardNonOwningTestContainer<T, S> CreateContainer()
    {
        auto* pointer = static_cast<T*>(std::malloc(sizeof(T) * S));
        if (pointer == nullptr)
            throw std::bad_alloc();

        std::memset((void*)pointer, 0b01010101, S * sizeof(T));
        return ForwardNonOwningTestContainer<T, S>(pointer);
    }

    template<typename T, Usize S>
    void DestroyContainer(ForwardNonOwningTestContainer<T, S> container)
    {
        std::free(container.m_Array);
    }
};

TEST_F(UninitializedTests, UninitializedCopy)
{
    std::string array[5] = { "Hello", "there", "this", "is", "text" };
    auto container = this->CreateContainer<std::string, 5>();

    auto it = Algorithms::UninitializedCopy(array, array + 5, container.GetBegin());
    EXPECT_EQ(it, container.GetEnd());

    std::string* begin = container.m_Array;
    EXPECT_GENERAL_STREQ(begin[0].c_str(), "Hello");
    EXPECT_GENERAL_STREQ(begin[1].c_str(), "there");
    EXPECT_GENERAL_STREQ(begin[2].c_str(), "this");
    EXPECT_GENERAL_STREQ(begin[3].c_str(), "is");
    EXPECT_GENERAL_STREQ(begin[4].c_str(), "text");

    std::destroy_n(container.m_Array, 5);
    this->DestroyContainer(container);
}

TEST_F(UninitializedTests, UninitializedCopyN)
{
    std::string array[5] = { "Hello", "there", "this", "is", "text" };
    auto container = this->CreateContainer<std::string, 5>();

    auto it = Algorithms::UninitializedCopyN(array, 5, container.GetBegin());
    EXPECT_EQ(it, container.GetEnd());

    std::string* begin = container.m_Array;
    EXPECT_GENERAL_STREQ(begin[0].c_str(), "Hello");
    EXPECT_GENERAL_STREQ(begin[1].c_str(), "there");
    EXPECT_GENERAL_STREQ(begin[2].c_str(), "this");
    EXPECT_GENERAL_STREQ(begin[3].c_str(), "is");
    EXPECT_GENERAL_STREQ(begin[4].c_str(), "text");

    std::destroy_n(container.m_Array, 5);
    this->DestroyContainer(container);
}

TEST_F(UninitializedTests, UninitializedMove)
{
    A array[5] = { A(2), A(543), A(123), A(340), A(11) };
    auto container = this->CreateContainer<A, 5>();

    auto it = Algorithms::UninitializedMove(array, array + 5, container.GetBegin());
    EXPECT_EQ(it, container.GetEnd());

    for (Index index = 0; index < 5; ++index)
        EXPECT_EQ(array[index].Value, 0);

    A* begin = container.m_Array;
    EXPECT_EQ(begin[0].Value, 2);
    EXPECT_EQ(begin[1].Value, 543);
    EXPECT_EQ(begin[2].Value, 123);
    EXPECT_EQ(begin[3].Value, 340);
    EXPECT_EQ(begin[4].Value, 11);

    std::destroy_n(container.m_Array, 5);
    this->DestroyContainer(container);
}

TEST_F(UninitializedTests, UninitializedMoveN)
{
    A array[5] = { A(2), A(543), A(123), A(340), A(11) };
    auto container = this->CreateContainer<A, 5>();

    auto it = Algorithms::UninitializedMoveN(array, 5, container.GetBegin());
    EXPECT_EQ(it, container.GetEnd());

    for (Index index = 0; index < 5; ++index)
        EXPECT_EQ(array[index].Value, 0);

    A* begin = container.m_Array;
    EXPECT_EQ(begin[0].Value, 2);
    EXPECT_EQ(begin[1].Value, 543);
    EXPECT_EQ(begin[2].Value, 123);
    EXPECT_EQ(begin[3].Value, 340);
    EXPECT_EQ(begin[4].Value, 11);

    std::destroy_n(container.m_Array, 5);
    this->DestroyContainer(container);
}

TEST_F(UninitializedTests, UninitializedFill)
{
    auto container = this->CreateContainer<std::string, 5>();
    Algorithms::UninitializedFill(container.GetBegin(), container.GetEnd(),
                                  "Some string");

    for (Index index = 0; index < 5; ++index)
        EXPECT_EQ(container[index], "Some string");

    std::destroy_n(container.m_Array, 5);
    this->DestroyContainer(container);
}

TEST_F(UninitializedTests, UninitializedFillN)
{
    auto container = this->CreateContainer<std::string, 5>();
    auto iter = Algorithms::UninitializedFillN(container.GetBegin(), 5,
                                  "Some string");

    EXPECT_EQ(iter, container.GetEnd());

    for (Index index = 0; index < 5; ++index)
        EXPECT_EQ(container[index], "Some string");

    std::destroy_n(container.m_Array, 5);
    this->DestroyContainer(container);
}
