#include <gtest/gtest.h>
#include "Foundation/Logging/StreamBuffer.h"

using namespace Kitsune;

TEST(StreamBufferTests, DefaultCtor)
{
    StreamBuffer<int> buffer;
    EXPECT_EQ(buffer.GetBegin(), nullptr);
    EXPECT_EQ(buffer.GetCurrent(), nullptr);
    EXPECT_EQ(buffer.GetEnd(), nullptr);
}

TEST(StreamBufferTests, PointerCtor)
{
    StreamBuffer<int> buffer((int*)0x1234, (int*)0x1235);
    EXPECT_EQ(buffer.GetBegin(), (int*)0x1234);
    EXPECT_EQ(buffer.GetCurrent(), (int*)0x1234);
    EXPECT_EQ(buffer.GetEnd(), (int*)0x1235);
}

TEST(StreamBufferTests, GetSize)
{
    int arr[3] = { 1, 2, 3 };
    StreamBuffer<int> buffer(arr, arr + 3);

    EXPECT_EQ(buffer.GetSize(), 3);
}

TEST(StreamBufferTests, GetWrittenCount)
{
    int arr[4] = { 3, 4, 5, 6 };
    StreamBuffer<int> buffer(arr, arr + 4);

    EXPECT_EQ(buffer.GetWrittenCount(), 0);

    /* Further tests in SetPointer() */
}

TEST(StreamBufferTests, GetRemainingCapacity)
{
    int arr[9] = { 3, 4, 5, 6, 2, 3, 2, 1, 1 };
    StreamBuffer<int> buffer(arr, arr + 9);

    EXPECT_EQ(buffer.GetRemainingCapacity(), 9);

    /* Further tests in SetPointer() */
}

TEST(StreamBufferTests, BumpPointer)
{
    int arr[5] = { 3, 4, 5, 1, 5 };
    StreamBuffer<int> buffer(arr, arr + 5);

    buffer.BumpPointer(2);

    EXPECT_EQ(buffer.GetWrittenCount(), 2);
    EXPECT_EQ(buffer.GetRemainingCapacity(), 3);
}

TEST(StreamBufferTests, SetPointer)
{
    int arr[5] = { 3, 4, 5, 1, 5 };
    StreamBuffer<int> buffer(arr, arr + 5);

    buffer.SetPointer(arr + 2);

    EXPECT_EQ(buffer.GetWrittenCount(), 2);
    EXPECT_EQ(buffer.GetRemainingCapacity(), 3);
}

TEST(StreamBufferTests, FrontBack)
{
    int arr[5] = { 3, 4, 5, 1, 5 };
    StreamBuffer<int> buffer(arr, arr + 5);

    buffer.BumpPointer(2);

    EXPECT_EQ(buffer.Front(), 3);
    EXPECT_EQ(buffer.Back(), 4);
}
