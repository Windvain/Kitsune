#include "Foundation/Diagnostics/SourceLocation.h"
#include <gtest/gtest.h>

using namespace Kitsune;

TEST(SourceLocationTests, DefaultConstructor)
{
    SourceLocation location;

    EXPECT_EQ(location.FileName(), "<unknown>");
    EXPECT_EQ(location.FunctionName(), "<unknown>");
    EXPECT_EQ(location.Line(), 0);
}

TEST(SourceLocationTests, Current)
{
    SourceLocation location = SourceLocation::Current();
    EXPECT_EQ(location.Line(), 17);

    /* FileName() and FunctionName()'s outputs are different
     * based on the compiler. */
}

TEST(SourceLocation, EqualOperator)
{
    SourceLocation location = SourceLocation::Current();
    SourceLocation location2 = location;

    EXPECT_EQ(location, location2);
}
