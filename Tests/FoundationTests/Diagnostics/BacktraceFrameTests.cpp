#include <gtest/gtest.h>
#include "Foundation/Diagnostics/BacktraceFrame.h"

namespace
{
    using namespace Kitsune;

    // BacktraceFrame::BacktraceFrame()
    TEST(BacktraceFrameTest, DefaultConstructor)
    {
        BacktraceFrame frame;
        EXPECT_EQ(frame.GetFileName(), "<unknown>");
        EXPECT_EQ(frame.GetSymbolName(), "<unknown>");
        EXPECT_EQ(frame.GetLineNumber(), 0);
        EXPECT_EQ(frame.GetAddress(), nullptr);
    }

    // BacktraceFrame::BacktraceFrame(StringView, StringView, Uint64, void*)
    TEST(BacktraceFrameTest, Constructor)
    {
        BacktraceFrame frame("some_file.h", "foo::bar()", 3, (void*)0xDEAD);

        EXPECT_EQ(frame.GetFileName(), "some_file.h");
        EXPECT_EQ(frame.GetSymbolName(), "foo::bar()");
        EXPECT_EQ(frame.GetLineNumber(), 3);
        EXPECT_EQ(frame.GetAddress(), (void*)0xDEAD);
    }

    // BacktraceFrame::operator==(const BacktraceFrame&)
    TEST(BacktraceFrameTest, Comparison)
    {
        BacktraceFrame frame("some_file.h", "foo::bar()", 3, (void*)0xDEAD);

        // NOLINTNEXTLINE(performance-unnecessary-copy-initialization)
        BacktraceFrame equalFrame = frame;
        BacktraceFrame inequalFrame("another_file.h", "foo::bar()", 3, (void*)0xDEAD);

        ASSERT_EQ(frame.GetSymbolName(), equalFrame.GetSymbolName());
        ASSERT_EQ(frame.GetFileName(), equalFrame.GetFileName());
        ASSERT_EQ(frame.GetAddress(), equalFrame.GetAddress());
        ASSERT_EQ(frame.GetLineNumber(), equalFrame.GetLineNumber());

        EXPECT_TRUE(frame == equalFrame);
        EXPECT_FALSE(frame == inequalFrame);

        EXPECT_FALSE(frame != equalFrame);
        EXPECT_TRUE(frame != inequalFrame);
    }
}
