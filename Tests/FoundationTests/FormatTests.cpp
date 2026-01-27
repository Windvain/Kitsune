#include <gtest/gtest.h>
#include "Foundation/String/Format.h"

namespace
{
    struct A { /* ... */ };
}

namespace Kitsune
{
    template<>
    class Formatter<A, char>
    {
    public:
        void Parse(const ParseContext& context)
        {
            if (context.GetFormatSpecs().Front() == 'A')
                m_SomeRandomFlag = true;
        }

        template<WritableIterator<char> Iter>
        Iter Format(A /* object */, const FormatContext<Iter>& context)
        {
            std::string str = m_SomeRandomFlag ? "<Flag Set>" : "<Flag Not Set>";
            return Algorithms::Copy(str.data(), str.data() + str.size(), context.GetOutput());
        }

    private:
        bool m_SomeRandomFlag = false;
    };

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

using namespace Kitsune;

TEST(FormatTests, IdealString)
{
    std::string str;
    FormatTo(MyIterator(str), StringView("{0}, {1:A} awd {0:A} {{ }}"),
             A(), A());

    EXPECT_EQ(str, "<Flag Not Set>, <Flag Set> awd <Flag Set> { }");
}

TEST(FormatTests, StringithMatchedBraces)
{
    std::string str;
    FormatTo(MyIterator(str), StringView("{{}} }}{{ {{{{"));

    EXPECT_EQ(str, "{} }{ {{");
}

TEST(FormatTests, InvalidArguments)
{
    std::string str;
    FormatTo(MyIterator(str), StringView("{0:}"), A());                 // No arguments, technically valid but kind of dumb.

    EXPECT_EQ(str, "<Flag Not Set>");

    EXPECT_THROW(FormatTo(MyIterator(str), StringView("{}"), A()), FormatException);            // No index argument, invalid.
    EXPECT_THROW(FormatTo(MyIterator(str), StringView("{{}")), FormatException);                // Unmatched braces, invalid.
    EXPECT_THROW(FormatTo(MyIterator(str), StringView("{}}")), FormatException);                // Unmatched braces, invalid.
    EXPECT_THROW(FormatTo(MyIterator(str), StringView("{isudn}")), FormatException);            // Gibberish as the index argument, invalid.
    EXPECT_THROW(FormatTo(MyIterator(str), StringView("{0} {1}"), A()), OutOfRangeException);   // Specified an out-of-range argument.
}
