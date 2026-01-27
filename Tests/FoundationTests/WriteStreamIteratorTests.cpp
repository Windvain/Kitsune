#include <gtest/gtest.h>
#include "Foundation/Logging/WriteStreamIterator.h"

using namespace Kitsune;

namespace
{
    class MyStream : public IWriteStream<int>
    {
    public:
        void Write(const int* x, Usize size) override
        {
            std::vector<int> put(x, x + size);
            Output.insert(Output.end(), put.begin(), put.end());
        }

        void Flush() override { /* ... */ }

    public:
        std::vector<int> Output;
    };
}

TEST(WriteStreamIteratorTests, DefaultCtor)
{
    WriteStreamIterator<int> it{};
    EXPECT_EQ(it.GetStream(), nullptr);
}

TEST(WriteStreamIteratorTests, StreamCtor)
{
    MyStream stream;
    WriteStreamIterator<int> it(stream);

    EXPECT_EQ(it.GetStream(), &stream);
}

TEST(WriteStreamIteratorTests, Assign)
{
    MyStream stream;
    WriteStreamIterator<int> it(stream);

    it = 27;

    EXPECT_EQ(stream.Output.size(), 1);
    EXPECT_EQ(stream.Output[0], 27);
}
