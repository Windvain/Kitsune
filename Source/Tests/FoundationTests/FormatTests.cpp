#include <gtest/gtest.h>
#include "CompareStrings.h"

#include "Foundation/String/Format.h"

namespace
{
    class A { /* ... */ };
}

namespace Kitsune
{
    template<>
    class Formatter<A>
    {
    public:
        void Parse(const ParseContext& context)
        {
            auto it = context.GetBegin();
            if (it == context.GetEnd()) return;

            if (*it == 'f')
                m_Flag = true;
        }

        template<typename Out>
        Out Format(const FormatContext<A, Out>& context)
        {
            StringView str = (m_Flag) ? "<FLAG SET>" : "<FLAG NOT SET>";
            return Algorithms::Copy(str.GetBegin(), str.GetEnd(), context.GetOutput());
        }

    private:
        bool m_Flag = false;
    };
}

using namespace Kitsune;

TEST(FormatTests, CustomFormatter)
{
    EXPECT_GENERAL_STREQ(Format("{0}", A()).Data(), "<FLAG NOT SET>");
    EXPECT_GENERAL_STREQ(Format("{0:???}", A()).Data(), "<FLAG NOT SET>");
    EXPECT_GENERAL_STREQ(Format("{0:fer}", A()).Data(), "<FLAG SET>");
}

TEST(FormatTests, Indexing)
{
    EXPECT_GENERAL_STREQ(Format("{1}, {0}", "World!", "Hello").Data(), "Hello, World!");
}

TEST(FormatTests, BooleanFormatting)
{
    EXPECT_GENERAL_STREQ(Format("{0} {1}", true, false).Raw(), "true false");
    EXPECT_GENERAL_STREQ(Format("{0:?} {1:?}", true, false).Raw(), "true false");

    EXPECT_GENERAL_STREQ(Format("{0:i} {1:i}", true, false).Raw(), "1 0");
    EXPECT_GENERAL_STREQ(Format("{0:I} {1:I}", true, false).Raw(), "1 0");
}

TEST(FormatTests, IntegralFormatting)
{
    EXPECT_GENERAL_STREQ(Format("{0}", -27).Raw(), "-27");
    EXPECT_GENERAL_STREQ(Format("{0:d}", -3).Raw(),"-3");
    EXPECT_GENERAL_STREQ(Format("{0:D}", -3).Raw(), "-3");

    EXPECT_GENERAL_STREQ(Format("{0:b}", -3).Raw(), "1111111111111111111111111111111111111111111111111111111111111101");
    EXPECT_GENERAL_STREQ(Format("{0:B}", -63).Raw(), "1111111111111111111111111111111111111111111111111111111111000001");

    EXPECT_GENERAL_STREQ(Format("{0:o}", -3).Raw(), "1777777777777777777775");
    EXPECT_GENERAL_STREQ(Format("{0:O}", -3).Raw(), "1777777777777777777775");

    EXPECT_GENERAL_STREQ(Format("{0:x}", -3).Raw(), "FFFFFFFFFFFFFFFD");
    EXPECT_GENERAL_STREQ(Format("{0:X}", -3).Raw(), "FFFFFFFFFFFFFFFD");

    EXPECT_GENERAL_STREQ(Format("{0:d}", 256).Raw(), "256");
    EXPECT_GENERAL_STREQ(Format("{0:D}", 256).Raw(), "256");

    EXPECT_GENERAL_STREQ(Format("{0:b}", 72).Raw(), "1001000");
    EXPECT_GENERAL_STREQ(Format("{0:B}", 256).Raw(), "100000000");

    EXPECT_GENERAL_STREQ(Format("{0:o}", 256).Raw(), "400");
    EXPECT_GENERAL_STREQ(Format("{0:O}", 256).Raw(), "400");

    EXPECT_GENERAL_STREQ(Format("{0:x}", 256).Raw(), "100");
    EXPECT_GENERAL_STREQ(Format("{0:X}", 256).Raw(), "100");
}

TEST(FormatTests, FloatingPointFormatting)
{
    // Implemented using std::snprintf()..
}

TEST(FormatTests, CharFormatting)
{
    EXPECT_GENERAL_STREQ(Format("{0}", 'c').Raw(), "c");
    EXPECT_GENERAL_STREQ(Format("{0:?}", 'c').Raw(), "c");
}

TEST(FormatTests, PointerFormatting)
{
    int x = 5;
    int* ptr = &x;

    Uintptr ptrRep;
    std::memcpy(&ptrRep, &ptr, sizeof(void*));

    EXPECT_GENERAL_STREQ(Format("{0}", ptr).Raw(), Format("{0:x}", ptrRep).Raw());
}

TEST(FormatTests, StringFormatting)
{
    EXPECT_GENERAL_STREQ(Format("{0}", "Hello!").Raw(), "Hello!");
    EXPECT_GENERAL_STREQ(Format("{0}", StringView("Yoo!")).Raw(), "Yoo!");
    EXPECT_GENERAL_STREQ(Format("{0}", String("Flakjsdasdkasd")).Raw(), "Flakjsdasdkasd");
}
