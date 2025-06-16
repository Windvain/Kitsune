#include <gtest/gtest.h>
#include "TestContainer.h"

#include "CompareStrings.h"
#include "PlatformStrings.h"
#include "IteratorWrappers.h"

#include "Foundation/Filesystem/WindowsPath.h"

using namespace Kitsune;
using namespace Kitsune::Filesystem;

using namespace Testing;

TEST(WindowsPathTests, DefaultCtor)
{
    WindowsPath path;
    EXPECT_TRUE(path.Native().IsEmpty());
}

TEST(WindowsPathTests, StringCtor)
{
    UTF16_STR str = MAKE_UTF16("C:/Windows/SysWOW64");
    WindowsPath path = Move(str);

    EXPECT_EQ(str.Size(), 0);
    EXPECT_GENERAL_STREQ(path.Raw(), MAKE_UTF16("C:/Windows/SysWOW64"));
}

TEST(WindowsPathTests, CstringCtor)
{
    WindowsPath path = MAKE_UTF16("C:/Windows/System32");
    EXPECT_GENERAL_STREQ(path.Raw(), MAKE_UTF16("C:/Windows/System32"));
}

TEST(WindowsPathTests, StringViewCtor)
{
    UTF16_STRV strpath = MAKE_UTF16("C:/Windows/System32");
    WindowsPath path = strpath;

    EXPECT_GENERAL_STREQ(path.Raw(), strpath.Data());
}

TEST(WindowsPathTests, RangeCtor)
{
    UTF16_STRV str = MAKE_UTF16("C:\\Windows\\");
    WindowsPath path = { str.GetBegin(), str.GetEnd() };

    EXPECT_GENERAL_STREQ(path.Raw(), str.Data());
}

TEST(WindowsPathTests, MoveStrAssign)
{
    UTF16_STR str = MAKE_UTF16("C:/Windows/SysWOW64");
    WindowsPath path = MAKE_UTF16("i/am/a/path");

    path = Move(str);

    EXPECT_EQ(str.Size(), 0);
    EXPECT_GENERAL_STREQ(path.Raw(), MAKE_UTF16("C:/Windows/SysWOW64"));
}

TEST(WindowsPathTests, CstringAssign)
{
    WindowsPath path = MAKE_UTF16("i/am/a/path.txt");
    path = MAKE_UTF16("C:/Windows/System32");

    EXPECT_GENERAL_STREQ(path.Raw(), MAKE_UTF16("C:/Windows/System32"));
}

TEST(WindowsPathTests, StringViewAssign)
{
    UTF16_STRV strpath = MAKE_UTF16("C:/Windows/System32");
    WindowsPath path = MAKE_UTF16("gsadfsd/fsfwd/trtaad/fs");

    path = strpath;
    EXPECT_GENERAL_STREQ(path.Raw(), strpath.Data());
}

TEST(WindowsPathTests, GetDrivePath)
{
    WindowsPath wslPath = MAKE_UTF16("//wsl.localhost/Ubuntu");
    WindowsPath uncPath = MAKE_UTF16("\\\\server\\shared\\file.txt");
    WindowsPath uncPath2 = MAKE_UTF16("\\\\?\\server\\shared//file");
    WindowsPath regPath = MAKE_UTF16("X:\\Path/To/File");
    WindowsPath regPath2 = MAKE_UTF16("X:Path/To/File");
    WindowsPath dosPath = MAKE_UTF16("//./COM56");

    WindowsPath noDrivePath = MAKE_UTF16("i/am/a/regular/file/path");

    EXPECT_GENERAL_STREQ(wslPath.GetDrivePath().Raw(), MAKE_UTF16("//wsl.localhost"));
    EXPECT_GENERAL_STREQ(uncPath.GetDrivePath().Raw(), MAKE_UTF16("\\\\server"));
    EXPECT_GENERAL_STREQ(uncPath2.GetDrivePath().Raw(), MAKE_UTF16("\\\\?"));
    EXPECT_GENERAL_STREQ(regPath.GetDrivePath().Raw(), MAKE_UTF16("X:"));
    EXPECT_GENERAL_STREQ(regPath2.GetDrivePath().Raw(), MAKE_UTF16("X:"));
    EXPECT_GENERAL_STREQ(dosPath.GetDrivePath().Raw(), MAKE_UTF16("//."));
    EXPECT_GENERAL_STREQ(noDrivePath.GetDrivePath().Raw(), MAKE_UTF16(""));
}

