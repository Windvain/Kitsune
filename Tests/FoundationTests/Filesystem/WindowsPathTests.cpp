#include <gtest/gtest.h>
#include "Foundation/Filesystem/Path.h"

using namespace Kitsune;
using namespace Kitsune::Filesystem;

#if !defined(_WIN32)
    #error This test file is only meant for Windows.
#endif

namespace
{
    // Path::Path()
    TEST(PathTest, DefaultConstructor)
    {
        Path path;
        EXPECT_TRUE(path.IsEmpty());
    }

    // Path::Path(StringType&&)
    TEST(PathTest, StringMoveConstructor)
    {
        String string = "C:/Windows/Fonts/";
        auto* pointer = string.Raw();

        Path path = std::move(string);
        EXPECT_STREQ(path.Raw(), "C:/Windows/Fonts/");
    }

    // Path::Path(const char*)
    TEST(PathTest, CStringStrvLikeConstructor)
    {
        const char* string = "C:/Program Files (x86)/Microsoft/";
        Path path = string;

        EXPECT_STREQ(path.Raw(), string);
    }

    // Path::Path(StringView)
    TEST(PathTest, StringViewStrvLikeConstructor)
    {
        StringView string = "C:/Program Files (x86)/Microsoft/";
        Path path = string;

        EXPECT_STREQ(path.Raw(), string.Data());
    }

    // Path::Path(PathView)
    TEST(PathTest, PathViewStrvLikeConstructor)
    {
        const char* string = "C:/Program Files (x86)/Microsoft/";
        Path path = string;

        EXPECT_STREQ(path.Raw(), string);
    }

    /* Path::Path(const Path&), Path::Path(Path&&), Path::~Path(),
     * Path::operator=(const Path&), and Path::operator=(Path&&) are set to `= default`.
     * */

    // Path::operator=(StringType&&)
    TEST(PathTest, StringMoveAssign)
    {
        Path path("C:/Users/desktop.ini");
        String string = "./PathTests.cpp";

        const char* pointer = string.Raw();
        path = Move(string);

        EXPECT_STREQ(path.Raw(), "./PathTests.cpp");
    }

    // Path::operator=(PathView)
    TEST(PathTest, PathViewStrvLikeAssign)
    {
        Path path("C:/Users/desktop.ini");
        PathView view = "../../CMakeLists.txt";

        path = view;
        EXPECT_STREQ(view.Data(), path.Raw());
    }

    // Path::operator=(StringView)
    TEST(PathTest, StringViewStrvLikeAssign)
    {
        Path path("C:/Users/desktop.ini");
        StringView string = "../../CMakeLists.txt";

        path = string;
        EXPECT_STREQ(path.Raw(), "../../CMakeLists.txt");
    }

    // Path::operator=(const char*)
    TEST(PathTest, CStringStrvLikeAssign)
    {
        Path path("C:/Users/desktop.ini");
        const char* string = "../../CMakeLists.txt";

        path = string;
        EXPECT_STREQ(path.Raw(), "../../CMakeLists.txt");
    }

    // operator String() const
    TEST(PathTest, StringImplicitCast)
    {
        Path path = "D:/$RECYCLE.BIN";
        String string = path;

        EXPECT_STREQ(string.Raw(), "D:/$RECYCLE.BIN");
    }

    // operator PathView() const
    TEST(PathTest, PathViewImplicitCast)
    {
        Path path = "C:/PerfLogs/";
        PathView view = path;

        EXPECT_STREQ(view.Data(), path.Raw());
    }

    // Path::operator+=(const PathView&)
    TEST(PathTest, AppendAssignPath)
    {
        Path path("D:/Dev/Example");
        Path appended("Project/CMakeLists.txt");

        path += appended;
        EXPECT_STREQ(path.Raw(), "D:/Dev/ExampleProject/CMakeLists.txt");
    }

    // Path::operator+=(const StringView&)
    TEST(PathTest, AppendAssignStringView)
    {
        Path path("D:/Dev/Example");
        StringView string = "Project/CMakeLists.txt";

        path += string;
        EXPECT_STREQ(path.Raw(), "D:/Dev/ExampleProject/CMakeLists.txt");
    }

    // Path::operator+=(const char*)
    TEST(PathTest, AppendAssignCString)
    {
        Path path("D:/Dev/Example");
        const char* string = "Project/CMakeLists.txt";

        path += string;
        EXPECT_STREQ(path.Raw(), "D:/Dev/ExampleProject/CMakeLists.txt");
    }

    // Path::operator+(const Path&)
    TEST(PathTest, AppendPath)
    {
        const Path path("D:/Dev/Example");
        const Path appended("Project/CMakeLists.txt");

        Path result = path + appended;
        EXPECT_STREQ(result.Raw(), "D:/Dev/ExampleProject/CMakeLists.txt");
    }

