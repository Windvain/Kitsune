#include <gtest/gtest.h>

#include <filesystem>
#include "Foundation/Filesystem/ToAbsolute.h"

using namespace Kitsune;

namespace
{
    TEST(ToAbsoluteTest, ToAbsolute)
    {
        std::string path = "./FoundationTests.exe";
        std::filesystem::path expected = std::filesystem::absolute(path);

        EXPECT_EQ(
            Filesystem::ToAbsolute(path.c_str()),
            expected.string().c_str());
    }
}
