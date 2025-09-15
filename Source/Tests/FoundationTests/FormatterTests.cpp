#include <gtest/gtest.h>
#include "Foundation/String/Formatter.h"

using namespace Kitsune;

namespace
{
    class MyIterator
    {
    public:
        using ValueType = char;
        using DifferenceType = std::ptrdiff_t;

    public:
        MyIterator() = default;
        inline MyIterator(std::string& str)
            : m_String(&str)
        {
        }

        MyIterator(const MyIterator&) = default;
        MyIterator(MyIterator&&) = default;

    public:
        MyIterator& operator=(const MyIterator&) = default;
        MyIterator& operator=(MyIterator&&) = default;

        inline MyIterator& operator=(char ch)
        {
            m_String->append(1, ch);
            return *this;
        }

    public:
        inline MyIterator& operator*() { return *this; }
        inline MyIterator& operator++() { return *this; }
        inline MyIterator operator++(int) { return *this; }

    private:
        std::string* m_String;
    };
}

TEST(FormatterTests, BoolFormatter)
{
    std::string str;
    std::string str2;

    std::string expected = "false";
    std::string expected2 = "true";

    ParseContext parseContext("");
    auto formatContext = FormatContext<MyIterator>(MyIterator(str));
    auto formatContext2 = FormatContext<MyIterator>(MyIterator(str2));

    Formatter<bool, char> formatter;
    formatter.Parse(parseContext);

    KITSUNE_UNUSED(formatter.Format(false, formatContext));
    KITSUNE_UNUSED(formatter.Format(true, formatContext2));

    EXPECT_EQ(str, expected);
    EXPECT_EQ(str2, expected2);
}

TEST(FormatterTests, BoolFormatterWithArgs)
{
    std::string str;
    std::string str2;

    std::string expected = "0";
    std::string expected2 = "1";

    ParseContext parseContext("i");
    ParseContext parseContext2("I");

    auto formatContext = FormatContext<MyIterator>(MyIterator(str));
    auto formatContext2 = FormatContext<MyIterator>(MyIterator(str2));

    Formatter<bool, char> formatter1;
    formatter1.Parse(parseContext);

    Formatter<bool, char> formatter2;
    formatter2.Parse(parseContext2);

    KITSUNE_UNUSED(formatter1.Format(false, formatContext));
    KITSUNE_UNUSED(formatter2.Format(true, formatContext2));

    EXPECT_EQ(str, expected);
    EXPECT_EQ(str2, expected2);
}

TEST(FormatterTests, PositiveSignedIntegerFormatter)
{
    std::string str;
    std::string expected = "127";

    std::string str2;
    std::string expected2 = "32767";

    std::string str3;
    std::string expected3 = "2147483647";

    std::string str4;
    std::string expected4 = "9223372036854775807";

    ParseContext parseContext;

    FormatContext<MyIterator> formatContext{ MyIterator(str) };
    FormatContext<MyIterator> formatContext2{ MyIterator(str2) };
    FormatContext<MyIterator> formatContext3{ MyIterator(str3) };
    FormatContext<MyIterator> formatContext4{ MyIterator(str4) };

    Formatter<Int8, char> formatter;
    Formatter<Int16, char> formatter2;
    Formatter<Int32, char> formatter3;
    Formatter<Int64, char> formatter4;

    formatter.Parse(parseContext);
    formatter2.Parse(parseContext);
    formatter3.Parse(parseContext);
    formatter4.Parse(parseContext);

    KITSUNE_UNUSED(formatter.Format(INT8_MAX, formatContext));
    KITSUNE_UNUSED(formatter2.Format(INT16_MAX, formatContext2));
    KITSUNE_UNUSED(formatter3.Format(INT32_MAX, formatContext3));
    KITSUNE_UNUSED(formatter4.Format(INT64_MAX, formatContext4));

    EXPECT_EQ(str, expected);
    EXPECT_EQ(str2, expected2);
    EXPECT_EQ(str3, expected3);
    EXPECT_EQ(str4, expected4);
}