TEST(WindowsPathTests, GetRootPath)
{
    WindowsPath wslPath = MAKE_UTF16("//wsl.localhost/Ubuntu");
    WindowsPath uncPath = MAKE_UTF16("\\\\server\\shared\\file.txt");
    WindowsPath uncPath2 = MAKE_UTF16("\\\\?\\server\\shared//file");
    WindowsPath regPath = MAKE_UTF16("X:\\Path/To/File");
    WindowsPath regPath2 = MAKE_UTF16("X:Path/To/File");
    WindowsPath dosPath = MAKE_UTF16("//./COM56");

    WindowsPath noDrivePath = MAKE_UTF16("i/am/a/regular/file/path");

    EXPECT_GENERAL_STREQ(wslPath.GetRootPath().Raw(), MAKE_UTF16("/"));
    EXPECT_GENERAL_STREQ(uncPath.GetRootPath().Raw(), MAKE_UTF16("\\"));
    EXPECT_GENERAL_STREQ(uncPath2.GetRootPath().Raw(), MAKE_UTF16("\\"));
    EXPECT_GENERAL_STREQ(regPath.GetRootPath().Raw(), MAKE_UTF16("\\"));
    EXPECT_GENERAL_STREQ(regPath2.GetRootPath().Raw(), MAKE_UTF16(""));
    EXPECT_GENERAL_STREQ(dosPath.GetRootPath().Raw(), MAKE_UTF16("/"));
    EXPECT_GENERAL_STREQ(noDrivePath.GetRootPath().Raw(), MAKE_UTF16(""));
}

TEST(WindowsPathTests, GetRelativePath)
{
    WindowsPath wslPath = MAKE_UTF16("//wsl.localhost/Ubuntu");
    WindowsPath uncPath = MAKE_UTF16("\\\\server\\shared\\file.txt");
    WindowsPath uncPath2 = MAKE_UTF16("\\\\?\\server\\shared//file");
    WindowsPath regPath = MAKE_UTF16("X:\\Path/To/File");
    WindowsPath regPath2 = MAKE_UTF16("X:Path/To/File");
    WindowsPath dosPath = MAKE_UTF16("//./COM56");

    WindowsPath noDrivePath = MAKE_UTF16("i/am/a/regular/file/path");

    EXPECT_GENERAL_STREQ(wslPath.GetRelativePath().Raw(), MAKE_UTF16("Ubuntu"));
    EXPECT_GENERAL_STREQ(uncPath.GetRelativePath().Raw(), MAKE_UTF16("shared\\file.txt"));
    EXPECT_GENERAL_STREQ(uncPath2.GetRelativePath().Raw(), MAKE_UTF16("server\\shared//file"));
    EXPECT_GENERAL_STREQ(regPath.GetRelativePath().Raw(), MAKE_UTF16("Path/To/File"));
    EXPECT_GENERAL_STREQ(regPath2.GetRelativePath().Raw(), MAKE_UTF16("Path/To/File"));
    EXPECT_GENERAL_STREQ(dosPath.GetRelativePath().Raw(), MAKE_UTF16("COM56"));
    EXPECT_GENERAL_STREQ(noDrivePath.GetRelativePath().Raw(), MAKE_UTF16("i/am/a/regular/file/path"));
}

