#include "Foundation/Diagnostics/BacktraceFrame.h"
#include <gtest/gtest.h>

using namespace Kitsune;

TEST(BacktraceFrameTests, DefaultConstructor)
{
    BacktraceFrame frame;
    EXPECT_EQ(frame.GetFileName(), "<unknown>");
    EXPECT_EQ(frame.GetSymbolName(), "<unknown>");
    EXPECT_EQ(frame.GetLineNumber(), 0);
    EXPECT_EQ(frame.GetAddress(), nullptr);
}

TEST(BacktraceFrameTests, Constructor)
{
    BacktraceFrame frame("some_file.h", "foo::bar()", 3, (void*)0xDEAD);

    EXPECT_EQ(frame.GetFileName(), "some_file.h");
    EXPECT_EQ(frame.GetSymbolName(), "foo::bar()");
    EXPECT_EQ(frame.GetLineNumber(), 3);
    EXPECT_EQ(frame.GetAddress(), (void*)0xDEAD);
}

TEST(BacktraceFrameTests, Comparison)
{
    BacktraceFrame frame("some_file.h", "foo::bar()", 3, (void*)0xDEAD);
    BacktraceFrame copiedFrame = frame;

    EXPECT_EQ(frame.GetSymbolName(), copiedFrame.GetSymbolName());
    EXPECT_EQ(frame.GetFileName(), copiedFrame.GetFileName());
    EXPECT_EQ(frame.GetAddress(), copiedFrame.GetAddress());
    EXPECT_EQ(frame.GetLineNumber(), copiedFrame.GetLineNumber());
}