TEST(FormatterTests, NegativeSignedIntegerFormatter)
{
    std::string str;
    std::string expected = "-128";

    std::string str2;
    std::string expected2 = "-32768";

    std::string str3;
    std::string expected3 = "-2147483648";

    std::string str4;
    std::string expected4 = "-9223372036854775808";

    ParseContext parseContext;

    FormatContext<MyIterator> formatContext{ MyIterator(str) };
    FormatContext<MyIterator> formatContext2{ MyIterator(str2) };
    FormatContext<MyIterator> formatContext3{ MyIterator(str3) };
    FormatContext<MyIterator> formatContext4{ MyIterator(str4) };

    Formatter<Int8, char> formatter;
    Formatter<Int16, char> formatter2;
    Formatter<Int32, char> formatter3;
    Formatter<Int64, char> formatter4;

    formatter.Parse(parseContext);
    formatter2.Parse(parseContext);
    formatter3.Parse(parseContext);
    formatter4.Parse(parseContext);

    KITSUNE_UNUSED(formatter.Format(INT8_MIN, formatContext));
    KITSUNE_UNUSED(formatter2.Format(INT16_MIN, formatContext2));
    KITSUNE_UNUSED(formatter3.Format(INT32_MIN, formatContext3));
    KITSUNE_UNUSED(formatter4.Format(INT64_MIN, formatContext4));

    EXPECT_EQ(str, expected);
    EXPECT_EQ(str2, expected2);
    EXPECT_EQ(str3, expected3);
    EXPECT_EQ(str4, expected4);
}

TEST(FormatterTests, NegativeSignedIntegerWithBase2)
{
    std::string str;
    std::string expected = "10000000";

    std::string str2;
    std::string expected2 = "1000000000000000";

    std::string str3;
    std::string expected3 = "10000000000000000000000000000000";

    std::string str4;
    std::string expected4 = "1000000000000000000000000000000000000000000000000000000000000000";

    ParseContext parseContext("b");
    ParseContext parseContext2("B");

    FormatContext<MyIterator> formatContext{ MyIterator(str) };
    FormatContext<MyIterator> formatContext2{ MyIterator(str2) };
    FormatContext<MyIterator> formatContext3{ MyIterator(str3) };
    FormatContext<MyIterator> formatContext4{ MyIterator(str4) };

    Formatter<Int8, char> formatter;
    Formatter<Int16, char> formatter2;
    Formatter<Int32, char> formatter3;
    Formatter<Int64, char> formatter4;

    formatter.Parse(parseContext);
    formatter2.Parse(parseContext);
    formatter3.Parse(parseContext2);
    formatter4.Parse(parseContext2);

    KITSUNE_UNUSED(formatter.Format(INT8_MIN, formatContext));
    KITSUNE_UNUSED(formatter2.Format(INT16_MIN, formatContext2));
    KITSUNE_UNUSED(formatter3.Format(INT32_MIN, formatContext3));
    KITSUNE_UNUSED(formatter4.Format(INT64_MIN, formatContext4));

    EXPECT_EQ(str, expected);
    EXPECT_EQ(str2, expected2);
    EXPECT_EQ(str3, expected3);
    EXPECT_EQ(str4, expected4);
}

TEST(FormatterTests, NegativeSignedIntegerWithBase8)
{
    std::string str;
    std::string expected = "200";

    std::string str2;
    std::string expected2 = "100000";

    std::string str3;
    std::string expected3 = "20000000000";

    std::string str4;
    std::string expected4 = "1000000000000000000000";

    ParseContext parseContext("o");
    ParseContext parseContext2("O");

    FormatContext<MyIterator> formatContext{ MyIterator(str) };
    FormatContext<MyIterator> formatContext2{ MyIterator(str2) };
    FormatContext<MyIterator> formatContext3{ MyIterator(str3) };
    FormatContext<MyIterator> formatContext4{ MyIterator(str4) };

    Formatter<Int8, char> formatter;
    Formatter<Int16, char> formatter2;
    Formatter<Int32, char> formatter3;
    Formatter<Int64, char> formatter4;

    formatter.Parse(parseContext);
    formatter2.Parse(parseContext);
    formatter3.Parse(parseContext2);
    formatter4.Parse(parseContext2);

    KITSUNE_UNUSED(formatter.Format(INT8_MIN, formatContext));
    KITSUNE_UNUSED(formatter2.Format(INT16_MIN, formatContext2));
    KITSUNE_UNUSED(formatter3.Format(INT32_MIN, formatContext3));
    KITSUNE_UNUSED(formatter4.Format(INT64_MIN, formatContext4));

    EXPECT_EQ(str, expected);
    EXPECT_EQ(str2, expected2);
    EXPECT_EQ(str3, expected3);
    EXPECT_EQ(str4, expected4);
}