    // Path::operator+(const PathView&)
    TEST(PathTest, AppendPathView)
    {
        const Path path("D:/Dev/Example");
        const PathView appended("Project/CMakeLists.txt");

        Path result = path + appended;
        EXPECT_STREQ(result.Raw(), "D:/Dev/ExampleProject/CMakeLists.txt");
    }

    // Path::operator+(const StringView&)
    TEST(PathTest, AppendStringView)
    {
        const Path path("D:/Dev/Example");
        const StringView appended("Project/CMakeLists.txt");

        Path result = path + appended;
        EXPECT_STREQ(result.Raw(), "D:/Dev/ExampleProject/CMakeLists.txt");
    }

    // Path::operator+(const char*)
    TEST(PathTest, AppendCString)
    {
        Path path("D:/Dev/Example");
        const char* string = "Project/CMakeLists.txt";

        Path result = path + string;
        EXPECT_STREQ(result.Raw(), "D:/Dev/ExampleProject/CMakeLists.txt");
    }

    // Path::operator/=(const Path& (Absolute))
    TEST(PathTest, ChangeDirAbsoluteAssign)
    {
        Path paths[5] = {
            "D:/Some/Random/Path.txt",
            "D:Example/Example2",
            "C:Example/Example2",
            "/Example/Example2",
            "Example/Example2"
        };

        for (Path& path : paths)
        {
            path /= Path("D:/Dev");
            EXPECT_STREQ(path.Raw(), "D:/Dev");
        }
    }

    // Path::operator/=(const Path& (Relative to current path))
    TEST(PathTest, ChangeDirRelativeCurrentPathAssign)
    {
        Path paths[5] = {
            "D:/Some/Random/Path.txt",
            "D:Example/Example2",
            "C:Example/Example2",
            "/Example/Example2",
            "Example/Example2"
        };

        Path results[5] = {
            "D:/Some/Random/Path.txt\\Example3/Example4",
            "D:Example/Example2\\Example3/Example4",
            "C:Example/Example2\\Example3/Example4",
            "/Example/Example2\\Example3/Example4",
            "Example/Example2\\Example3/Example4"
        };

        for (Index index = 0; index < 5; ++index)
        {
            paths[index] /= Path("Example3/Example4");
            EXPECT_STREQ(paths[index].Raw(), results[index].Raw());
        }
    }

    // Path::operator/=(const Path& (Relative to current path & drive))
    TEST(PathTest, ChangeDirRelativeCurrentPathDriveAssign)
    {
        Path paths[4] = {
            "D:/Some/Random/Path.txt",
            "C:Example/Example2",
            "/Example/Example2",
            "Example/Example2"
        };

        Path results[4] = {
            "D:/Some/Random/Path.txt\\Example3\\Example4",
            "D:Example3\\Example4",
            "D:Example3\\Example4",
            "D:Example3\\Example4"
        };

        for (Index index = 0; index < 4; ++index)
        {
            paths[index] /= Path("D:Example3\\Example4");
            EXPECT_STREQ(paths[index].Raw(), results[index].Raw());
        }
    }

    // Path::operator/=(const Path& (Relative to drive))
    TEST(PathTest, ChangeDirRelativeDriveAssign)
    {
        Path paths[5] = {
            "D:/Some/Random/Path.txt",
            "D:Example/Example2",
            "C:Example/Example2",
            "/Example/Example2",
            "Example/Example2"
        };

        Path results[5] = {
            "D:/Example3\\Example4",
            "D:/Example3\\Example4",
            "C:/Example3\\Example4",
            "/Example3\\Example4",
            "/Example3\\Example4"
        };

        for (Index index = 0; index < 5; ++index)
        {
            paths[index] /= Path("/Example3\\Example4");
            EXPECT_STREQ(paths[index].Raw(), results[index].Raw());
        }
    }

    // Path::operator/=(StringView)
    TEST(PathTest, ChangeDirAssignStringView)
    {
        Path path = "D:/Dev/";
        StringView string = "My/Example/Proj";

        path /= string;
        EXPECT_STREQ(path.Raw(), "D:/Dev/My/Example/Proj");
    }

    // Path::operator/=(const char*)
    TEST(PathTest, ChangeDirAssignCString)
    {
        Path path = "D:/Dev/";
        StringView string = "My/Example/Proj";

        path /= string;
        EXPECT_STREQ(path.Raw(), "D:/Dev/My/Example/Proj");
    }

    // Path::operator/(const Path&)
    TEST(PathTest, ChangeDirPath)
    {
        Path path = "D:/Dev";
        Path newPath = path / Path("My/Example/Project");

        EXPECT_STREQ(newPath.Raw(), "D:/Dev\\My/Example/Project");
    }

    // Path::operator/(StringView)
    TEST(PathTest, ChangeDirStringView)
    {
        Path path = "D:/Dev/Project/";
        StringView string = "Dir1/Dir2/File.txt";

        Path newPath = path / string;
        EXPECT_STREQ(newPath.Raw(), "D:/Dev/Project/Dir1/Dir2/File.txt");
    }

