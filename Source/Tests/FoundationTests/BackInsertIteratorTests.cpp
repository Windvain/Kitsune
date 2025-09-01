#include <gtest/gtest.h>
#include "Foundation/Iterators/BackInsertIterator.h"

using namespace Kitsune;

namespace
{
    class MyContainer
    {
    public:
        using ValueType = int;
        using Iterator = int*;
        using ConstIterator = const int*;

        void PushBack(int x)
        {
            Container.push_back(x);
        }

    public:
        int* GetBegin() { return nullptr; }
        const int* GetBegin() const { return nullptr; }

        int* GetEnd() { return nullptr; }
        const int* GetEnd() const { return nullptr; }

        bool operator==(const MyContainer&) const { return true; }

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
    MyContainer cont;
    BackInsertIterator<MyContainer> it(cont);

    EXPECT_EQ(it.GetContainer(), &cont);
}

TEST(BackInsertIteratorTests, Assign)
{
    MyContainer cont;
    BackInsertIterator<MyContainer> it(cont);

    it = 27;
    it = 3;

    EXPECT_EQ(cont.Container.size(), 2);
    EXPECT_EQ(cont.Container[0], 27);
    EXPECT_EQ(cont.Container[1], 3);
}