TEST(WindowsPathTests, GetParentPath)
{
    WindowsPath wslPath = MAKE_UTF16("//wsl.localhost/Ubuntu/");
    WindowsPath uncPath = MAKE_UTF16("\\\\server\\shared\\file.txt");
    WindowsPath uncPath2 = MAKE_UTF16("\\\\?\\server\\shared//file");
    WindowsPath regPath = MAKE_UTF16("X:\\Path/To/File");
    WindowsPath regPath2 = MAKE_UTF16("X:Path/To/File");
    WindowsPath dosPath = MAKE_UTF16("//./COM56");
    WindowsPath dosPath2 = MAKE_UTF16("//./");

    WindowsPath noDrivePath = MAKE_UTF16("i/am/a/regular/file/path");

    EXPECT_GENERAL_STREQ(wslPath.GetParentPath().Raw(), MAKE_UTF16("//wsl.localhost/Ubuntu"));
    EXPECT_GENERAL_STREQ(uncPath.GetParentPath().Raw(), MAKE_UTF16("\\\\server\\shared"));
    EXPECT_GENERAL_STREQ(uncPath2.GetParentPath().Raw(), MAKE_UTF16("\\\\?\\server\\shared"));
    EXPECT_GENERAL_STREQ(regPath.GetParentPath().Raw(), MAKE_UTF16("X:\\Path/To"));
    EXPECT_GENERAL_STREQ(regPath2.GetParentPath().Raw(), MAKE_UTF16("X:Path/To"));
    EXPECT_GENERAL_STREQ(dosPath.GetParentPath().Raw(), MAKE_UTF16("//./"));
    EXPECT_GENERAL_STREQ(dosPath2.GetParentPath().Raw(), MAKE_UTF16("//./"));
    EXPECT_GENERAL_STREQ(noDrivePath.GetParentPath().Raw(), MAKE_UTF16("i/am/a/regular/file"));
}

TEST(WindowsPathTests, IsEmpty)
{
    WindowsPath path = MAKE_UTF16("C:\\Program Files\\ASUS\\GlideX");
    WindowsPath empty;

    EXPECT_FALSE(path.IsEmpty());
    EXPECT_TRUE(empty.IsEmpty());
}

TEST(WindowsPathTests, IsAbsoluteRelative)
{
    WindowsPath wslPath = MAKE_UTF16("//wsl.localhost/Ubuntu/");
    WindowsPath uncPath = MAKE_UTF16("\\\\server\\shared\\file.txt");
    WindowsPath uncPath2 = MAKE_UTF16("\\\\?\\server\\shared//file");
    WindowsPath regPath = MAKE_UTF16("X:\\Path/To/File");
    WindowsPath regPath2 = MAKE_UTF16("X:Path/To/File");
    WindowsPath dosPath = MAKE_UTF16("//./COM56");
    WindowsPath dosPath2 = MAKE_UTF16("//./");

    WindowsPath relative = MAKE_UTF16("Path/To/File");
    WindowsPath relative2 = MAKE_UTF16("./Path/To/File");
    WindowsPath relative3 = MAKE_UTF16("../Path/To/File");

    EXPECT_TRUE(wslPath.IsAbsolute());
    EXPECT_TRUE(uncPath.IsAbsolute());
    EXPECT_TRUE(uncPath2.IsAbsolute());
    EXPECT_TRUE(regPath.IsAbsolute());
    EXPECT_TRUE(regPath2.IsAbsolute());
    EXPECT_TRUE(dosPath.IsAbsolute());
    EXPECT_TRUE(dosPath2.IsAbsolute());

    EXPECT_FALSE(wslPath.IsRelative());
    EXPECT_FALSE(uncPath.IsRelative());
    EXPECT_FALSE(uncPath2.IsRelative());
    EXPECT_FALSE(regPath.IsRelative());
    EXPECT_FALSE(regPath2.IsRelative());
    EXPECT_FALSE(dosPath.IsRelative());
    EXPECT_FALSE(dosPath2.IsRelative());

    EXPECT_FALSE(relative.IsAbsolute());
    EXPECT_FALSE(relative2.IsAbsolute());
    EXPECT_FALSE(relative3.IsAbsolute());

    EXPECT_TRUE(relative.IsRelative());
    EXPECT_TRUE(relative2.IsRelative());
    EXPECT_TRUE(relative3.IsRelative());
}

TEST(WindowsPathTests, HasDrivePath)
{
    WindowsPath wslPath = MAKE_UTF16("//wsl.localhost/Ubuntu");
    WindowsPath uncPath = MAKE_UTF16("\\\\server\\shared\\file.txt");
    WindowsPath uncPath2 = MAKE_UTF16("\\\\?\\server\\shared//file");
    WindowsPath regPath = MAKE_UTF16("X:\\Path/To/File");
    WindowsPath regPath2 = MAKE_UTF16("X:Path/To/File");
    WindowsPath dosPath = MAKE_UTF16("//./COM56");

    WindowsPath noDrivePath = MAKE_UTF16("i/am/a/regular/file/path");

    EXPECT_TRUE(wslPath.HasDrivePath());
    EXPECT_TRUE(uncPath.HasDrivePath());
    EXPECT_TRUE(uncPath2.HasDrivePath());
    EXPECT_TRUE(regPath.HasDrivePath());
    EXPECT_TRUE(regPath2.HasDrivePath());
    EXPECT_TRUE(dosPath.HasDrivePath());

    EXPECT_FALSE(noDrivePath.HasDrivePath());
}

