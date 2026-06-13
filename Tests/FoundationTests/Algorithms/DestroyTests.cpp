#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "TestContainer.h"

#include "Foundation/Algorithms/Destroy.h"

namespace
{
    using namespace Kitsune;
    using Testing::ForwardNonOwningTestContainer;

    class DestructionTracker
    {
    public:
        DestructionTracker() = default;
        ~DestructionTracker()
        {
            OnDestroy();
        }

    public:
        MOCK_METHOD(void, OnDestroy, ());
    };

    class DestroyTest : public ::testing::Test
    {
    protected:
        using ContainerType = ForwardNonOwningTestContainer<DestructionTracker, 5>;

        // NOLINTBEGIN(cppcoreguidelines-pro-type-member-init): `Container` is
        // initialized in the constructor body.
        inline DestroyTest()
        {
            void* pointer = std::malloc(5 * sizeof(DestructionTracker));
            Container = ContainerType(static_cast<DestructionTracker*>(pointer));
        }
        // NOLINTEND(cppcoreguidelines-pro-type-member-init)

        inline ~DestroyTest()
        {
            std::free(Container.m_Array);
        }

    protected:
        inline void SetUp() override
        {
            for (int index = 0; index < 5; ++index)
                std::construct_at(Container.m_Array + index);
        }

        inline void TearDown() override
        {
            // Do nothing, we expect the test cases to destroy the objects.
        }

    protected:
        ContainerType Container;
    };

    // Algorithms::Destroy(Iter, Iter)
    TEST_F(DestroyTest, Destroy)
    {
        for (int index = 0; index < 5; ++index)
            EXPECT_CALL(this->Container[index], OnDestroy);

        Algorithms::Destroy(this->Container.GetBegin(), this->Container.GetEnd());
    }

    // Algorithms::DestroyN(Iter, Size)
    TEST_F(DestroyTest, DestroyN)
    {
        for (int index = 0; index < 5; ++index)
            EXPECT_CALL(this->Container[index], OnDestroy);

        Algorithms::DestroyN(this->Container.GetBegin(), this->Container.Size());
    }
}
