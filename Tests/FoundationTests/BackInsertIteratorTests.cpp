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
            Vector.push_back(x);
        }

        Usize Size() const
        {
            return Vector.size();
        }

        bool IsEmpty() const
        {
            return Vector.empty();
        }

    public:
        // Unused.
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
            Vector.swap(container.Vector);
        }

        bool operator==(const MyContainer&) const
        {
            return true;
        }

    public:
        std::vector<int> Vector;
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

    EXPECT_EQ(container.Vector.size(), 2);
    EXPECT_EQ(container.Vector[0], 27);
    EXPECT_EQ(container.Vector[1], 3);
}
