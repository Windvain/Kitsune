#include <gtest/gtest.h>

#include <fstream>
#include <filesystem>

#include "Foundation/Filesystem/Filesystem.h"
#include "Foundation/Diagnostics/InvalidArgumentException.h"

using namespace Kitsune;

namespace
{
    TEST(FilesystemTest, CreateDirectory)
    {
        if (std::filesystem::exists("Directory1"))
            ASSERT_TRUE(std::filesystem::remove("Directory1"));

        EXPECT_TRUE(Filesystem::CreateDirectory("Directory1"));
        EXPECT_TRUE(std::filesystem::is_directory("Directory1"));

        std::filesystem::remove("Directory1");
    }

    TEST(FilesystemTest, CreateDirectoryFailCase)
    {
        std::filesystem::remove("Directory1");
        std::filesystem::create_directory("Directory1");

        ASSERT_TRUE(std::filesystem::is_directory("Directory1"));

        EXPECT_FALSE(Filesystem::CreateDirectory("Directory1"));
        EXPECT_FALSE(Filesystem::CreateDirectory("Directory1/Directory2/Directory3"));

        std::filesystem::remove("Directory1");
    }

    TEST(FilesystemTest, CreateSymbolicLink)
    {
        EXPECT_TRUE(true);
    }

    // TODO: Add tests for symlink versions.
    TEST(FilesystemTest, FileExists)
    {
        std::filesystem::remove("MyDirectory");
        std::filesystem::remove("MyFile");

        std::filesystem::create_directory("MyDirectory");
        std::ofstream stream("MyFile");
        stream.close();

        EXPECT_TRUE(Filesystem::FileExists("MyDirectory"));
        EXPECT_TRUE(Filesystem::FileExists("MyFile"));

        ASSERT_FALSE(std::filesystem::exists("OtherFile"));
        EXPECT_FALSE(Filesystem::FileExists("OtherFile"));

        std::filesystem::remove("MyDirectory");
        std::filesystem::remove("MyFile");
    }

    TEST(FilesystemTest, GetFileSize)
    {
        std::filesystem::remove("MyFile");
        std::ofstream stream("MyFile", std::ios::binary);

        for (int i = 0; i < 100; ++i)
            stream << "Hello";

        stream.close();

        EXPECT_EQ(Filesystem::GetFileSize("MyFile"), 500);

        std::filesystem::remove("RandomFile");
        std::filesystem::remove("MyDirectory");

        std::filesystem::create_directory("MyDirectory");

        EXPECT_THROW(Filesystem::GetFileSize("RandomFile"), InvalidArgumentException);
        EXPECT_THROW(Filesystem::GetFileSize("MyDirectory"), InvalidArgumentException);

        std::filesystem::remove("MyDirectory");
    }

    TEST(FilesystemTest, GetFileType)
    {
        std::filesystem::remove("MyDirectory");
        std::filesystem::remove("MyFile");

        std::filesystem::create_directory("MyDirectory");
        std::ofstream stream("MyFile");
        stream.close();

        EXPECT_EQ(
            Filesystem::GetFileType("MyFile"),
            Filesystem::FileType::File);

        EXPECT_TRUE(Filesystem::IsRegularFile("MyFile"));
        EXPECT_FALSE(Filesystem::IsDirectory("MyFile"));
        EXPECT_FALSE(Filesystem::IsSymbolicLink("MyFile"));

        EXPECT_EQ(
            Filesystem::GetFileType("MyDirectory"),
            Filesystem::FileType::Directory);

        EXPECT_FALSE(Filesystem::IsRegularFile("MyDirectory"));
        EXPECT_TRUE(Filesystem::IsDirectory("MyDirectory"));
        EXPECT_FALSE(Filesystem::IsSymbolicLink("MyDirectory"));

        std::filesystem::remove("MyDirectory");
        std::filesystem::remove("MyFile");
    }
}
