#include <gtest/gtest.h>

#if defined(_WIN32)
    #include <Windows.h>
#endif

#include "Foundation/Filesystem/WindowsPath.h"

using namespace Kitsune;

TEST(WindowsPathTests, DefaultCtor)
{
    WindowsPath path;
    EXPECT_TRUE(path.Native().IsEmpty());
}

TEST(WindowsPathTests, CStringCtor)
{
    WindowsPath path = L"C:/Windows/System32";
    EXPECT_STREQ(path.Native().Raw(), L"C:/Windows/System32");
}

TEST(WindowsPathTests, StringViewCtor)
{
    WideStringView strpath = L"C:/Windows/System32";
    WindowsPath path = strpath;

    EXPECT_STREQ(path.Native().Raw(), strpath.Data());
}

TEST(WindowsPathTests, StringMoveCtor)
{
    WindowsPath path = WindowsPath(WideString(L"C:/Windows/System32/kernel32.dll"));
    EXPECT_STREQ(path.Native().Raw(), L"C:/Windows/System32/kernel32.dll");
}

TEST(WindowsPathTests, Clear)
{
    WindowsPath path = L"X:/A/B";
    path.Clear();

    EXPECT_STREQ(path.Native().Raw(), L"X:/A/B");
}

#if defined(_WIN32)
TEST(WindowsPathTests, Exists)
{
    WindowsPath path = L"kitsune_temp.temp";
    EXPECT_FALSE(path.Exists());

    HANDLE fd = ::CreateFileW(path.Native().Raw(), GENERIC_READ | GENERIC_WRITE,
                              0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

    EXPECT_NE(fd, INVALID_HANDLE_VALUE);
    EXPECT_TRUE(path.Exists());

    ::CloseHandle(fd);
    ::DeleteFileW(path.Native().Raw());
}

TEST(WindowsPathTests, IsFileDirectory)
{
    WindowsPath file = L"C:/Windows/System32/kernel32.dll";
    WindowsPath dir = L"C:/Windows";

    EXPECT_TRUE(file.IsFile());
    EXPECT_FALSE(file.IsDirectory());

    EXPECT_TRUE(dir.IsDirectory());
    EXPECT_FALSE(dir.IsFile());
}

TEST(WindowsPathTests, IsAbsoluteRelative)
{
    WindowsPath wslPath = L"\\\\wsl.localhost\\Ubuntu";
    WindowsPath uncPath = L"\\\\server\\shared\\file";
    WindowsPath uncPath2 = L"\\\\?\\server\\shared//file";
    WindowsPath drivePath = L"X:/Path\\to/file";
    WindowsPath drivePath2 = L"X:";
    WindowsPath devicePath = L"\\\\.\\COM4";

    WindowsPath relPath = L"./path/to/file";
    WindowsPath relPath2 = L"../path\\to\\file";

    EXPECT_TRUE(wslPath.IsAbsolute());
    EXPECT_TRUE(uncPath.IsAbsolute());
    EXPECT_TRUE(uncPath2.IsAbsolute());
    EXPECT_TRUE(drivePath.IsAbsolute());
    EXPECT_TRUE(drivePath2.IsAbsolute());
    EXPECT_TRUE(devicePath.IsAbsolute());

    EXPECT_FALSE(relPath.IsAbsolute());
    EXPECT_FALSE(relPath2.IsAbsolute());

    EXPECT_FALSE(wslPath.IsRelative());
    EXPECT_FALSE(uncPath.IsRelative());
    EXPECT_FALSE(uncPath2.IsRelative());
    EXPECT_FALSE(drivePath.IsRelative());
    EXPECT_FALSE(drivePath2.IsRelative());
    EXPECT_FALSE(devicePath.IsRelative());

    EXPECT_TRUE(relPath.IsRelative());
    EXPECT_TRUE(relPath.IsRelative());
}
#endif