    // Path::operator/(const char*)
    TEST(PathTest, ChangeDirCString)
    {
        Path path = "D:/Dev/Project/";
        const char* string = "Dir1/Dir2/File.txt";

        Path newPath = path / string;
        EXPECT_STREQ(newPath.Raw(), "D:/Dev/Project/Dir1/Dir2/File.txt");
    }

    /* Path::IsValidPath(), Path::IsAbsolute(), Path::IsRelative(), Path::GetRootName(),
     * Path::GetRootDirectory(), Path::GetRootPath(), Path::GetRelativePath(),
     * Path::GetParentPath(), Path::GetFilename(), Path::GetFileStem(),
     * Path::GetExtension(), and Path::Native() are tested in PathViewTests.cpp.
     * */

    // Path::Size()
    TEST(PathTest, Size)
    {
        Path path = "D:/Documents/";
        EXPECT_EQ(path.Size(), 13);
    }

    // Path::IsEmpty()
    TEST(PathTest, IsEmpty)
    {
        Path path = "D:/Dev";
        Path empty = "";

        EXPECT_TRUE(empty.IsEmpty());
        EXPECT_FALSE(path.IsEmpty());
    }

    // Path::Clear()
    TEST(PathTest, Clear)
    {
        Path path = "D:/Dev/Example";
        ASSERT_NE(path.Size(), 0);

        path.Clear();
        EXPECT_EQ(path.Size(), 0);
    }

    // Path::Swap(Path&)
    TEST(PathTest, Swap)
    {
        Path path = "C:/Windows/Boot/";
        Path other = "Dev\\Example";

        path.Swap(other);

        EXPECT_STREQ(path.Raw(), "Dev\\Example");
        EXPECT_STREQ(other.Raw(), "C:/Windows/Boot/");
    }

    // Path::MakePreferredSeperator()
    TEST(PathTest, MakePreferredSeperator)
    {
        Path path = R"(\\?/C:\Windows/Boot/)";
        path.MakePreferredSeperator();

        EXPECT_STREQ(path.Raw(), "\\\\?\\C:\\Windows\\Boot\\");
    }

    // Path::RemoveFilename()
    TEST(PathTest, RemoveFilename)
    {
        Path filenames[] = {
            "C:/Dev/Important.txt",
            "C:/Dev/Important.txt/",
            "F:"
        };

        Path expected[] = {
            "C:/Dev/",
            "C:/Dev/Important.txt/",
            "F:"
        };

        for (int index = 0; index < 3; ++index)
        {
            filenames[index].RemoveFilename();
            EXPECT_STREQ(filenames[index].Raw(), expected[index].Raw());
        }
    }

    // Path::RemoveExtension()
    TEST(PathTest, RemoveExtension)
    {
        Path filenames[] = {
            "C:/Dev/Important.txt",
            "C:/.dotfile",
            "C:/Dev/Important",
            "F:/Dev/",
            "C:",
        };

        Path expected[] = {
            "C:/Dev/Important",
            "C:/.dotfile",
            "C:/Dev/Important",
            "F:/Dev/",
            "C:"
        };

        for (int index = 0; index < 5; ++index)
        {
            filenames[index].RemoveExtension();
            EXPECT_STREQ(filenames[index].Raw(), expected[index].Raw());
        }
    }

    // Path::ReplaceFilename(const Path&)
    TEST(PathTest, ReplaceFilename)
    {
        Path filenames[] = {
            "C:/Dev/Important.txt",
            "C:/.dotfile",
            "F:/Dev/",
            "C:",
        };

        Path expected[] = {
            "C:/Dev/Other.txt",
            "C:/Other.txt",
            "F:/Dev/",
            "C:"
        };

        for (int index = 0; index < 4; ++index)
        {
            // operator/=(const Path&) has been tested elsewhere, no need to test all
            // cases here, just verify that it does what we need it to do haha
            filenames[index].ReplaceFilename(Path("Other.txt"));
            EXPECT_STREQ(filenames[index].Raw(), expected[index].Raw());
        }
    }

    // Path::ReplaceExtension(const Path&)
    TEST(PathTest, ReplaceExtension)
    {
        Path filenames[] = {
            "C:/Dev/Important.txt",
            "C:/.dotfile",
            "C:/Dev/Important",
            "F:/Dev/",
            "C:",
        };

        Path expected[] = {
            "C:/Dev/Important.myext",
            "C:/.dotfile",
            "C:/Dev/Important",
            "F:/Dev/",
            "C:"
        };

        for (int index = 0; index < 5; ++index)
        {
            filenames[index].ReplaceExtension(".myext");
            EXPECT_STREQ(filenames[index].Raw(), expected[index].Raw());
        }
    }
}
