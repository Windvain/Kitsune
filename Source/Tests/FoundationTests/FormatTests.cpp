#include <gtest/gtest.h>
#include "CompareStrings.h"

#include <typeinfo>
#include "Foundation/String/Format.h"

using namespace Kitsune;

namespace FormatTesting
{
    class C { /* ... */ };
}

using namespace FormatTesting;
namespace Kitsune
{
    template<>
    class Formatter<C, char>
    {
    public:
        void Parse(const ParseContext<char>& ctx)
        {
            auto it = ctx.GetBegin();
            if (it == ctx.GetEnd()) return;

            if (*it == 'f')
                m_Flag = true;

            ++it;
        }
        String Format(C) { return m_Flag ? "[FLAG SET]" : "[UNKNOWN]"; }

    private:
        bool m_Flag = false;
    };
}

TEST(FormatTests, ThrowingFormatStrings)
{
    EXPECT_THROW(Format("{}"), FormatException);
    EXPECT_THROW(Format("}"), FormatException);
    EXPECT_THROW(Format("{"), FormatException);
    EXPECT_THROW(Format("}{"), FormatException);
}

TEST(FormatTests, CallsFormatter)
{
    EXPECT_GENERAL_STREQ(Format("{0}", C()).Raw(), "[UNKNOWN]");
    EXPECT_GENERAL_STREQ(Format("{0:?}", C()).Raw(), "[UNKNOWN]");
    EXPECT_GENERAL_STREQ(Format("{0:f}", C()).Raw(), "[FLAG SET]");
}

TEST(FormatTests, NoFormattingUsage)
{
    EXPECT_GENERAL_STREQ(Format("Hello, I am just a regular string.").Raw(), "Hello, I am just a regular string.");
}

TEST(FormatTests, CorrectIndexing)
{
    EXPECT_GENERAL_STREQ(Format("{1}, {0}!", "World", "Hello").Raw(), "Hello, World!");
}

TEST(FormatTests, BooleanFormatting)
{
    EXPECT_GENERAL_STREQ(Format("{0} {1}", true, false).Raw(), "true false");
    EXPECT_GENERAL_STREQ(Format("{0:s} {1:s}", true, false).Raw(), "true false");
    EXPECT_GENERAL_STREQ(Format("{0:S} {1:S}", true, false).Raw(), "true false");
    EXPECT_GENERAL_STREQ(Format("{0:?} {1:?}", true, false).Raw(), "1 0");
}

TEST(FormatTests, IntegralFormatting)
{
    EXPECT_GENERAL_STREQ(Format("{0}", -27).Raw(), ToString(-27, 10).Raw());
    EXPECT_GENERAL_STREQ(Format("{0:d}", -3).Raw(), ToString(-3, 10).Raw());
    EXPECT_GENERAL_STREQ(Format("{0:D}", -3).Raw(), ToString(-3, 10).Raw());

    EXPECT_GENERAL_STREQ(Format("{0:b}", -3).Raw(), ToString(-3, 2).Raw());
    EXPECT_GENERAL_STREQ(Format("{0:B}", -63).Raw(), ToString(-63, 2).Raw());

    EXPECT_GENERAL_STREQ(Format("{0:o}", -3).Raw(), ToString(-3, 8).Raw());
    EXPECT_GENERAL_STREQ(Format("{0:O}", -3).Raw(), ToString(-3, 8).Raw());

    EXPECT_GENERAL_STREQ(Format("{0:x}", -3).Raw(), ToString(-3, 16).Raw());
    EXPECT_GENERAL_STREQ(Format("{0:X}", -3).Raw(), ToString(-3, 16).Raw());

    EXPECT_GENERAL_STREQ(Format("{0:d}", 256).Raw(), ToString(256, 10).Raw());
    EXPECT_GENERAL_STREQ(Format("{0:D}", 256).Raw(), ToString(256, 10).Raw());

    EXPECT_GENERAL_STREQ(Format("{0:b}", 72).Raw(), ToString(72, 2).Raw());
    EXPECT_GENERAL_STREQ(Format("{0:B}", 256).Raw(), ToString(256, 2).Raw());

    EXPECT_GENERAL_STREQ(Format("{0:o}", 256).Raw(), ToString(256, 8).Raw());
    EXPECT_GENERAL_STREQ(Format("{0:O}", 256).Raw(), ToString(256, 8).Raw());

    EXPECT_GENERAL_STREQ(Format("{0:x}", 256).Raw(), ToString(256, 16).Raw());
    EXPECT_GENERAL_STREQ(Format("{0:X}", 256).Raw(), ToString(256, 16).Raw());
}

TEST(FormatTests, FloatingPointFormatting)
{
    EXPECT_GENERAL_STREQ(Format("{0}", 2.0f).Raw(), ToString(2.0f).Raw());
    EXPECT_GENERAL_STREQ(Format("{0}", -32.0f).Raw(), ToString(-32.0f).Raw());
    EXPECT_GENERAL_STREQ(Format("{0}", 2.531).Raw(), ToString(2.531).Raw());
    EXPECT_GENERAL_STREQ(Format("{0}", -9.7).Raw(), ToString(-9.7).Raw());
}

TEST(FormatTests, CharFormatting)
{
    EXPECT_GENERAL_STREQ(Format("{0}", 'c').Raw(), "c");
    EXPECT_GENERAL_STREQ(Format("{0:?}", 'c').Raw(), ToString('c').Raw());

    EXPECT_GENERAL_STREQ(Format("{0:c}", 'c').Raw(), "c");
    EXPECT_GENERAL_STREQ(Format("{0:C}", 'c').Raw(), "c");
}

TEST(FormatTests, PointerFormatting)
{
    int x = 5;
    int* ptr = &x;

    Uintptr ptrRep;
    std::memcpy(&ptrRep, &ptr, sizeof(void*));

    EXPECT_GENERAL_STREQ(Format("{0}", ptr).Raw(), ToString<Uintptr>(ptrRep, 16).Raw());
}

TEST(FormatTests, StringFormatting)
{
    EXPECT_GENERAL_STREQ(Format("{0}", "Hello!").Raw(), "Hello!");
    EXPECT_GENERAL_STREQ(Format("{0}", StringView("Yoo!")).Raw(), "Yoo!");
    EXPECT_GENERAL_STREQ(Format("{0}", String("Flakjsdasdkasd")).Raw(), "Flakjsdasdkasd");
}