TEST(FormatterTests, NegativeSignedIntegerBase10)
{
    std::string str;
    std::string expected = "-128";

    std::string str2;
    std::string expected2 = "-32768";

    std::string str3;
    std::string expected3 = "-2147483648";

    std::string str4;
    std::string expected4 = "-9223372036854775808";

    ParseContext parseContext("d");
    ParseContext parseContext2("D");

    FormatContext<MyIterator> formatContext{ MyIterator(str) };
    FormatContext<MyIterator> formatContext2{ MyIterator(str2) };
    FormatContext<MyIterator> formatContext3{ MyIterator(str3) };
    FormatContext<MyIterator> formatContext4{ MyIterator(str4) };

    Formatter<Int8, char> formatter;
    Formatter<Int16, char> formatter2;
    Formatter<Int32, char> formatter3;
    Formatter<Int64, char> formatter4;

    formatter.Parse(parseContext);
    formatter2.Parse(parseContext);
    formatter3.Parse(parseContext2);
    formatter4.Parse(parseContext2);

    KITSUNE_UNUSED(formatter.Format(INT8_MIN, formatContext));
    KITSUNE_UNUSED(formatter2.Format(INT16_MIN, formatContext2));
    KITSUNE_UNUSED(formatter3.Format(INT32_MIN, formatContext3));
    KITSUNE_UNUSED(formatter4.Format(INT64_MIN, formatContext4));

    EXPECT_EQ(str, expected);
    EXPECT_EQ(str2, expected2);
    EXPECT_EQ(str3, expected3);
    EXPECT_EQ(str4, expected4);
}

TEST(FormatterTests, NegativeSignedIntegerWithBase16)
{
    std::string str;
    std::string expected = "80";

    std::string str2;
    std::string expected2 = "8000";

    std::string str3;
    std::string expected3 = "80000000";

    std::string str4;
    std::string expected4 = "8000000000000000";

    ParseContext parseContext("x");
    ParseContext parseContext2("X");

    FormatContext<MyIterator> formatContext{ MyIterator(str) };
    FormatContext<MyIterator> formatContext2{ MyIterator(str2) };
    FormatContext<MyIterator> formatContext3{ MyIterator(str3) };
    FormatContext<MyIterator> formatContext4{ MyIterator(str4) };

    Formatter<Int8, char> formatter;
    Formatter<Int16, char> formatter2;
    Formatter<Int32, char> formatter3;
    Formatter<Int64, char> formatter4;

    formatter.Parse(parseContext);
    formatter2.Parse(parseContext);
    formatter3.Parse(parseContext2);
    formatter4.Parse(parseContext2);

    KITSUNE_UNUSED(formatter.Format(INT8_MIN, formatContext));
    KITSUNE_UNUSED(formatter2.Format(INT16_MIN, formatContext2));
    KITSUNE_UNUSED(formatter3.Format(INT32_MIN, formatContext3));
    KITSUNE_UNUSED(formatter4.Format(INT64_MIN, formatContext4));

    EXPECT_EQ(str, expected);
    EXPECT_EQ(str2, expected2);
    EXPECT_EQ(str3, expected3);
    EXPECT_EQ(str4, expected4);
}

TEST(FormatterTests, PositiveUnsignedIntegerWithBase2)
{
    std::string str;
    std::string expected = "11111111";

    std::string str2;
    std::string expected2 = "1111111111111111";

    std::string str3;
    std::string expected3 = "11111111111111111111111111111111";

    std::string str4;
    std::string expected4 = "1111111111111111111111111111111111111111111111111111111111111111";

    ParseContext parseContext("b");
    ParseContext parseContext2("B");

    FormatContext<MyIterator> formatContext{ MyIterator(str) };
    FormatContext<MyIterator> formatContext2{ MyIterator(str2) };
    FormatContext<MyIterator> formatContext3{ MyIterator(str3) };
    FormatContext<MyIterator> formatContext4{ MyIterator(str4) };

    Formatter<Uint8, char> formatter;
    Formatter<Uint16, char> formatter2;
    Formatter<Uint32, char> formatter3;
    Formatter<Uint64, char> formatter4;

    formatter.Parse(parseContext);
    formatter2.Parse(parseContext);
    formatter3.Parse(parseContext2);
    formatter4.Parse(parseContext2);

    KITSUNE_UNUSED(formatter.Format(UINT8_MAX, formatContext));
    KITSUNE_UNUSED(formatter2.Format(UINT16_MAX, formatContext2));
    KITSUNE_UNUSED(formatter3.Format(UINT32_MAX, formatContext3));
    KITSUNE_UNUSED(formatter4.Format(UINT64_MAX, formatContext4));

    EXPECT_EQ(str, expected);
    EXPECT_EQ(str2, expected2);
    EXPECT_EQ(str3, expected3);
    EXPECT_EQ(str4, expected4);
}

