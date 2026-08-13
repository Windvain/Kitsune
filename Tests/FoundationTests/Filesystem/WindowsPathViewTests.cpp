#include <gtest/gtest.h>
#include "Foundation/Filesystem/PathView.h"

using namespace Kitsune;

namespace
{
    TEST(PathViewTest, DefaultConstructor)
    {
        PathView pathView;
        EXPECT_EQ(pathView.Data(), nullptr);
        EXPECT_EQ(pathView.Size(), 0);
    }

    TEST(PathViewTest, CStringConstructor)
    {
        const char* string = "My/Path/To/A/File";
        PathView pathView = string;

        EXPECT_STREQ(pathView.Data(), string);
        EXPECT_EQ(pathView.Size(), 17);
    }

    TEST(PathViewTest, CStringSizeConstructor)
    {
        const char* string = "My/Path/To/A/File";
        PathView pathView(string, 12);

        for (int index = 0; index < 12; ++index)
            EXPECT_EQ(pathView.Data()[index], string[index]);

        EXPECT_STREQ(pathView.Data(), string);
        EXPECT_EQ(pathView.Size(), 12);
    }

    TEST(PathViewTest, StringViewConstructor)
    {
        StringView string = "My/Path/To/A/File";
        PathView pathView = string;

        for (int index = 0; index < 12; ++index)
            EXPECT_EQ(pathView.Data()[index], string.Data()[index]);

        EXPECT_EQ(pathView.Size(), 17);
    }

    TEST(PathViewTest, RangeConstructor)
    {
        std::string string = "I/Am/A/Path";
        PathView path(string.data(), string.data() + string.size());

        EXPECT_EQ(path.Data(), string.data());
        EXPECT_EQ(path.Size(), string.size());
    }

    /* PathView::PathView(const PathView&), PathView::~PathView(), and
     * PathView::operator=(const PathView&) are defaulted.
     * */

    TEST(PathViewTest, StringViewImplicitCast)
    {
        PathView path = "Another/Path/I/Am";
        StringView string = path;

        EXPECT_EQ(string.Data(), path.Data());
        EXPECT_EQ(string.Size(), path.Size());
    }

    /* PathView::Size(), PathView::IsEmpty(), PathView::Data(), and PathView::View()
     * are expected to work.
     * */

    TEST(PathViewTest, Native)
    {
        PathView path = "Im/Also/Another/Path/😁";
        NativeString native = path.Native();

        EXPECT_STREQ(native.Raw(), L"Im/Also/Another/Path/😁");
    }

    TEST(PathViewTest, IsValidPath)
    {
        PathView invalidPaths[] = {
            "I/Am/A</Path",
            "I/Am/A>/Path",
            "I/Am/A:/Path",
            "I/Am/A\"/Path",
            "I/Am/A|/Path",
            "I/Am/A?/Path",
            "I/Am/A*/Path",
            "I/Am/A\a/Path",
            "I/Am/A\b/Path",
        };

        for (const PathView& path : invalidPaths)
            EXPECT_FALSE(path.IsValidPath());

        PathView validPaths[] = {
            "I/Am/A/Path",
            R"(\\?\D:\A\Path)"
        };

        for (const PathView& path : validPaths)
            EXPECT_TRUE(path.IsValidPath());
    }

    TEST(PathViewTest, IsAbsoluteRelative)
    {
        PathView absolutePaths[] = {
            "D:/",
            "Z:\\Food",
            R"(\\.\C:/Windows)",
            R"(\\?\F:/File/)",
            R"(\\LOCALHOST/G:\File\File2)"
        };

        PathView relativePaths[] = {
            "First/Second",
            "First\\Second",
            "/File",
            "\\File.txt",
            "C:Windows",
        };

        for (const PathView& path : absolutePaths)
        {
            EXPECT_TRUE(path.IsAbsolute());
            EXPECT_FALSE(path.IsRelative());
        }

        for (const PathView& path : relativePaths)
        {
            EXPECT_FALSE(path.IsAbsolute());
            EXPECT_TRUE(path.IsRelative());
        }
    }

    TEST(PathViewTest, GetRootName)
    {
        std::tuple<PathView, PathView> paths[] = {
            { "C:/Program Files/", "C:" },
            { R"(\\?\D:/Folder/)", R"(\\?\D:)" },
            { R"(\\.\F:/)", R"(\\.\F:)" },
            { R"(\\LOCALHOST\C$\Program Files)", R"(\\LOCALHOST)" },
            { "C:Program Files/", "C:" },
            { R"(\\?\D:Folder/)", R"(\\?\D:)" },
            { R"(\\.\F:Data)", R"(\\.\F:)" },
            { "/Program Files (x86)\\", "" },
            { "Program Files/Windows", "" }
        };

        for (const auto& [path, res] : paths)
        {
            PathView rootName = path.GetRootName();
            EXPECT_EQ(rootName.Size(), res.Size());
            EXPECT_EQ(rootName.View(), res.View());
        }
    }