TEST(WindowsPathTests, HasRootPath)
{
    WindowsPath wslPath = MAKE_UTF16("//wsl.localhost/Ubuntu");
    WindowsPath uncPath = MAKE_UTF16("\\\\server\\shared\\file.txt");
    WindowsPath uncPath2 = MAKE_UTF16("\\\\?\\server\\shared//file");
    WindowsPath regPath = MAKE_UTF16("X:\\Path/To/File");
    WindowsPath regPath2 = MAKE_UTF16("X:Path/To/File");
    WindowsPath dosPath = MAKE_UTF16("//./COM56");

    WindowsPath noDrivePath = MAKE_UTF16("i/am/a/regular/file/path");

    EXPECT_TRUE(wslPath.HasRootPath());
    EXPECT_TRUE(uncPath.HasRootPath());
    EXPECT_TRUE(uncPath2.HasRootPath());
    EXPECT_TRUE(regPath.HasRootPath());
    EXPECT_FALSE(regPath2.HasRootPath());
    EXPECT_TRUE(dosPath.HasRootPath());

    EXPECT_FALSE(noDrivePath.HasRootPath());
}

TEST(WindowsPathTests, HasRelativePath)
{
    WindowsPath wslPath = MAKE_UTF16("//wsl.localhost/Ubuntu");
    WindowsPath uncPath = MAKE_UTF16("\\\\server\\shared\\file.txt");
    WindowsPath uncPath2 = MAKE_UTF16("\\\\?\\");
    WindowsPath regPath = MAKE_UTF16("X:\\Path/To/File");
    WindowsPath regPath2 = MAKE_UTF16("X:");
    WindowsPath dosPath = MAKE_UTF16("//./COM56");

    WindowsPath noDrivePath = MAKE_UTF16("i/am/a/regular/file/path");
    WindowsPath empty;

    EXPECT_TRUE(wslPath.HasRelativePath());
    EXPECT_TRUE(uncPath.HasRelativePath());
    EXPECT_FALSE(uncPath2.HasRelativePath());
    EXPECT_TRUE(regPath.HasRelativePath());
    EXPECT_FALSE(regPath2.HasRelativePath());
    EXPECT_TRUE(dosPath.HasRelativePath());

    EXPECT_TRUE(noDrivePath.HasRelativePath());
    EXPECT_FALSE(empty.HasRelativePath());
}

TEST(WindowsPathTests, HasParentPath)
{
    WindowsPath wslPath = MAKE_UTF16("//wsl.localhost/Ubuntu");
    WindowsPath uncPath = MAKE_UTF16("\\\\server\\shared\\file.txt");
    WindowsPath uncPath2 = MAKE_UTF16("\\\\?\\");
    WindowsPath regPath = MAKE_UTF16("X:\\Path/To/File");
    WindowsPath regPath2 = MAKE_UTF16("X:");
    WindowsPath dosPath = MAKE_UTF16("//./COM56");

    WindowsPath noDrivePath = MAKE_UTF16("i/am/a/regular/file/path");
    WindowsPath empty;

    EXPECT_TRUE(wslPath.HasParentPath());
    EXPECT_TRUE(uncPath.HasParentPath());
    EXPECT_TRUE(uncPath2.HasParentPath());
    EXPECT_TRUE(regPath.HasParentPath());
    EXPECT_TRUE(regPath2.HasParentPath());
    EXPECT_TRUE(dosPath.HasParentPath());

    EXPECT_TRUE(noDrivePath.HasParentPath());
    EXPECT_FALSE(empty.HasParentPath());
}

TEST(WindowsPathTests, Clear)
{
    WindowsPath path = MAKE_UTF16("X:/A/B");
    path.Clear();

    EXPECT_TRUE(path.IsEmpty());
}