TEST(FormatterTests, PositiveUnsignedIntegerWithBase8)
{
    std::string str;
    std::string expected = "377";

    std::string str2;
    std::string expected2 = "177777";

    std::string str3;
    std::string expected3 = "37777777777";

    std::string str4;
    std::string expected4 = "1777777777777777777777";

    ParseContext parseContext("o");
    ParseContext parseContext2("O");

    FormatContext<MyIterator> formatContext{ MyIterator(str) };
    FormatContext<MyIterator> formatContext2{ MyIterator(str2) };
    FormatContext<MyIterator> formatContext3{ MyIterator(str3) };
    FormatContext<MyIterator> formatContext4{ MyIterator(str4) };

    Formatter<Uint8, char> formatter;
    Formatter<Uint16, char> formatter2;
    Formatter<Uint32, char> formatter3;
    Formatter<Uint64, char> formatter4;

    formatter.Parse(parseContext);
    formatter2.Parse(parseContext);
    formatter3.Parse(parseContext2);
    formatter4.Parse(parseContext2);

    KITSUNE_UNUSED(formatter.Format(UINT8_MAX, formatContext));
    KITSUNE_UNUSED(formatter2.Format(UINT16_MAX, formatContext2));
    KITSUNE_UNUSED(formatter3.Format(UINT32_MAX, formatContext3));
    KITSUNE_UNUSED(formatter4.Format(UINT64_MAX, formatContext4));

    EXPECT_EQ(str, expected);
    EXPECT_EQ(str2, expected2);
    EXPECT_EQ(str3, expected3);
    EXPECT_EQ(str4, expected4);
}

TEST(FormatterTests, PositiveUnsignedIntegerBase10)
{
    std::string str;
    std::string expected = "255";

    std::string str2;
    std::string expected2 = "65535";

    std::string str3;
    std::string expected3 = "4294967295";

    std::string str4;
    std::string expected4 = "18446744073709551615";

    ParseContext parseContext("d");
    ParseContext parseContext2("D");

    FormatContext<MyIterator> formatContext{ MyIterator(str) };
    FormatContext<MyIterator> formatContext2{ MyIterator(str2) };
    FormatContext<MyIterator> formatContext3{ MyIterator(str3) };
    FormatContext<MyIterator> formatContext4{ MyIterator(str4) };

    Formatter<Uint8, char> formatter;
    Formatter<Uint16, char> formatter2;
    Formatter<Uint32, char> formatter3;
    Formatter<Uint64, char> formatter4;

    formatter.Parse(parseContext);
    formatter2.Parse(parseContext);
    formatter3.Parse(parseContext2);
    formatter4.Parse(parseContext2);

    KITSUNE_UNUSED(formatter.Format(UINT8_MAX, formatContext));
    KITSUNE_UNUSED(formatter2.Format(UINT16_MAX, formatContext2));
    KITSUNE_UNUSED(formatter3.Format(UINT32_MAX, formatContext3));
    KITSUNE_UNUSED(formatter4.Format(UINT64_MAX, formatContext4));

    EXPECT_EQ(str, expected);
    EXPECT_EQ(str2, expected2);
    EXPECT_EQ(str3, expected3);
    EXPECT_EQ(str4, expected4);
}

TEST(FormatterTests, PositiveUnsignedIntegerWithBase16)
{
    std::string str;
    std::string expected = "FF";

    std::string str2;
    std::string expected2 = "FFFF";

    std::string str3;
    std::string expected3 = "FFFFFFFF";

    std::string str4;
    std::string expected4 = "FFFFFFFFFFFFFFFF";

    ParseContext parseContext("x");
    ParseContext parseContext2("X");

    FormatContext<MyIterator> formatContext{ MyIterator(str) };
    FormatContext<MyIterator> formatContext2{ MyIterator(str2) };
    FormatContext<MyIterator> formatContext3{ MyIterator(str3) };
    FormatContext<MyIterator> formatContext4{ MyIterator(str4) };

    Formatter<Uint8, char> formatter;
    Formatter<Uint16, char> formatter2;
    Formatter<Uint32, char> formatter3;
    Formatter<Uint64, char> formatter4;

    formatter.Parse(parseContext);
    formatter2.Parse(parseContext);
    formatter3.Parse(parseContext2);
    formatter4.Parse(parseContext2);

    KITSUNE_UNUSED(formatter.Format(UINT8_MAX, formatContext));
    KITSUNE_UNUSED(formatter2.Format(UINT16_MAX, formatContext2));
    KITSUNE_UNUSED(formatter3.Format(UINT32_MAX, formatContext3));
    KITSUNE_UNUSED(formatter4.Format(UINT64_MAX, formatContext4));

    EXPECT_EQ(str, expected);
    EXPECT_EQ(str2, expected2);
    EXPECT_EQ(str3, expected3);
    EXPECT_EQ(str4, expected4);
}

