#include "Foundation/Iterators/BackInsertIterator.h"
#include <gtest/gtest.h>

using namespace Kitsune;

namespace
{
    class MyContainer
    {
    public:
        using ValueType = int;

        using Iterator = int*;
        using ConstIterator = const int*;

    public:
        void PushBack(int x)
        {
            Container.push_back(x);
        }

        Usize Size() const
        {
            return Container.size();
        }

        bool IsEmpty() const
        {
            return Container.empty();
        }

    public:
        int* GetBegin()
        {
            return nullptr;
        }

        const int* GetBegin() const
        {
            return nullptr;
        }

        int* GetEnd()
        {
            return nullptr;
        }

        const int* GetEnd() const
        {
            return nullptr;
        }

        void Swap(MyContainer& container)
        {
            Container.swap(container.Container);
        }

        bool operator==(const MyContainer&) const
        {
            return true;
        }

    public:
        std::vector<int> Container;
    };
}

TEST(BackInsertIteratorTests, DefaultConstructor)
{
    BackInsertIterator<MyContainer> it{};
    EXPECT_EQ(it.GetContainer(), nullptr);
}

TEST(BackInsertIteratorTests, ContainerConstructor)
{
    MyContainer container;
    BackInsertIterator<MyContainer> it(container);

    EXPECT_EQ(it.GetContainer(), &container);
}

TEST(BackInsertIteratorTests, Assign)
{
    MyContainer container;
    BackInsertIterator<MyContainer> it(container);

    it = 27;
    it = 3;

    EXPECT_EQ(container.Container.size(), 2);
    EXPECT_EQ(container.Container[0], 27);
    EXPECT_EQ(container.Container[1], 3);
}
