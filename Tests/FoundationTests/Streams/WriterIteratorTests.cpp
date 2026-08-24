#include <gtest/gtest.h>
#include "Foundation/Streams/WriterIterator.h"

using namespace Kitsune;

namespace
{
    class MyWriter
    {
    public:
        using ValueType = char;

    public:
        MyWriter() = default;
        inline MyWriter(std::string& string)
            : m_String(std::addressof(string))
        {
        }

        inline void Write(const char* data, Usize count)
        {
            *m_String += std::string_view(data, count);
        }

        inline void WriteLine(const char* data, Usize count)
        {
            Write(data, count);
            Write("\n", 1);
        }

        inline void Flush()
        {
        }

    private:
        std::string* m_String;
    };

    static_assert(
        Writer<MyWriter>,
        "MyWriter does not satisfy the requirements for Writer<T>.");

    TEST(WriterIteratorTest, DefaultConstructor)
    {
        WriterIterator<MyWriter> iterator{};
        EXPECT_EQ(iterator.GetWriter(), nullptr);
    }

    TEST(WriterIteratorTest, WriterConstructor)
    {
        MyWriter writer{};
        WriterIterator<MyWriter> iterator(writer);

        EXPECT_EQ(iterator.GetWriter(), &writer);
    }

    TEST(WriterIteratorTest, Assign)
    {
        std::string string;

        MyWriter writer(string);
        WriterIterator<MyWriter> iterator(writer);

        iterator = 'H';
        iterator = 'e';
        iterator = 'l';
        iterator = 'l';
        iterator = 'o';

        EXPECT_EQ(string, "Hello");
    }
}
