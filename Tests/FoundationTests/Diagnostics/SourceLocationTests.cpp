#include <gtest/gtest.h>
#include "Foundation/Diagnostics/SourceLocation.h"

namespace
{
    using namespace Kitsune;

    // SourceLocation::SourceLocation()
    TEST(SourceLocationTest, DefaultConstructor)
    {
        SourceLocation location;

        EXPECT_EQ(location.FileName(), "<unknown>");
        EXPECT_EQ(location.FunctionName(), "<unknown>");
        EXPECT_EQ(location.Line(), 0);
    }

    // SourceLocation::Current(/* ... */)
    TEST(SourceLocationTest, Current)
    {
        SourceLocation location = SourceLocation::Current();
        EXPECT_EQ(location.Line(), 21);

        /* The outputs of FileName() and FunctionName() differ between compilers. */
    }

    // SourceLocation::operator==(const SourceLocation&)
    TEST(SourceLocation, EqualOperator)
    {
        SourceLocation location = SourceLocation::Current();

        SourceLocation sameLocation = location;
        SourceLocation diffLocation = SourceLocation::Current();

        EXPECT_TRUE(location == sameLocation);
        EXPECT_FALSE(location == diffLocation);

        EXPECT_TRUE(location != diffLocation);
        EXPECT_FALSE(location != sameLocation);
    }
}