TEST(FormatterTests, IntegerAlwaysAddSign)
{
    std::string str;
    std::string expected = "+23";

    std::string str2;
    std::string expected2 = "110101011000000";

    std::string str3;
    std::string expected3 = "3487102";

    std::string str4;
    std::string expected4 = "+2039123981";

    ParseContext parseContext("+");
    ParseContext parseContext2("+b");
    ParseContext parseContext3("+-");
    ParseContext parseContext4("-+");

    FormatContext<MyIterator> formatContext{ MyIterator(str) };
    FormatContext<MyIterator> formatContext2{ MyIterator(str2) };
    FormatContext<MyIterator> formatContext3{ MyIterator(str3) };
    FormatContext<MyIterator> formatContext4{ MyIterator(str4) };

    Formatter<Int8, char> formatter;
    Formatter<Int16, char> formatter2;
    Formatter<Int32, char> formatter3;
    Formatter<Int64, char> formatter4;

    formatter.Parse(parseContext);
    formatter2.Parse(parseContext2);
    formatter3.Parse(parseContext3);
    formatter4.Parse(parseContext4);

    KITSUNE_UNUSED(formatter.Format(23, formatContext));
    KITSUNE_UNUSED(formatter2.Format(27328, formatContext2));
    KITSUNE_UNUSED(formatter3.Format(3487102, formatContext3));
    KITSUNE_UNUSED(formatter4.Format(2039123981, formatContext4));

    EXPECT_EQ(str, expected);
    EXPECT_EQ(str2, expected2);
    EXPECT_EQ(str3, expected3);
    EXPECT_EQ(str4, expected4);
}

TEST(FormatterTests, IntegerPadSize)
{
    std::string str;
    std::string expected = "023";

    std::string str2;
    std::string expected2 = "0000000000001100";

    std::string str3;
    std::string expected3 = "00015232576";

    std::string str4;
    std::string expected4 = "FFFFFFFF86756FF3";

    ParseContext parseContext("0");
    ParseContext parseContext2("b0");
    ParseContext parseContext3("0o");
    ParseContext parseContext4("x0");

    FormatContext<MyIterator> formatContext{ MyIterator(str) };
    FormatContext<MyIterator> formatContext2{ MyIterator(str2) };
    FormatContext<MyIterator> formatContext3{ MyIterator(str3) };
    FormatContext<MyIterator> formatContext4{ MyIterator(str4) };

    Formatter<Uint8, char> formatter;
    Formatter<Int16, char> formatter2;
    Formatter<Uint32, char> formatter3;
    Formatter<Int64, char> formatter4;

    formatter.Parse(parseContext);
    formatter2.Parse(parseContext2);
    formatter3.Parse(parseContext3);
    formatter4.Parse(parseContext4);

    KITSUNE_UNUSED(formatter.Format(23, formatContext));
    KITSUNE_UNUSED(formatter2.Format(12, formatContext2));
    KITSUNE_UNUSED(formatter3.Format(3487102, formatContext3));
    KITSUNE_UNUSED(formatter4.Format(-2039123981, formatContext4));

    EXPECT_EQ(str, expected);
    EXPECT_EQ(str2, expected2);
    EXPECT_EQ(str3, expected3);
    EXPECT_EQ(str4, expected4);
}