    TEST(PathViewTest, GetRootDirectory)
    {
        std::tuple<PathView, PathView> paths[] = {
            { "C:/Program Files/", "/" },
            { R"(\\?\D:/Folder/)", "/" },
            { R"(\\.\F:\)", "\\" },
            { R"(\\LOCALHOST\C$\Program Files)", "\\" },
            { "C:Program Files/", "" },
            { R"(\\?\D:Folder/)", "" },
            { R"(\\.\F:Data)", "" },
            { "/Program Files (x86)\\", "/" },
            { "Program Files/Windows", "" }
        };

        for (const auto& [path, res] : paths)
        {
            PathView rootDir = path.GetRootDirectory();
            EXPECT_EQ(rootDir.Size(), res.Size());
            EXPECT_EQ(rootDir.View(), res.View());
        }
    }

    TEST(PathViewTest, GetRootPath)
    {
        std::tuple<PathView, PathView> paths[] = {
            { "C:/Program Files/", "C:/" },
            { R"(\\?\D:/Folder/)", R"(\\?\D:/)" },
            { R"(\\.\F:\)", R"(\\.\F:\)" },
            { R"(\\LOCALHOST\C$\Program Files)", R"(\\LOCALHOST\)" },
            { "C:Program Files/", "C:" },
            { R"(\\?\D:Folder/)", R"(\\?\D:)" },
            { R"(\\.\F:Data)", R"(\\.\F:)" },
            { "/Program Files (x86)\\", "/" },
            { "Program Files/Windows", "" }
        };

        for (const auto& [path, res] : paths)
        {
            PathView rootPath = path.GetRootPath();
            EXPECT_EQ(rootPath.Size(), res.Size());
            EXPECT_EQ(rootPath.View(), res.View());
        }
    }

    TEST(PathViewTest, GetRelativePath)
    {
        std::tuple<PathView, PathView> paths[] = {
            { "C:/Program Files/", "Program Files/" },
            { R"(\\?\D:/Folder/)", "Folder/" },
            { R"(\\.\F:\)", "" },
            { R"(\\LOCALHOST\C$\Program Files)", R"(C$\Program Files)" },
            { "C:Program Files/", "Program Files/" },
            { R"(\\?\D:Folder/)", R"(Folder/)" },
            { R"(\\.\F:Data)", R"(Data)" },
            { "/Program Files (x86)\\", "Program Files (x86)\\" },
            { "Program Files/Windows", "Program Files/Windows" }
        };

        for (const auto& [path, res] : paths)
        {
            PathView relPath = path.GetRelativePath();
            EXPECT_EQ(relPath.Size(), res.Size());
            EXPECT_EQ(relPath.View(), res.View());
        }
    }

    TEST(PathViewTest, GetParentPath)
    {
        std::tuple<PathView, PathView> paths[] = {
            { "C:/Program Files/", "C:/Program Files" },
            { R"(\\?\D:/Folder)", R"(\\?\D:/)" },
            { R"(\\.\F:\)", R"(\\.\F:\)" },
            { R"(\\LOCALHOST\)", R"(\\LOCALHOST\)" },
            { "Folder1/Folder2/Folder3/File", "Folder1/Folder2/Folder3" },
            { "Folder1", "" }
        };

        for (const auto& [path, res] : paths)
        {
            PathView parentPath = path.GetParentPath();
            EXPECT_EQ(parentPath.Size(), res.Size());
            EXPECT_EQ(parentPath.View(), res.View());
        }
    }

    TEST(PathViewTest, GetFilename)
    {
        std::tuple<PathView, PathView> paths[] = {
            { "C:/Program Files", "Program Files" },
            { R"(\\?\D:/Folder/)", "" },
            { R"(\\.\F:\)", "" },
            { R"(F:)", "" }
        };

        for (const auto& [path, res] : paths)
        {
            PathView filename = path.GetFilename();
            EXPECT_EQ(filename.Size(), res.Size());
            EXPECT_EQ(filename.View(), res.View());
        }
    }

    TEST(PathViewTest, GetFileStem)
    {
        std::tuple<PathView, PathView> paths[] = {
            { "C:/File.txt", "File" },
            { R"(\\?\D:/Folder/)", "" },
            { "My/Path\\Path2/.dotfile", ".dotfile" },
            { R"(F:File)", "File" }
        };

        for (const auto& [path, res] : paths)
        {
            PathView stem = path.GetFileStem();
            EXPECT_EQ(stem.Size(), res.Size());
            EXPECT_EQ(stem.View(), res.View());
        }
    }

    TEST(PathViewTest, GetExtension)
    {
        std::tuple<PathView, PathView> paths[] = {
            { "C:/File.txt", ".txt" },
            { R"(\\?\D:/Folder/)", "" },
            { "My/Path\\Path2/.dotfile", "" },
            { R"(F:File)", "" }
        };

        for (const auto& [path, res] : paths)
        {
            PathView extension = path.GetExtension();
            EXPECT_EQ(extension.Size(), res.Size());
            EXPECT_EQ(extension.View(), res.View());
        }
    }
}
