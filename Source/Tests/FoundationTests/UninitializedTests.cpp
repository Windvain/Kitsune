#include <gtest/gtest.h>
#include "Foundation/Algorithms/Uninitialized.h"

#include <cstring>
#include "TestContainer.h"

#include "CompareStrings.h"
#include "IteratorWrappers.h"

using namespace Kitsune;

namespace UninitializedTesting
{
    class C
    {
    public:
        C(int x) : Value(x) { /* ... */ }
        C(const C&) = default;
        C(C&& c)
        {
            Value = std::exchange(c.Value, 0);
        }

    public:
        int Value;
    };
}

using namespace UninitializedTesting;

class UninitializedTests : public testing::Test
{
private:
    template<typename T>
    using TemplatedTestCont = Testing::TestContainer<Testing::ForwardIteratorWrapper<T>>;

protected:
    using TestContainer = Testing::TestContainer<Testing::ForwardIteratorWrapper<std::string>>;
    using CTestContainer = Testing::TestContainer<Testing::ForwardIteratorWrapper<C>>;

protected:
    void SetUp() override
    {
        SetupContainer(StringContainer);
        SetupContainer(CContainer);
    }

    void TearDown() override
    {
        FreeContainer(CContainer);
        FreeContainer(StringContainer);
    }

private:
    template<typename T>
    void SetupContainer(TemplatedTestCont<T>& cont)
    {
        T* arr = (T*)std::malloc(5 * sizeof(T));
        if (arr == nullptr)
            return;

        std::memset(arr, 0b01010101, 5 * sizeof(T));     // Fill with absolute garbage.
        cont = TemplatedTestCont<T>(arr, arr + 5);
    }

    template<typename T>
    void FreeContainer(TemplatedTestCont<T>& cont)
    {
        std::free(cont.Begin.Pointer());
    }

protected:
    TestContainer StringContainer;
    CTestContainer CContainer;
};

template<typename It>
void DestroyContainer(Testing::TestContainer<It>& cont)
{
    using T = IteratorTraits<It>::ValueType;
    for (auto it = cont.Begin; it != cont.End; ++it)
        (*it).~T();
}

TEST_F(UninitializedTests, UninitializedCopy)
{
    std::string arr[5] = { "Hello", "there", "this", "is", "text" };
    TestContainer cont(arr, arr + 5);

    auto it = Algorithms::UninitializedCopy(cont.Begin, cont.End, StringContainer.Begin);
    EXPECT_EQ(it, StringContainer.End);

    std::string* begin = StringContainer.Begin.Pointer();
    EXPECT_GENERAL_STREQ(begin[0].c_str(), "Hello");
    EXPECT_GENERAL_STREQ(begin[1].c_str(), "there");
    EXPECT_GENERAL_STREQ(begin[2].c_str(), "this");
    EXPECT_GENERAL_STREQ(begin[3].c_str(), "is");
    EXPECT_GENERAL_STREQ(begin[4].c_str(), "text");

    DestroyContainer(StringContainer);
}

TEST_F(UninitializedTests, UninitializedCopyN)
{
    std::string arr[5] = { "Hello", "there", "this", "is", "text" };
    TestContainer cont(arr, arr + 5);

    auto it = Algorithms::UninitializedCopyN(cont.Begin, 5, StringContainer.Begin);
    EXPECT_EQ(it, StringContainer.End);

    std::string* begin = StringContainer.Begin.Pointer();
    EXPECT_GENERAL_STREQ(begin[0].c_str(), "Hello");
    EXPECT_GENERAL_STREQ(begin[1].c_str(), "there");
    EXPECT_GENERAL_STREQ(begin[2].c_str(), "this");
    EXPECT_GENERAL_STREQ(begin[3].c_str(), "is");
    EXPECT_GENERAL_STREQ(begin[4].c_str(), "text");

    DestroyContainer(StringContainer);
}

TEST_F(UninitializedTests, UninitializedMove)
{
    C arr[5] = { 54, 23, 1, 5, 7 };
    CTestContainer cont(arr, arr + 5);

    auto it = Algorithms::UninitializedMove(cont.Begin, cont.End, CContainer.Begin);
    EXPECT_EQ(it, CContainer.End);

    EXPECT_EQ(arr[0].Value, 0);
    EXPECT_EQ(arr[1].Value, 0);
    EXPECT_EQ(arr[2].Value, 0);
    EXPECT_EQ(arr[3].Value, 0);
    EXPECT_EQ(arr[4].Value, 0);

    C* begin = CContainer.Begin.Pointer();
    EXPECT_EQ(begin[0].Value, 54);
    EXPECT_EQ(begin[1].Value, 23);
    EXPECT_EQ(begin[2].Value, 1);
    EXPECT_EQ(begin[3].Value, 5);
    EXPECT_EQ(begin[4].Value, 7);

    DestroyContainer(CContainer);
}

TEST_F(UninitializedTests, UninitializedMoveN)
{
    C arr[5] = { 54, 23, 1, 5, 7 };
    CTestContainer cont(arr, arr + 5);

    auto it = Algorithms::UninitializedMoveN(cont.Begin, 5, CContainer.Begin);
    EXPECT_EQ(it, CContainer.End);

    EXPECT_EQ(arr[0].Value, 0);
    EXPECT_EQ(arr[1].Value, 0);
    EXPECT_EQ(arr[2].Value, 0);
    EXPECT_EQ(arr[3].Value, 0);
    EXPECT_EQ(arr[4].Value, 0);

    C* begin = CContainer.Begin.Pointer();
    EXPECT_EQ(begin[0].Value, 54);
    EXPECT_EQ(begin[1].Value, 23);
    EXPECT_EQ(begin[2].Value, 1);
    EXPECT_EQ(begin[3].Value, 5);
    EXPECT_EQ(begin[4].Value, 7);

    DestroyContainer(CContainer);
}

TEST_F(UninitializedTests, UninitializedFill)
{
    std::string* begin = StringContainer.Begin.Pointer();
    Algorithms::UninitializedFill(StringContainer.Begin, StringContainer.End, "Hello!");

    EXPECT_GENERAL_STREQ(begin[0].c_str(), "Hello!");
    EXPECT_GENERAL_STREQ(begin[1].c_str(), "Hello!");
    EXPECT_GENERAL_STREQ(begin[2].c_str(), "Hello!");
    EXPECT_GENERAL_STREQ(begin[3].c_str(), "Hello!");
    EXPECT_GENERAL_STREQ(begin[4].c_str(), "Hello!");
}

TEST_F(UninitializedTests, UninitializedFillN)
{
    auto it = Algorithms::UninitializedFillN(StringContainer.Begin, 5, "Hello!");
    EXPECT_EQ(it, StringContainer.End);

    std::string* begin = StringContainer.Begin.Pointer();
    EXPECT_GENERAL_STREQ(begin[0].c_str(), "Hello!");
    EXPECT_GENERAL_STREQ(begin[1].c_str(), "Hello!");
    EXPECT_GENERAL_STREQ(begin[2].c_str(), "Hello!");
    EXPECT_GENERAL_STREQ(begin[3].c_str(), "Hello!");
    EXPECT_GENERAL_STREQ(begin[4].c_str(), "Hello!");
}