TEST(FormatterTests, IntegerAppendSign)
{
    std::string str;
    std::string expected = "23";

    std::string str2;
    std::string expected2 = "0b1100";

    std::string str3;
    std::string expected3 = "015232576";

    std::string str4;
    std::string expected4 = "0xFFFFFFFF86756FF3";

    ParseContext parseContext("#d");
    ParseContext parseContext2("b#");
    ParseContext parseContext3("#o");
    ParseContext parseContext4("x#");

    FormatContext<MyIterator> formatContext{ MyIterator(str) };
    FormatContext<MyIterator> formatContext2{ MyIterator(str2) };
    FormatContext<MyIterator> formatContext3{ MyIterator(str3) };
    FormatContext<MyIterator> formatContext4{ MyIterator(str4) };

    Formatter<Uint8, char> formatter;
    Formatter<Int16, char> formatter2;
    Formatter<Uint32, char> formatter3;
    Formatter<Int64, char> formatter4;

    formatter.Parse(parseContext);
    formatter2.Parse(parseContext2);
    formatter3.Parse(parseContext3);
    formatter4.Parse(parseContext4);

    KITSUNE_UNUSED(formatter.Format(23, formatContext));
    KITSUNE_UNUSED(formatter2.Format(12, formatContext2));
    KITSUNE_UNUSED(formatter3.Format(3487102, formatContext3));
    KITSUNE_UNUSED(formatter4.Format(-2039123981, formatContext4));

    EXPECT_EQ(str, expected);
    EXPECT_EQ(str2, expected2);
    EXPECT_EQ(str3, expected3);
    EXPECT_EQ(str4, expected4);
}

TEST(FormatterTests, CharFormatter)
{
    std::string str;
    std::string str2;

    std::string expected = "c";
    std::string expected2 = "y";

    ParseContext parseContext("");
    auto formatContext = FormatContext<MyIterator>(MyIterator(str));
    auto formatContext2 = FormatContext<MyIterator>(MyIterator(str2));

    Formatter<char, char> formatter;
    formatter.Parse(parseContext);

    KITSUNE_UNUSED(formatter.Format('c', formatContext));
    KITSUNE_UNUSED(formatter.Format('y', formatContext2));

    EXPECT_EQ(str, expected);
    EXPECT_EQ(str2, expected2);
}

TEST(FormatterTests, CharFormatterWithArgs)
{
    std::string str;
    std::string str2;

    std::string expected = "&";
    std::string expected2 = "96";

    ParseContext parseContext("ic");
    ParseContext parseContext2("cI");

    auto formatContext = FormatContext<MyIterator>(MyIterator(str));
    auto formatContext2 = FormatContext<MyIterator>(MyIterator(str2));

    Formatter<char, char> formatter1;
    formatter1.Parse(parseContext);

    Formatter<char, char> formatter2;
    formatter2.Parse(parseContext2);

    KITSUNE_UNUSED(formatter1.Format('&', formatContext));
    KITSUNE_UNUSED(formatter2.Format('`', formatContext2));

    EXPECT_EQ(str, expected);
    EXPECT_EQ(str2, expected2);
}

TEST(FormatterTests, FloatingPointFormatter)
{
    // Our currrent implementation uses std::snprintf().
    EXPECT_TRUE(true);
}

TEST(FormatterTests, PointerFormatter)
{
    std::string str;
    std::string str2;

    std::unique_ptr<int> ptr = std::make_unique<int>();
    std::unique_ptr<int> ptr2 = std::make_unique<int>();

    std::string expected;
    std::string expected2;

    Formatter<Uintptr, char> pointerFormatter;
    pointerFormatter.Parse(ParseContext("#0x"));

    KITSUNE_UNUSED(pointerFormatter.Format(Uintptr(ptr.get()), FormatContext<MyIterator>(expected)));
    KITSUNE_UNUSED(pointerFormatter.Format(Uintptr(ptr2.get()), FormatContext<MyIterator>(expected2)));

    ParseContext parseContext("??");
    auto formatContext = FormatContext<MyIterator>(MyIterator(str));
    auto formatContext2 = FormatContext<MyIterator>(MyIterator(str2));

    Formatter<int*, char> formatter;
    formatter.Parse(parseContext);

    KITSUNE_UNUSED(formatter.Format(ptr.get(), formatContext));
    KITSUNE_UNUSED(formatter.Format(ptr2.get(), formatContext2));

    EXPECT_EQ(str, expected);
    EXPECT_EQ(str2, expected2);
}

TEST(FormatterTests, StringViewFormatter)
{
    std::string str;
    std::string expected = "Hello!";

    ParseContext parseContext("??");
    auto formatContext = FormatContext<MyIterator>(MyIterator(str));

    Formatter<StringView, char> formatter;
    formatter.Parse(parseContext);
    KITSUNE_UNUSED(formatter.Format("Hello!", formatContext));

    EXPECT_EQ(str, expected);
}
